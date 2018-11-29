//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "LevelSetSignedDistancVelocity.h"

registerMooseObject("LevelSetApp", LevelSetSignedDistancVelocity);

template <>
InputParameters
validParams<LevelSetSignedDistancVelocity>()
{
  InputParameters params = validParams<AuxKernel>();
  MooseEnum comp("x=0 y=1 z=2");
  params.addRequiredParam<MooseEnum>("component", comp, "The component of velocity to return.");
  params.addRequiredCoupledVar("psi", "The variable for signed distance value.");
  params.addParam<Real>("scale", 1.0, "The scale value for the velocity vector.");
  return params;
}

LevelSetSignedDistancVelocity::LevelSetSignedDistancVelocity(const InputParameters & parameters)
  : AuxKernel(parameters),
    _psi(coupledValue("psi")),
    _grad_psi(coupledGradient("psi")),
    _component(getParam<MooseEnum>("component")),
    _scale(getParam<Real>("scale"))
{
}

Real
LevelSetSignedDistancVelocity::computeValue()
{
  Real psi = _psi[_qp];
  Real sign = (psi > 0) - (psi < 0);
  Real grad_psi_norm = _grad_psi[_qp].norm();
  if (grad_psi_norm < 1e-6)
    return 0.0;
  else
  {
    if (psi < -0.5 || 0.5 < psi)
      return 0.0;
    else
      return _scale * sign * _grad_psi[_qp](_component) / grad_psi_norm;
  }
}
