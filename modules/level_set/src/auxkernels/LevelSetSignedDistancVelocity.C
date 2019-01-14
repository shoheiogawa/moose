//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LevelSetSignedDistancVelocity.h"

#include <cmath>

registerMooseObject("LevelSetApp", LevelSetSignedDistancVelocity);

template <>
InputParameters
validParams<LevelSetSignedDistancVelocity>()
{
  InputParameters params = validParams<AuxKernel>();
  MooseEnum comp("x=0 y=1 z=2");
  params.addRequiredParam<MooseEnum>("component", comp, "The component of velocity to return.");
  params.addRequiredCoupledVar("psi", "The variable for signed distance value.");
  params.addRequiredCoupledVar("psi_0", "The initial value of the variable for signed distance value.");
  params.addRequiredParam<Real>("epsilon",  "epsilon value for smoothed distance function.");
  params.addParam<Real>("scale", 1.0, "The scale value for the velocity vector.");
  return params;
}

LevelSetSignedDistancVelocity::LevelSetSignedDistancVelocity(const InputParameters & parameters)
  : AuxKernel(parameters),
    _psi(coupledValue("psi")),
    _grad_psi(coupledGradient("psi")),
    _psi_0(coupledValue("psi_0")),
    _component(getParam<MooseEnum>("component")),
    _epsilon(getParam<Real>("epsilon")),
    _scale(getParam<Real>("scale"))
{
}

Real
LevelSetSignedDistancVelocity::computeValue()
{
  Real psi_0 = _psi_0[_qp];
  Real grad_psi_norm = _grad_psi[_qp].norm();
  Real S_0_psi_0 = 2.0 * smoothStepFunction(psi_0) - 1.0;

  if (grad_psi_norm < 1e-6)
    return 0.0;
  else
  {
    return _scale * S_0_psi_0 * _grad_psi[_qp](_component) / grad_psi_norm;
  }
}

Real
LevelSetSignedDistancVelocity::smoothStepFunction(Real psi) const
{
  if (psi < -_epsilon)
    return 0.0;
  else if (-_epsilon <= psi && psi <= _epsilon)
    return 1.0 / 2.0 * (1.0 + psi / _epsilon + 1.0 / M_PI * std::sin(M_PI * psi / _epsilon));
  else
    return 1.0;
}
