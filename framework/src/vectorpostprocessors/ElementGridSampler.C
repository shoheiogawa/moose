//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ElementGridSampler.h"

registerMooseObject("MooseApp", ElementGridSampler);

defineLegacyParams(ElementGridSampler);

InputParameters
ElementGridSampler::validParams()
{
  InputParameters params = ElementVectorPostprocessor::validParams();
  params.addClassDescription(
      "This vector post processor computes the average (or integral) of "
      "variables and material properties in each rectangular "
      "cell on a grid.");
  params += GridSamplerInterface<ElementVectorPostprocessor>::validParams();
  return params;
}

ElementGridSampler::ElementGridSampler(const InputParameters & parameters)
  : GridSamplerInterface<ElementVectorPostprocessor>(parameters)
{
}
