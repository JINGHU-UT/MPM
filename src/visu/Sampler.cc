/*
 * This file is part of Sand6, a C++ software for the continuous
 * simulation of granular materials. See http://gdaviet.fr
 *
 * Copyright 2016 Gilles Daviet <gilles.daviet@inria.fr>
 *
 * Sand6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.

 * Sand6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Sand6.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Sampler.hh"

#include "Offline.hh"

#include "geometry/tensor.h"
#include "simulate/phase.h"

#include <Eigen/Eigenvalues>

#include <random>
#include <iostream>

#define RAD_FAC 2

namespace mpm {

struct BallSampler {

	BallSampler()
	: e(rd()), dist(-1,1)
	{}

	void operator() (Vec3 &v) {
		do {
			v << dist(e), dist(e), dist(e) ;
		} while (v.squaredNorm() > 1. ) ;
	}

	void onSphere( Vec3& v ) {
		do {
			(*this)(v) ;
		} while( v.isZero() ) ;

		v /= v.norm() ;
	}

	std::random_device rd;
	std::default_random_engine e ;
	std::uniform_real_distribution<Scalar> dist;
} ;

void Sampler::move( )
{
	const Scalar dt = m_offline.frame_dt() ;
	const Phase& grains = m_offline.grains() ;

	const ID n = count() ;

#pragma omp parallel for
	for(ID i = 0 ; i < n ; ++i) {

		const Vec3 p0 = m_positions.col(i).cast< Scalar >()  ;
		const Vec3 p1 = p0 + dt * grains.velocity( p0 ) ;
		m_positions.col(i) = p1.cast< float >() ;

		// Orientation
		if( m_mode == Discs ) {
			Mat3 Wu, Du ;
			tensor_view( grains.sym_grad(p0) ).get( Du ) ;
			tensor_view( grains.spi_grad(p0) ).get( Wu ) ;

			Vec3 t0 ;
			if ( std::fabs(m_normals(i,0)) > 1.e-6 ) {
				t0 = Vec3( -m_normals(i,1), m_normals(i,0), 0 ).normalized() ;
			} else {
				t0 = Vec3( 0, -m_normals(i,2), m_normals(i,1) ).normalized() ;
			}
			Vec3 t1 = m_normals.col(i).cast<Scalar>().cross(t0) ;

			t0 += dt * ( Wu*t0 + m_offline.config().elongation*
						 ( Du*t0 - (Du.cwiseProduct(t0*t0.transpose()).sum())*t0) ) ;
			t1 += dt * ( Wu*t1 + m_offline.config().elongation*
						 ( Du*t1 - (Du.cwiseProduct(t1*t1.transpose()).sum())*t1) ) ;

			m_normals.col(i) = t0.cross(t1).normalized().cast<float>() ;
		}

	}

	// Predict particle positions
	const ID np = m_particlesCount ;
#pragma omp parallel for
	for(ID i = 0 ; i < np ; ++i) {
		m_predPos.col(i) += dt*grains.velocity(	m_predPos.col(i) )   ;
	}

}

void Sampler::compute_absolute()
{
	const Scalar noise = 1.e-1 ;

	const Particles& particles = m_offline.particles() ;
	const Phase& grains = m_offline.grains() ;

	assert( particles.count() == m_particlesCount ) ;
//	std::cerr << particles.count() << " vs " << m_particlesCount << std::endl ;

	const ID n = count() ;

#pragma omp parallel for
	for( ID i = 0 ; i < n ; ++i) {
		const unsigned pid = m_particleIds[i] ;

		const Vec3 p0 = particles.centers().col( pid ) ;
		const Vec3 p0_pred = m_predPos.col( pid ) ;

		m_offsets.col( i ) = m_positions.col(i).cast<Scalar>() - p0_pred ;

		Mat3 frame ;
		tensor_view( particles.frames().col(pid) ).get( frame ) ;
		frame *= RAD_FAC ;

		const Scalar vn = std::max( 1., Scalar( m_offsets.col(i).transpose() *  frame.inverse() * m_offsets.col(i) ) ) ;
		m_offsets.col( i ) /= std::sqrt(vn)  ;

		const Vec3 pos = m_offline.mesh().clamp_point(p0 + m_offsets.col(i) ) ;
		m_offsets.col( i ) = pos - p0 ;

		m_positions.col(i) = pos.cast< float >() ;

		Eigen::Vector3f grad_phi = grains.grad_phi( pos ).cast<float>() ;
		const float gn = grad_phi.norm() ;

		if( m_mode == Normal ) {
			if( gn > 1.e-4 ) {
				grad_phi /= gn ;
				Eigen::Quaternionf rot( Eigen::AngleAxisf( noise, m_normalNoise.col(i) ) ) ;
				m_normals.col(i) =  - ( rot * grad_phi ).normalized() ;
			}
			else
				m_normals.col(i).setZero() ;
		}

		if( m_mode == VelocityCut && m_positions.col(i)[1] < m_offline.config().box[1] * .5 )
		{
			m_visibility(i) = -1 ;
		}

		if( m_visibility(i) >= 0 ) {
			if ( m_mode == VelocityCut ) {
				m_visibility(i) = particles.velocities().col(pid).norm();
			} else {
				m_visibility(i) = std::max( 0., std::min( 1., 1. - grains.fraction(pos) ) ) ;
			}
		}
	}

}

void Sampler::reassign( )
{
	typedef Particles::Event Event ;

	const unsigned NoEvent = -1 ;
	const unsigned Destroy = -2 ;

	std::vector< unsigned > eventIds ;

	const Particles& particles = m_offline.particles() ;

	// Reset particle positions and reserve storage space
	unsigned nEvents = 0 ;
	for( const std::vector< Event > &events :  m_offline.events().log() ) {
		nEvents += events.size() ;
	}
	m_predPos.resize(3, m_particlesCount + nEvents );
	m_predPos.leftCols( m_particlesCount ) = particles.centers().leftCols( m_particlesCount ) ;

	const ID n = count() ;
	unsigned nParticles = m_particlesCount ;

	std::vector< unsigned > relocs ;

	for(const std::vector< Event > &events :  m_offline.events().log()) {

		unsigned splits = 0 ;
		unsigned merges = 0 ;
		unsigned removes = 0 ;

		// Assign event-ids to particles
		eventIds.assign( nParticles + events.size(), NoEvent );
		for( unsigned eId = 0 ; eId < events.size() ; ++eId ) {
			const Event& e = events[eId] ;

			if( e.type == Event::Split ) {
				++splits ;

				eventIds[ e.first  ] = eId ;

				m_predPos.col( e.second ) = m_predPos.col( e.first ) + e.dx ;
				m_predPos.col( e.first  ) = m_predPos.col( e.first ) - e.dx ;
			} else if( e.type == Event::Remove ) {
				++removes ;
			} else if( e.type == Event::Merge ) {
				++merges;

				eventIds[ e.first  ] = eId ;
				eventIds[ e.second ] = eId ;

				m_predPos.col( e.first  ) = m_predPos.col( e.second ) + e.dx ;
			}
		}

		if( splits > 0 )
		{

			//Split
#pragma omp parallel for
			for( ID i = 0 ; i < n ; ++i ) {

				const unsigned pid = m_particleIds[i] ;
				if( eventIds[ pid ] == NoEvent ) continue ;

				const Event& e = events[eventIds[pid]] ;

				if( e.type == Event::Split ) {

					const Vec3 dx = events[eventIds[pid]].dx ;

					//dx = new_center_2 - old_center = old_center - new_center_1
					//offset = p - old_center
					//offset_2 = p - new_center_2 = p - (dx + old_center) = p - dx
					//offset_1 = p - new_center_1 = p - (old_center - dx) = p + dx

					const Vec3 &off1 = m_offsets.col(i) + dx ;
					const Vec3 &off2 = m_offsets.col(i) - dx ;

					if( off1.squaredNorm() < off2.squaredNorm() ) {
						m_offsets.col(i) = off1 ;
					} else {
						m_offsets.col(i) = off2 ;
						m_particleIds[i] = e.second ;
					}
				}
			}

			nParticles += splits ;
		}

		if( merges > 0 )
		{

	#pragma omp parallel for
			for( ID i = 0 ; i < n ; ++i ) {

				const unsigned pid = m_particleIds[i] ;

				if( eventIds[ pid ] == NoEvent ) continue ;

				const Event& e = events[eventIds[pid]] ;

				if( e.type == Event::Merge ) {

					const Vec3 dx = events[eventIds[pid]].dx ;
					if( pid == e.first ) {
						m_offsets.col(i) -= dx ;
					} else {
						m_offsets.col(i) += dx ;
						m_particleIds[i] = e.first ;
					}
				}
			}

			// Reloc
			relocs.assign( nParticles, NoEvent );
			size_t reloc_src = nParticles ;
			for( unsigned eId = 0 ; eId < events.size() ; ++eId ) {
				const Event& e = events[eId] ;

				if( e.type == Event::Merge && e.second + merges < nParticles ) {
					// Find last non-emptym pos
					do {
						--reloc_src ;
					} while( reloc_src > e.second && eventIds[reloc_src] != NoEvent
							 && events[ eventIds[reloc_src] ].type == Event::Merge
							 && events[ eventIds[reloc_src] ].second == reloc_src
							 ) ;
					relocs[ reloc_src ] = e.second ;
					m_predPos.col( e.second ) = m_predPos.col( reloc_src ) ;
				}
			}

	#pragma omp parallel for
			for( ID i = 0 ; i < n ; ++i ) {

				const unsigned pid = m_particleIds[i] ;
				if( relocs[ pid ] == NoEvent ) continue ;

				m_particleIds[ i ] = relocs[pid] ;
			}

			nParticles -= merges ;
		}

		if( removes > 0 )
		{

			// Remove -- Impossible without destroyign samples
			relocs.assign( nParticles, NoEvent );
			for( unsigned eId = 0 ; eId < events.size() ; ++eId ) {
				const Event& e = events[eId] ;

				if( e.type == Event::Remove ) {
					relocs[ e.second ] = e.first ;
					relocs[ e.first ] = Destroy ;
					m_predPos.col( e.first ) = m_predPos.col( e.second ) ;
				}
			}
#pragma omp parallel for
			for( ID i = 0 ; i < n ; ++i ) {

				const unsigned pid = m_particleIds[i] ;

				if( relocs[ pid ] == NoEvent ) continue ;

				if( relocs[ pid ] == Destroy )  {
					m_particleIds[ i ] = 0 ;
					m_visibility(i) = -1 ;
					continue ;
				}

				m_particleIds[ i ] = relocs[pid] ;
			}

			nParticles -= removes ;
		}
	}

	m_particlesCount = nParticles ;

}

void Sampler::sampleParticles( unsigned nSamples )
{
	const Particles& particles = m_offline.particles() ;
	const ID n = particles.count() ;

	m_particleIds.resize( n * nSamples ) ;
	m_offsets.resize( 3, n * nSamples ) ;
	m_normalNoise.resize( 3, n * nSamples ) ;

	m_positions.resize( 3, n * nSamples ) ;
	m_normals.resize( 3, n * nSamples ) ;

	m_visibility.resize( n * nSamples ) ;
	m_visibility.setZero() ;

	Arr3 oris = m_offline.config().initialOri.array().max(1.e-12).sqrt() ;

#pragma omp parallel
	{
		BallSampler sampler ;

		#pragma omp for
		for(ID i = 0 ; i < n ; ++i) {

			Vec3 v ;
			Mat3 frame ;
			tensor_view( particles.frames().col(i) ).get( frame ) ;
			frame *= RAD_FAC ;

			for( unsigned j = 0 ; j < nSamples ; ++j )
			{
				const unsigned idx = i*nSamples +  j ;
				m_particleIds[idx] = i ;

				sampler(v) ;
				v *= std::sqrt(RAD_FAC) ;

				const Scalar vn = std::max( 1., Scalar( v.transpose() * frame.inverse() * v ) ) ;
				m_offsets.col( idx ) = v/std::sqrt(vn)  ;
				m_positions.col( idx ) = ( particles.centers().col(i) + m_offsets.col( idx )).cast<float>() ;

				sampler( v ) ;
				m_normalNoise.col( idx ) = v.cast< float >() ;


				if( m_mode == Discs ) {
					sampler( v ) ;
					m_normals.col( idx ) = (v.array() * oris).matrix().normalized().cast<float>() ;
				}

			}

		}
	}

	m_particlesCount = n ;
	m_predPos = particles.centers().leftCols( m_particlesCount ) ;
}


} // mpm
