//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ParametricStudyAux.h"

registerMooseObject("MooseApp", ParametricStudyAux);

template <>
InputParameters
validParams<ParametricStudyAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription(
      "Aux kernel that sets values to an aux variable for a parametric study.");
  params += ParametricStudyInterface<AuxKernel>::validParams();
  params.set<ExecFlagEnum>("execute_on") = "TIMESTEP_BEGIN";
  return params;
}

ParametricStudyAux::ParametricStudyAux(const InputParameters & parameters)
  : ParametricStudyInterface<AuxKernel>(parameters)
{
}

Real
ParametricStudyAux::computeValue()
{
  return getCurrentParamValue();
}
 
void
ParametricStudyAux::timestepSetup()
{
  setCurrentParamValue();
  _console << "Parametric study : " + _var.name() << " = ";
  showParamInfoToConsole();
}
