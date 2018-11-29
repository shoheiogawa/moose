//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef LEVEL_SET_SIGNED_DISTANC_VELOCITY_H
#define LEVEL_SET_SIGNED_DISTANC_VELOCITY_H

#include "AuxKernel.h"

class LevelSetSignedDistancVelocity;

template <>
InputParameters validParams<LevelSetSignedDistancVelocity>();

class LevelSetSignedDistancVelocity
  : public AuxKernel
{
public:
  LevelSetSignedDistancVelocity(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  // Distance value variable and its gradient
  const VariableValue & _psi;
  const VariableGradient & _grad_psi;

  // For the selection of x, y, or z
  const MooseEnum & _component;

  // Scale for the interface velocity
  const Real _scale;
};

#endif // LEVEL_SET_SIGNED_DISTANC_VELOCITY_H
