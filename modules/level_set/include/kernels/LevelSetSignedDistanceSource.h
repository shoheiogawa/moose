//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef LEVEL_SET_SIGNED_DISTANCE_SOURCE_H
#define LEVEL_SET_SIGNED_DISTANCE_SOURCE_H

#include "Kernel.h"

class LevelSetSignedDistanceSource;

template <>
InputParameters validParams<LevelSetSignedDistanceSource>();

class LevelSetSignedDistanceSource
  : public Kernel
{
public:
  LevelSetSignedDistanceSource(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  Real smoothStepFunction(Real psi) const;

  // Distance value variable and its gradient

  const VariableValue & _psi_0;

  // Value for smoothed distance function
  const Real _epsilon;

  // Scale for the interface velocity
  const Real _scale;
};

#endif // LEVEL_SET_SIGNED_DISTANCE_VELOCITY_H
