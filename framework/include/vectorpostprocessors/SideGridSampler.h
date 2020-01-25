//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "SideVectorPostprocessor.h"
#include "GridSamplerInterface.h"

class SideGridSampler;

/**
 *  This vector post processor computes the average (or integral) of
 *  variables and material properties on surfaces in each rectangular
 *  cell on a grid.
 */
class SideGridSampler
  : public GridSamplerInterface<SideVectorPostprocessor>
{
public:
  static InputParameters validParams();

  SideGridSampler(const InputParameters & parameters);
};
