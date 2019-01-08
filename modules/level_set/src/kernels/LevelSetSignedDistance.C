//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LevelSetSignedDistance.h"
#include <cmath>

registerMooseObject("LevelSetApp", LevelSetSignedDistance);

template <>
InputParameters
validParams<LevelSetSignedDistance>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Computes the source term for distance function computation.");
  params.addRequiredCoupledVar("initial_value", "inital value of distance function");
  params.addParam<Real>("epsilon", "epsilon value");
  return params;
}

LevelSetSignedDistance::LevelSetSignedDistance(const InputParameters & parameters)
  : Kernel(parameters),
    _u_0(coupledValue("initial_value")),
    _epsilon(getParam<Real>("epsilon"))
{
}

Real
LevelSetSignedDistance::computeQpResidual()
{
  Real u = _u[_qp];
  Real u_0 = _u_0[_qp];
  Real s = u_0 / (sqrt(u_0 * u_0 + _epsilon));
  Real grad_u_norm = _grad_u[_qp].norm();

  return s * (grad_u_norm - 1.0) * _test[_i][_qp];
}

