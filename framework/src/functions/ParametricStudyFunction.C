//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ParametricStudyFunction.h"

registerMooseObject("MooseApp", ParametricStudyFunction);

defineLegacyParams(ParametricStudyFunction);

InputParameters
ParametricStudyFunction::validParams()
{
  InputParameters params = Function::validParams();
  params.addClassDescription(
      "Function that computes values for parametric study");
  params += ParametricStudyInterface<Function>::validParams();
  return params;
}

ParametricStudyFunction::ParametricStudyFunction(const InputParameters & parameters)
  : ParametricStudyInterface<Function>(parameters)
{
}

Real
ParametricStudyFunction::value(Real, const Point &) const
{
  return getCurrentParamValue();
}

void
ParametricStudyFunction::timestepSetup()
{
  setCurrentParamValue();
  _console << "Parametric study for function: " + name() << " = ";
  showParamInfoToConsole();
}
