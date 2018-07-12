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
  params.addParam<std::vector<Real>>("value_list", "List of values which you want to set to the aux variable.");
  params.addParam<std::vector<Real>>("range_func", "Range function which determines "
                                     "the values which you want to set to the aux variable."
                                     "Three Real values should be given: start end increment");
  params.addParam<Real>("tolerance", "List of values which you want to set to the aux variable.");
  return params;
}

ParametricStudyAux::ParametricStudyAux(const InputParameters & parameters)
: AuxKernel(parameters),
  _value_list(getParam<std::vector<Real>>("value_list")),
  _range_func_args(getParam<std::vector<Real>>("range_func")),
  _tolerance(getParam<Real>("tolerance"))
{
  // TODO
  // 1) Check if the simulatio is transient.
  // 2) Set `exectute_on` TIMESTEP_BEGIN
  // 3) Check `end_time` is large enough to study all the parameters.

  //if (!(_problem->isTransient()))
  //  mooseError("ParametricStudyAux works only in a transient simulation.");

  if (isParamValid("range_func") && isParamValid("value_list"))
    mooseError("value_list and value_range are given. Only one is accepted.");

  // If we use value_range, check if there are exactly three values given to the parameter.
  // Store the range function results to _value_list, so that the following process is the same.
  if (isParamValid("range_func"))
  {
    if (_range_func_args.size() != 3)
      mooseError("The number of values provided to `range_func` parameter is not 3.");

    Real start_value = _range_func_args[0];
    Real end_value   = _range_func_args[1];
    Real increment   = _range_func_args[2];

    if (start_value > end_value)
      mooseError("The start value is larger than the end value.");

    Real value = start_value;
    while (value < end_value)
    {
      _value_list.push_back(value);
      value += increment;
    }
  }

  //if ((_app.getExecutioner()->endTime() + _tolerance) <static_cast<Real>(_value_list.size()))
  //  mooseWarning("end_time is too small to study all the parameters.");
}

Real
ParametricStudyAux::computeValue()
{
  unsigned int index = static_cast<unsigned int>(_t - _tolerance);
  return _value_list[index];
}
