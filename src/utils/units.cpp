

#include "units.h"


namespace mpm{

	static const Scalar s_L = 1.e-1 ; 
	static const Scalar s_G = 9.81 ; 
	static const Scalar s_R = 1.5e3 ; 

	static const Scalar s_U = std::sqrt( s_G * s_L ) ; 

	static const Scalar s_T = ( s_L / s_U ) ; 

	Units::Units()
	  : L(1), G(9.81), R(1.5e3)
	{
		setTypical(L,G,R);
	}

	void Units::setTypical(Scalar l, Scalar g, Scalar rho){
		L = l ; 
		G = g ; 
		R = rho ; 
		U = std::sqrt(G*L) ; 
		T = L / U ; 
		P = R * G * L ; 
		M = P * T ; 
	}

	Scalar Units::fromSI(Unit u) const{
		return 1. / toSI( u ) ; 
	}

	Scalar Units::toSI( Unit u ) const{
        switch( u ){
        	case Length:{ return 1. * L ; }
        	case Volume:{ return std::pow(L, 3.) ; }
        	case VolumicMass:{ return 1. * R ; }
        	case Acceleration:{ return 1. * G ; }
        	case Velocity: { return 1. * U ; }
        	case Time: { return 1. * T ; }
        	case Frequency: { return 1. / T ; }
        	case Viscosity: { return 1. * M ; }
        	case Stress: { return 1. * P ; }
        	case Torque: { return std::pow(L, 3.) * P ; }
        	case None: { return 1; }
        }
        return 1; 
	}

}