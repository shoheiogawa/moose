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
      "depth", "Number of elements refined from the sidesets.");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "sidesets", "Names of the sidesets with which the marker marks neighbor elements.");

  MooseEnum marker_states = Marker::markerStates();
  params.addRequiredParam<MooseEnum>(
      "mark", marker_states, "How to mark elements near the sidesets.");

  params.addClassDescription(
      "Marks elements within 'depth' elements from selected sidesets in selected blocks.");
  return params;
}

SideNeighborMarker::SideNeighborMarker(const InputParameters & parameters)
  : Marker(parameters),
    _marker_value((MarkerValue)(int)getParam<MooseEnum>("mark")),
    _depth(getParam<unsigned int>("depth"))
{
  // If this marker just marks elements next to selected sidesets, i.e. depth = 0,
  // it should work with a distributed mesh.
  if (_depth > 1)
    _mesh.errorIfDistributedMesh("SideNeighborMarker");

  if (isParamValid("sidesets"))
  {
    auto & sideset_names = getParam<std::vector<BoundaryName>>("sidesets");
    for (auto & sideset_name : sideset_names)
      _mark_sideset_ids.insert(_mesh.getBoundaryID(sideset_name));
  }
  else
  {
    mooseError("'sidesets' param input is not valid.");
  }
}

bool
SideNeighborMarker::searchForSidesets(const Elem * elem, unsigned int depth)
{
  bool use_node = false;
  bool use_elem = true;

  if (use_node)
    // loop over nodes of the current elem
    for (unsigned int node = 0; node < elem->n_nodes(); node++)
      for (auto sideset_id : _mark_sideset_ids)
        // check if the current node is on the sideset that you are looking for
        if (_mesh.isBoundaryNode(elem->get_node(node)->id(), sideset_id))
          return true;

  if (use_elem)
    // loop over sides of the current elem
    for (auto sideset_id : _mark_sideset_ids)
      if (_mesh.isBoundaryElem(elem->id(), sideset_id))
        return true;

  // The `depth` is equal to the nubmer of elements to refine.
  // If it is 1, you don't go to neighbors' elements for the sideset search.
  if (depth > 1)
  {
    for (unsigned int side = 0; side < elem->n_sides(); side++)
    {
      const Elem * neighbor_elem = elem->neighbor_ptr(side);
      if (neighbor_elem != NULL)
        if (elem->subdomain_id() == neighbor_elem->subdomain_id())
        {
          // Check the neighbor elements recursively.
          // If the function below eventually returns a true value in a recursive calling,
          // the true value is returned to the if statement in the computeElementMarker() member function.
          // depth - 1 is supplied because you jump to one of the neighbor.
          if (searchForSidesets(neighbor_elem, depth - 1))
            return true;
        }
    }
  }
  return false;
}

Marker::MarkerValue
SideNeighborMarker::computeElementMarker()
{
  if (searchForSidesets(_current_elem, _depth))
    return _marker_value;
  else
    return DO_NOTHING;
}

