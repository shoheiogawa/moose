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
  virtual void timestepSetup() override;
  virtual Real computeValue() override;

  /**
   * Used in the timestepSetup function of the class which is inheriting from this class.
   */
  virtual void showParamInfoToConsole();

  /**
   * Computes and sets _current_param_value. Should be called in timestepSetup().
   */
  void setCurrentParamValue();

private:
  /// Current parameter value.
  Real _current_param_value;

  /// List of parameters to be studied.
  std::vector<Real> _value_list;

  /// Vector to store start_value, end_value, increment.
  std::vector<Real> _range_func_args;

  /// Store ascending or descending.
  MooseEnum _sort;

  /// Has true if the _value_list should be a unique set.
  bool _unique_list;

  /// Tolerance to find two values the same in the list.
  Real _unique_tolerance;

  /// Small value used in range function calculation.
  Real _epsilon_range;

  /// Small value used to find a next parameter should be studied.
  Real _epsilon_index;
};
#endif // PARAMETRIC_STUDY_AUX_H
