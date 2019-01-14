//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LevelSetSignedDistanceSource.h"

#include <cmath>

registerMooseObject("LevelSetApp", LevelSetSignedDistanceSource);

template <>
InputParameters
validParams<LevelSetSignedDistanceSource>()
{
  InputParameters params = validParams<Kernel>();
  params.addRequiredCoupledVar("psi_0", "The initial value of the variable for signed distance value.");
  params.addRequiredParam<Real>("epsilon",  "epsilon value for smoothed distance function.");
  params.addParam<Real>("scale", 1.0, "The scale value for the velocity vector.");
  return params;
}

LevelSetSignedDistanceSource::LevelSetSignedDistanceSource(const InputParameters & parameters)
  : Kernel(parameters),
    _psi_0(coupledValue("psi_0")),
    _epsilon(getParam<Real>("epsilon")),
    _scale(getParam<Real>("scale"))
{
}

Real
LevelSetSignedDistanceSource::computeQpResidual()
{
  Real psi_0 = _psi_0[_qp];
  Real S_0_psi_0 = 2.0 * smoothStepFunction(psi_0) - 1.0;
  return -_scale * S_0_psi_0 * _test[_i][_qp];
}

Real
LevelSetSignedDistanceSource::smoothStepFunction(Real psi) const
{
  if (psi < -_epsilon)
    return 0.0;
  else if (-_epsilon <= psi && psi <= _epsilon)
    return 1.0 / 2.0 * (1.0 + psi / _epsilon + 1.0 / M_PI * std::sin(M_PI * psi / _epsilon));
  else
    return 1.0;
}
