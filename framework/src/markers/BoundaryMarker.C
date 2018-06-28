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
      "range", "Integer value within which number of elements to be refined.");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundaries", "The names of the boundary to mark neighbors.");

  MooseEnum marker_states = Marker::markerStates();
  params.addRequiredParam<MooseEnum>(
      "mark", marker_states, "How to mark elements near the boundaries.");

  params.addClassDescription(
      "Marks elements within 'range' elements from the boundary.");
  return params;
}

BoundaryMarker::BoundaryMarker(const InputParameters & parameters)
  : Marker(parameters),
    _marker_value((MarkerValue)(int)getParam<MooseEnum>("mark")),
    _boundary_info(_mesh.getMesh().get_boundary_info()),
    _range(getParam<unsigned int>("range"))
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
//  // updating the boundary info everytime this marker works.
//  Marker::markerSetup(); 
//  _boundary_info = _mesh.getMesh().get_boundary_info();
//}

bool
BoundaryMarker::searchForBoundaries(const Elem * elem, unsigned int range)
{
  // // loop over sides of the current elem
  // for (unsigned int side = 0; side < elem->n_sides(); side++)

  // loop over nodes of the current elem
  for (unsigned int node = 0; node < _current_elem->n_nodes(); node++)
  {
    // loop over all the boundary ids that you are looking for
    for (auto boundary_id : _mark_boundary_ids)
    {
      // // check if the current side has same boundary id that you are looking for
      // if (_boundary_info.has_boundary_id(elem, side, boundary_id))
      
      // check if the current node is on the boundary that you are looking for
      if (_boundary_info.has_boundary_id(_current_elem->get_node(node), boundary_id))
      {
        return true;
      }
    }
  }

  // If range is equal to the nubmer of elements to refine.
  // If it is 1, you don't go to neighbors' elements for the boundary search.
  if (range > 1) 
  {
    for (unsigned int side = 0; side < elem->n_sides(); side++)
    {
      const Elem * neighbor_elem = elem->neighbor_ptr(side);
      if (neighbor_elem != NULL)
      {
        // Checking the neighbor elements.
        // range - 1 because this jump to the neighbor.
        if(searchForBoundaries(neighbor_elem, range - 1))
          return true;
      }
    }
  }
  return false;
}

Marker::MarkerValue
BoundaryMarker::computeElementMarker()
{
  if (searchForBoundaries(_current_elem, _range))
    return _marker_value;
  else
    return DO_NOTHING;
}
