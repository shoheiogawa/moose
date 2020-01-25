//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SideGridSampler.h"

registerMooseObject("MooseApp", SideGridSampler);

defineLegacyParams(SideGridSampler);

InputParameters
SideGridSampler::validParams()
{
  InputParameters params = SideVectorPostprocessor::validParams();
  params.addClassDescription(
      "This vector post processor computes the average (or integral) of "
      "variables and material properties on surfaces in each rectangular "
      "cell on a grid.");
  params += GridSamplerInterface<SideVectorPostprocessor>::validParams();
  return params;
}

SideGridSampler::SideGridSampler(const InputParameters & parameters)
  : GridSamplerInterface<SideVectorPostprocessor>(parameters)
{
}
