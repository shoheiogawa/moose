//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "NeumannWithWritableAuxBC.h"

registerMooseObject("MooseApp", NeumannWithWritableAuxBC);

template <>
InputParameters
validParams<NeumannWithWritableAuxBC>()
{
  InputParameters params = validParams<IntegratedBC>();
  params.addParam<Real>("value", 0.0, "The value of the gradient on the boundary.");
  params.declareControllable("value");
  params.addRequiredCoupledVar("aux_var", "An aux variable to store BC value.");
  params.addClassDescription("Imposes the integrated boundary condition "
                             "$\\frac{\\partial u}{\\partial n}=h$, "
                             "where $h$ is a constant, controllable value.");
  return params;
}

NeumannWithWritableAuxBC::NeumannWithWritableAuxBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
  _aux_var(writableCoupledValue("aux_var")),
  _value(getParam<Real>("value"))
{
}

Real
NeumannWithWritableAuxBC::computeQpResidual()
{
  _aux_var[_qp] = _value;
  return -_test[_i][_qp] * _value;
}
