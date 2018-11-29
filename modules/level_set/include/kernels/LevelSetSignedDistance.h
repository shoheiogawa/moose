//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef LEVEL_SET_SIGNED_DISTANCE_H
#define LEVEL_SET_SIGNED_DISTANCE_H

#include "Kernel.h"

class LevelSetSignedDistance;

template <>
InputParameters validParams<LevelSetSignedDistance>();

class LevelSetSignedDistance
  : public Kernel
{
public:
  LevelSetSignedDistance(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  const VariableValue & _u_0;
  const Real _epsilon;
};

#endif // LEVEL_SET_SIGNED_DISTANC_SOURCE_H
