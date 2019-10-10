//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "AuxKernel.h"
#include "ParametricStudyInterface.h"

// Forward Declarations
class ParametricStudyAux;

template <>
InputParameters validParams<ParametricStudyAux>();

/**
 * Aux kernel that sets values to an aux variable for a parametric study.
 */
class ParametricStudyAux
  : public ParametricStudyInterface<AuxKernel>
{
public:
  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  ParametricStudyAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;
  virtual void timestepSetup() override;
};
