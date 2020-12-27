//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MooseTypes.h"
#include "ExecFlagEnum.h"
#include "MooseEnum.h"
#include "InputParameters.h"
#include "SubProblem.h"
#include "Transient.h"

//// Forward Declarations
template <typename T>
class ParametricStudyInterface;

template <typename T>
class ParametricStudyInterface
  : public T
{
public:
  ParametricStudyInterface(const InputParameters & parameters);

  /**
   * validParams function which computes values for a parametric study
   */
  static InputParameters validParams();

protected:
  /**
   * Used in the timestepSetup function of the class which is inheriting from this class.
   */
  virtual void showParamInfoToConsole() const;

  /**
   * Computes current parameter values.
   */
  virtual void setCurrentParamValue();

  /**
   * Returns current parameter values.
   */
  virtual Real getCurrentParamValue() const;

protected:
  using T::_t;
  using T::_app;
  using T::_console;

  /// Current parameter value
  Real _current_param_value;

  /// List of parameters to be studied
  std::vector<Real> _value_list;

  /// Vector to store start_value, end_value, increment
  std::vector<Real> _range_func_args;

  /// Store ascending or descending
  MooseEnum _sort;

  /// Set true if the _value_list should be a unique set
  bool _unique_list;

  /// Tolerance to find two values the same in the list
  Real _unique_list_tolerance;

  /// Small value used in the range function calculation
  Real _range_func_toleracne;

  /// Small value used to find a next parameter should be studied
  Real _indexing_tolerance;
};

template <typename T>
ParametricStudyInterface<T>:: ParametricStudyInterface(const InputParameters & parameters)
: T(parameters),
  _value_list(getParam<std::vector<Real>>("value_list")),
  _range_func_args(getParam<std::vector<Real>>("range_func")),
  _sort(getParam<MooseEnum>("sort")),
  _unique_list(getParam<bool>("unique_list")),
  _unique_list_tolerance(getParam<Real>("unique_list_tolerance")),
  _range_func_toleracne(getParam<Real>("range_func_tolerance")),
  _indexing_tolerance(getParam<Real>("indexing_tolerance"))
{
  SubProblem & subproblem = *this->template getCheckedPointerParam<SubProblem *>("_subproblem");
  // The simulation has to be transient for indexing.
  if (!(subproblem.isTransient()))
    mooseError("Parametric study system works only in a transient simulation.");

  // If we use value_range, check if there are exactly three values given to the parameter.
  // Add the range function results to _value_list, which may have some values already.
  if (this->isParamValid("range_func"))
  {
    if (_range_func_args.size() != 3)
      mooseError("The number of values provided to the `range_func` parameter is not three.");

    Real start_value = _range_func_args[0];
    Real end_value   = _range_func_args[1];
    Real increment   = _range_func_args[2];

    Real num_params_to_generate =
      (end_value - start_value - _range_func_toleracne) / increment;
    if (num_params_to_generate < 1.0)
      mooseError("No sweep parameters are generated using the `range_func` parameter.");

    for (unsigned int i = 0; i <= (unsigned int)num_params_to_generate; ++i)
      _value_list.push_back(start_value + i * increment);
  }

  if (this->isParamValid("sort"))
  {
    if (_sort == "ascending")
      std::sort(_value_list.begin(), _value_list.end(),
                [](Real a, Real b) {return a < b;});
    else if (_sort == "descending")
      std::sort(_value_list.begin(), _value_list.end(),
                [](Real a, Real b) {return a > b;});
    else
      mooseError("sort parameter is wrong in parametric study object.");
  }

  if (_unique_list)
  {
    // Remove consecutive duplicates in the value list. If we study for
    // the same parameter, the simulation could fail because the solution
    // doesn't change at all for the second instance of the same parameter.
    std::vector<Real>::iterator it;
    it = std::unique(_value_list.begin(), _value_list.end(),
        [this](Real a, Real b) {return std::fabs(a - b) < _unique_list_tolerance;});
    _value_list.resize(std::distance(_value_list.begin(), it));
  }

  // The parametric study system has to work in a transient simulation for parameter indexing
  Transient * transient_executioner = dynamic_cast<Transient *>(_app.getExecutioner());
  if (transient_executioner == NULL)
    mooseError("Parametric study system works only in a transient simulation."
               " The pointer to the executioner is not a transient one.");
  else
  {
    // Setting the end time such that all the parameters are studied.
    transient_executioner->endTime() = static_cast<Real>(_value_list.size());
  }
}

template <typename T>
void
ParametricStudyInterface<T>::showParamInfoToConsole() const
{
  _console << _current_param_value << " at time = " << _t <<" ";
  if (_t + _indexing_tolerance < ceil(_t - _indexing_tolerance))
    _console << "(This time step is intermediate for mesh refinement.)";
  _console << "\n";
}

template <typename T>
void
ParametricStudyInterface<T>::setCurrentParamValue()
{
  unsigned int index = static_cast<unsigned int>(_t - _indexing_tolerance);
  _current_param_value = _value_list[index];
}

template <typename T>
Real
ParametricStudyInterface<T>::getCurrentParamValue() const
{
  return _current_param_value;
}

template <typename T>
InputParameters
ParametricStudyInterface<T>::validParams()
{
  InputParameters params = emptyInputParameters();
  params.addClassDescription("Parametric study interface.");

  params.addParam<std::vector<Real>>("value_list", "List of values to be studied.");
  params.addParam<std::vector<Real>>("range_func",
      "Range function that provides values to study. "
      "Three Real values (start end increment) should be given."
      "Ex) With arguments: 0.0 0.3 0.1, 0.0, 0.1, 0.2 will be the parameters.");

  MooseEnum order("ascending descending");
  params.addParam<MooseEnum>("sort", order,
      "Specify ascending or descending for the sorting order of parameter values."
      "If it is not specified, the parameters are the list (in the order as written) "
      "followed by the range function values.");

  params.addParam<bool>("unique_list", false,
      "Set true to study the same parameters only once in one simulation. "
      "The `sort` parameter must be ascending or descending for this to work.");

  params.addParam<Real>("unique_list_tolerance", 1e-12,
      "Tolerance to find two values in the parameter list the same.");

  params.addParam<Real>("range_func_tolerance", 1e-12,
      "Small value used in `(start - end - range_func_tolerance) / increment` "
      "to compute the number of parameters studied.");

  params.addParam<Real>("indexing_tolerance", 1e-12,
      "Small value used in the time to index conversion. For example, "
      "at t = 1 + 1e-13, the first parameter is studied in stead of the second one "
      "when the default value is used.");

  // Parameters have to be set at the beginning of
  // each time step because it uses the time value for indexing.
  //params.set<ExecFlagEnum>("execute_on") = "TIMESTEP_BEGIN";

  return params;
}
