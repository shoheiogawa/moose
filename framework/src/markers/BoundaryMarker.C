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
      "depth", "Number of elements refined from the boundaries.");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "boundaries", "Names of the boundary with which the marker marks neighbor elements.");

  MooseEnum marker_states = Marker::markerStates();
  params.addRequiredParam<MooseEnum>(
      "mark", marker_states, "How to mark elements near the boundaries.");

  params.addClassDescription(
      "Marks elements within 'depth' elements from selected boundaries in selected blocks.");
  return params;
}

BoundaryMarker::BoundaryMarker(const InputParameters & parameters)
  : Marker(parameters),
    _marker_value((MarkerValue)(int)getParam<MooseEnum>("mark")),
    _boundary_info(_mesh.getMesh().get_boundary_info()),
    _depth(getParam<unsigned int>("depth"))
{
  // If this marker just marks elements next to selected boundaries, i.e. depth = 0,
  // it should work with a distributed mesh.
  if (_depth > 1)
    _mesh.errorIfDistributedMesh("BoundaryMarker");

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

bool
BoundaryMarker::searchForBoundaries(const Elem * elem, unsigned int depth)
{
  // // loop over sides of the current elem
  // for (unsigned int side = 0; side < elem->n_sides(); side++)

  // loop over nodes of the current elem
  for (unsigned int node = 0; node < elem->n_nodes(); node++)
  {
    // loop over all the boundary ids that you are looking for
    for (auto boundary_id : _mark_boundary_ids)
    {
      // // check if the current side has same boundary id that you are looking for
      // if (_boundary_info.has_boundary_id(elem, side, boundary_id))

      // check if the current node is on the boundary that you are looking for
      if (_boundary_info.has_boundary_id(elem->get_node(node), boundary_id))
      {
        return true;
      }
    }
  }

  // The `depth` is equal to the nubmer of elements to refine.
  // If it is 1, you don't go to neighbors' elements for the boundary search.
  if (depth > 1)
  {
    for (unsigned int side = 0; side < elem->n_sides(); side++)
    {
      const Elem * neighbor_elem = elem->neighbor_ptr(side);
      if (neighbor_elem != NULL)
        if (elem->subdomain_id() == neighbor_elem->subdomain_id())
        {
          // Checking the neighbor elements recursively.
          // If the function below eventually returns a true value in a recursive calling,
          // the true value is returned to the if statement in the computeElementMarker() member function.
          // depth - 1 because this jump to the neighbor.
          if (searchForBoundaries(neighbor_elem, depth - 1))
            return true;
        }
    }
  }
  return false;
}

Marker::MarkerValue
BoundaryMarker::computeElementMarker()
{
  if (searchForBoundaries(_current_elem, _depth))
    return _marker_value;
  else
    return DO_NOTHING;
}

