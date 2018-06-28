//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "BoundaryMarker.h"

registerMooseObject("MooseApp", BoundaryMarker);

template <>
InputParameters
validParams<BoundaryMarker>()
{
  InputParameters params = validParams<Marker>();
  params.addRequiredParam<unsigned int>(
      "distance", "Integer value within which number of elements to be refined.");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundaries", "The names of the boundary to mark neighbors.");

  MooseEnum marker_states = Marker::markerStates();
  params.addRequiredParam<MooseEnum>(
      "mark", marker_states, "How to mark elements near the boundaries.");

  params.addClassDescription(
      "Marks elements within 'distance' elements from the boundary.");
  return params;
}

BoundaryMarker::BoundaryMarker(const InputParameters & parameters)
  : Marker(parameters),
    _marker_value((MarkerValue)(int)parameters.get<MooseEnum>("mark"))
{
  if (isParamValid("boundaries"))
  {
    auto & boundary_names = getParam<std::vector<BoundaryName>>("boundaries");
    for (auto & boundary_name : boundary_names)
      _mark_boundary_ids.insert(_mesh.getBoundaryID(boundary_name));
  }
  else
  {
    mooseError("'boundaries' param input is not valid.");
  }
}

//void
//BoundaryMarker::markerSetup()
//{
//}

Marker::MarkerValue
BoundaryMarker::computeElementMarker()
{
  auto & boundary_info = _mesh.getMesh().get_boundary_info();

  // loop over sides of the current elem
  for (unsigned int side = 0; side < _current_elem->n_sides(); side++)
  {
    // loop over all the boundary ids that you are looking for
    for (auto boundary_id : _mark_boundary_ids)
    {
      // check if the current side has same boundary id that you are looking for
      if (boundary_info.has_boundary_id(_current_elem, side, boundary_id))
      {
        return _marker_value;
      }
    }
  }
  return DO_NOTHING;
}
