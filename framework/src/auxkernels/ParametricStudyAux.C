//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ParametricStudyAux.h"
#include "TimeStepper.h"
#include "Transient.h"

#include <algorithm>
#include <cmath>

registerMooseObject("MooseApp", ParametricStudyAux);

template <>
InputParameters
validParams<ParametricStudyAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription("Aux kernel that sets values to the aux variable. "
                             "The parameters are given either as a list or a range function, "
                             "which is similar to `range` function in Python. If the time: t is between `tolerance` to 1 + `tolerance`, "
                             "this aux kernel sets the first value. This is repeated until all the parameters are studied. ");

  // Either value_list or range_func should be provided but not both.
  params.addParam<std::vector<Real>>("value_list", "List of values which you want to set to the aux variable.");
  params.addParam<std::vector<Real>>("range_func", "Range function which determines "
                                     "the values which you want to set to the aux variable."
                                     "Three Real values should be given: start end increment");

  MooseEnum order("ascending descending");
  params.addParam<MooseEnum>("sort", order, "Sorting order for the params. If it is not specified, "
                                            "the parameters are the list + the range function results.");

  params.addParam<bool>("unique", true, "Set true to make all the parameters unique.");
  params.addParam<Real>("unique_tolerance", 1e-12, "Tolerance to find two values in the list the same.");

  // Parameters have to be set at the beginning of each time step because it uses the time value for indexing.
  params.set<ExecFlagEnum>("execute_on") = "TIMESTEP_BEGIN";

  return params;
}

ParametricStudyAux::ParametricStudyAux(const InputParameters & parameters)
: AuxKernel(parameters),
  _value_list(getParam<std::vector<Real>>("value_list")),
  _range_func_args(getParam<std::vector<Real>>("range_func")),
  _sort(getParam<MooseEnum>("sort")),
  _unique_list(getParam<bool>("unique")),
  _unique_tolerance(getParam<Real>("unique_tolerance"))
{
  // The simulation has to be transient for indexing.
  if (!(_subproblem.isTransient()))
    mooseError("ParametricStudyAux works only in a transient simulation.");

  //if (isParamValid("range_func") && isParamValid("value_list"))
  //  mooseWarning("value_list and value_range are given. Only one is accepted.");

  // If we use value_range, check if there are exactly three values given to the parameter.
  // Add the range function results to _value_list, which may have some values already.
  if (isParamValid("range_func"))
  {
    if (_range_func_args.size() != 3)
      mooseError("The number of values provided to `range_func` parameter is not 3.");

    Real start_value = _range_func_args[0];
    Real end_value   = _range_func_args[1];
    Real increment   = _range_func_args[2];

    Real num_of_params_to_generate = (end_value - start_value) / increment;
    if (num_of_params_to_generate < 1.0)
      mooseError("No sweep parameter is generated using `range_func` parameter.");

    for (unsigned int i = 0; i <= (unsigned int)num_of_params_to_generate; ++i)
      _value_list.push_back(start_value + i * increment);
  }

  if (isParamValid("sort"))
  {
    if (_sort == "ascending")
      std::sort(_value_list.begin(), _value_list.end(), [](Real a, Real b) {return a < b;});
    else if (_sort == "descending")
      std::sort(_value_list.begin(), _value_list.end(), [](Real a, Real b) {return a > b;});
    else
      mooseError("sort parameter is wrong in ParametricStudyAux.");
  }

  if (_unique_list)
  {
    // Remove consecutive duplicates in range because if we study for the same parameter,
    // the simulation fails because the solution doesn't change at all for the second instance of the same parameter.
    std::vector<Real>::iterator it;
    it = std::unique(_value_list.begin(), _value_list.end(), [this](Real a, Real b) {return std::fabs(a - b) < _unique_tolerance;});
    _value_list.resize(std::distance(_value_list.begin(), it));
  }

  // Check if end_time given to the executioner is large enough to study all the parameters.
  Transient * transient_executioner = dynamic_cast<Transient *>(_app.getExecutioner());
  if (transient_executioner == NULL)
    mooseError("ParametricStudyAux works only in a transient simulation. The pointer to the executioner is not a transient one.");
  else
  {
    transient_executioner->endTime() = static_cast<Real>(_value_list.size());
  }

  _console << "Parameters for " + _var.name() << " : ";
  for (auto value : _value_list)
    _console << value << " ";
  _console << "\n";
}
void
ParametricStudyAux::timestepSetup()
{
  unsigned int index = static_cast<unsigned int>(_t - 1e-15);
  _console << "Parametric study : " + _var.name() << " = " << _value_list[index] << " at time = " << _t << "\n";
  if (_t + 1e-15 < ceil(_t - 1e-15))
    _console << "(This time step is intermediate for mesh refinement.)\n";
}

Real
ParametricStudyAux::computeValue()
{
  unsigned int index = static_cast<unsigned int>(_t - 1e-15);
  return _value_list[index];
}
