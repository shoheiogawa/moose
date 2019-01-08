//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ConstKernelWithWritableAux.h"

registerMooseObject("MooseApp", ConstKernelWithWritableAux);

template <>
InputParameters
validParams<ConstKernelWithWritableAux>()
{
  InputParameters params = validParams<Kernel>();
  params.addParam<Real>("value", 0.0, "The value of the gradient on the boundary.");
  params.declareControllable("value");
  params.addRequiredCoupledVar("aux_var", "An aux variable to store BC value.");
  params.addClassDescription("Imposes the contribution from a constant, controllable value.");
  return params;
}

ConstKernelWithWritableAux::ConstKernelWithWritableAux(const InputParameters & parameters)
  : Kernel(parameters),
  _aux_var(writableCoupledValue("aux_var")),
  _value(getParam<Real>("value"))
{
}

Real
ConstKernelWithWritableAux::computeQpResidual()
{
  _aux_var[_qp] = _value;
  return -_test[_i][_qp] * _value;
}
