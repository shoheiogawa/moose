//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef PARAMETRIC_STUDY_AUX_H
#define PARAMETRIC_STUDY_AUX_H

#include "AuxKernel.h"

// Forward Declarations
class ParametricStudyAux;

template <>
InputParameters validParams<ParametricStudyAux>();

class ParametricStudyAux
  : public AuxKernel
{
public:
  ParametricStudyAux(const InputParameters & parameters);

protected:
  virtual Real computeValue() override;

  std::vector<Real> _value_list;
  std::vector<Real> _range_func_args;
  Real _tolerance;
};

#endif // PARAMETRIC_STUDY_AUX_H
