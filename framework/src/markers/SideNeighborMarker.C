//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SideNeighborMarker.h"

registerMooseObject("MooseApp", SideNeighborMarker);

template <>
InputParameters
validParams<SideNeighborMarker>()
{
  InputParameters params = validParams<Marker>();
  params.addRequiredParam<unsigned int>(
      "depth", "The number of elements refined from the sidesets");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "sidesets", "The names of the sidesets with which the marker marks neighbor elements");
  MooseEnum marker_states = Marker::markerStates();
  params.addRequiredParam<MooseEnum>(
      "mark", marker_states, "How to mark elements near the sidesets");
  params.addRelationshipManager(
    "ElementSideNeighborLayers",
    Moose::RelationshipManagerType::GEOMETRIC,
    [](const InputParameters & params, InputParameters & rm_params) {
      rm_params.set<unsigned short>("layers") = params.get<unsigned int>("depth");
    }
  );
  params.addClassDescription(
      "Marks elements within 'depth' elements from selected sidesets in selected blocks.");
  return params;
}

SideNeighborMarker::SideNeighborMarker(const InputParameters & parameters)
  : Marker(parameters),
    _marker_value((MarkerValue)(int)getParam<MooseEnum>("mark")),
    _boundary_info(_mesh.getMesh().get_boundary_info()),
    _depth(getParam<unsigned int>("depth"))
{
  if (isParamValid("sidesets"))
  {
    auto & sideset_names = getParam<std::vector<BoundaryName>>("sidesets");
    for (auto & sideset_name : sideset_names)
      _mark_sideset_ids.insert(_mesh.getBoundaryID(sideset_name));
  }
  else
  {
    mooseError("The input to 'sidesets' is not valid.");
  }
}

bool
SideNeighborMarker::searchForSidesets(const Elem * elem, bool previous_elem,
                              unsigned int previous_elem_id, unsigned int depth)
{
  // Check if the sides of this element has the boundary IDs we are looking for
  for (unsigned int side = 0; side < elem->n_sides(); side++)
    for (auto boundary_id : _mark_sideset_ids)
      if (_boundary_info.has_boundary_id(elem, side, boundary_id))
        return true;

  // `depth` is equal to the nubmer of elements from the sidesets to be refined.
  // If it is 1, you don't go to any neighbor elements for the sideset search.
  if (depth > 1)
  {
    for (unsigned int side = 0; side < elem->n_sides(); side++)
    {
      const Elem * neighbor_elem = elem->neighbor_ptr(side);
      // Avoid going back in recursive search
      if (previous_elem && previous_elem_id == neighbor_elem->id())
        continue;
      if (neighbor_elem != NULL)
      {
        if (elem->subdomain_id() == neighbor_elem->subdomain_id())
        {
          // Check the neighbor elements recursively. If the function below
          // eventually returns a true value in a recursive calling, true value
          // is returned to the if statement in computeElementMarker().
          // depth - 1 because you jump to one of the neighbors.
          if (searchForSidesets(neighbor_elem, true, elem->id(), depth - 1))
            return true;
        }
      }
    }
  }
  return false;
}

Marker::MarkerValue
SideNeighborMarker::computeElementMarker()
{
  if (searchForSidesets(_current_elem, false, 0, _depth))
    return _marker_value;
  else
    return DO_NOTHING;
}

