#ifndef mpm_utils_units_h_
#define mpm_utils_units_h_

#include "scalar.h"
#include <cmath>

#include <iostream>

namespace mpm{
  struct Units{
  	enum Unit{
  		None, 
  		Length,
  		Volume, 
  		VolumicMass,
  		Acceleration,
  		Velocity,
  		Time,
  		Frequency,
  		Viscosity,
  		Stress,
  		Torque
  	};

  Scalar L ; // Length
  Scalar G ; // Acceleration
  Scalar R ; // Density
 
  Scalar U ; // Velocity
  Scalar T ; // Time
  Scalar P ; // Stress
  Scalar M ; // Viscosity

  Units(); 
  
  void setTypical(Scalar len, Scalar acc, Scalar volMass);

  Scalar fromSI( Unit u) const;
  Scalar toSI(Unit u) const;
  };
}

#endif