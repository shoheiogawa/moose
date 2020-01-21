//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Function.h"
#include "ParametricStudyInterface.h"

class ParametricStudyFunction;

/**
 * Function that gives values for parametric study.
 */
class ParametricStudyFunction
  : public ParametricStudyInterface<Function>
{
public:
  static InputParameters validParams();

  ParametricStudyFunction(const InputParameters & parameters);

  virtual Real value(Real t, const Point & p) const override;

protected:
  virtual void timestepSetup() override;
};
