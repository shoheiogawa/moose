//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef PARAMETRIC_STUDY_INTERFACE_H
#define PARAMETRIC_STUDY_INTERFACE_H

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
   * validParams funcion which has several paramter related functions.
   */
  static InputParameters validParams();

protected:
  /**
   * Used in the timestepSetup function of the class which is inheriting from this class.
   */
  virtual void showParamInfoToConsole();

  /**
   * Computes current parameter values.
   */
  virtual void setCurrentParamValue();

  /**
   * Returns current parameter values.
   */
  virtual Real getCurrentParamValue();

protected:
  using T::_t;
  using T::_app;
  using T::_console;

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

template <typename T>
ParametricStudyInterface<T>:: ParametricStudyInterface(const InputParameters & parameters)
: T(parameters),
  _value_list(this->template getParam<std::vector<Real>>("value_list")),
  _range_func_args(this->template getParam<std::vector<Real>>("range_func")),
  _sort(this->template getParam<MooseEnum>("sort")),
  _unique_list(this->template getParam<bool>("unique_list")),
  _unique_tolerance(this->template getParam<Real>("unique_tolerance")),
  _epsilon_range(this->template getParam<Real>("epsilon_range")),
  _epsilon_index(this->template getParam<Real>("epsilon_index"))
{
  SubProblem & subproblem = *this->template getCheckedPointerParam<SubProblem *>("_subproblem");
  // The simulation has to be transient for indexing.
  if (!(subproblem.isTransient()))
    mooseError("Parametric study capability works only in a transient simulation.");

  // If we use value_range, check if there are exactly three values given to the parameter.
  // Add the range function results to _value_list, which may have some values already.
  if (this->isParamValid("range_func"))
  {
    if (_range_func_args.size() != 3)
      mooseError("The number of values provided to `range_func` parameter is not 3.");

    Real start_value = _range_func_args[0];
    Real end_value   = _range_func_args[1];
    Real increment   = _range_func_args[2];

    Real num_of_params_to_generate =
      (end_value - start_value - _epsilon_range) / increment;
    if (num_of_params_to_generate < 1.0)
      mooseError("No sweep parameter is generated using `range_func` parameter.");

    for (unsigned int i = 0; i <= (unsigned int)num_of_params_to_generate; ++i)
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
      mooseError("sort parameter is wrong (parametric study capability).");
  }

  if (_unique_list)
  {
    // Remove consecutive duplicates in range. If we study for
    // the same parameter, the simulation fails because the solution
    // doesn't change at all for the second instance of the same parameter.
    std::vector<Real>::iterator it;
    it = std::unique(_value_list.begin(), _value_list.end(),
        [this](Real a, Real b) {return std::fabs(a - b) < _unique_tolerance;});
    _value_list.resize(std::distance(_value_list.begin(), it));
  }

  // The parametric study capability has to work in a transient simulation for parameter indexing
  Transient * transient_executioner = dynamic_cast<Transient *>(_app.getExecutioner());
  if (transient_executioner == NULL)
    mooseError("ParametricStudyInterface works only in a transient simulation."
               " The pointer to the executioner is not a transient one.");
  else
  {
    // Setting the end time such that all the parameters are studied.
    transient_executioner->endTime() = static_cast<Real>(_value_list.size());
  }
}

template <typename T>
void
ParametricStudyInterface<T>::showParamInfoToConsole()
{
  _console << _current_param_value << " at time = " << _t <<" ";
  if (_t + _epsilon_index < ceil(_t - _epsilon_index))
    _console << "(This time step is intermediate for mesh refinement.)";
  _console << "\n";
}

template <typename T>
void
ParametricStudyInterface<T>::setCurrentParamValue()
{
  unsigned int index = static_cast<unsigned int>(_t - _epsilon_index);
  _current_param_value = _value_list[index];
}

template <typename T>
Real
ParametricStudyInterface<T>::getCurrentParamValue()
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
      "Range function which provides values to study. "
      "Three Real values (start end increment) should be given."
      "Ex) With arguments: 0.0 0.3 0.1, 0.0, 0.1, 0.2 will be the parameters.");

  MooseEnum order("ascending descending");
  params.addParam<MooseEnum>("sort", order,
      "Sorting order for the params. If it is not specified, "
      "the parameters are the list (in the order as written) "
      "followed by the range function results.");

  params.addParam<bool>("unique_list", true,
      "Set false to study same parameters twice or more in one simulation.");

  params.addParam<Real>("unique_tolerance", 1e-12,
      "Tolerance to find two values in the parameter list the same.");

  params.addParam<Real>("epsilon_range", 1e-12,
      "Small value used in `(start - end - epsilon_range) / increment` "
      "to compute the number of parameters studied.");

  params.addParam<Real>("epsilon_index", 1e-12,
      "Small value used in time to index conversion such that, for example, "
      "at t = 1, the first parameter is studied in stead of the second one.");

  // Parameters have to be set at the beginning of
  // each time step because it uses the time value for indexing.
  //params.set<ExecFlagEnum>("execute_on") = "TIMESTEP_BEGIN";

  return params;
}

#endif // PARAMETRIC_STUDY_INTERFACE_H
