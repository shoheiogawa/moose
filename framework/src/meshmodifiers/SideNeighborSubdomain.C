//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SideNeighborSubdomain.h"
#include "MooseMesh.h"

#include <algorithm>

registerMooseObject("MooseApp", SideNeighborSubdomain);

template <>
InputParameters
validParams<SideNeighborSubdomain>()
{
  InputParameters params = validParams<MeshModifier>();
  params.addClassDescription("Changes the subdomain ID of elements "
                             "near the specified sidesets.");
  params.addRequiredParam<std::vector<BoundaryName>>(
      "sidesets", "Names of the sidesets with which the marker marks neighbor elements.");

  params.addRequiredParam<SubdomainID>(
      "new_subdomain_id", "Subdomain ID to set for the neighbor elements.");
  params.addParam<SubdomainName>(
      "new_subdomain_name", "Subdomain name to set for the neighbor elements.");

  params.addParam<std::vector<SubdomainID>>(
      "subdomain_ids", "This mesh modifier works only to subdomains with given subdomain IDs.");
  params.addParam<std::vector<SubdomainName>>(
      "subdomain_names", "This mesh modifier works only to subdomains with given subdoamin names.");

  return params;
}

SideNeighborSubdomain::SideNeighborSubdomain(const InputParameters & parameters)
  : MeshModifier(parameters),
    _new_subdomain_id(parameters.get<SubdomainID>("new_subdomain_id"))
{
}

void
SideNeighborSubdomain::modify()
{
  // Check that we have access to the mesh
  if (!_mesh_ptr)
    mooseError("_mesh_ptr must be initialized before calling SideNeighborSubdomain::modify()");

  _mesh_ptr->errorIfDistributedMesh("SideNeighborSubdomain");

  auto & mesh = _mesh_ptr->getMesh(); // This simplifies some expressions later.

  if (isParamValid("subdomain_ids") && isParamValid("subdomain_names"))
    mooseError("You must supply exactly one of subdomain_ids or subdomain_names.");
  if (isParamValid("subdomain_ids"))
  {
    _subdomain_ids = getParam<std::vector<SubdomainID>>("subdomain_ids");
  }
  else if (isParamValid("subdomain_names"))
  {
    _subdomain_names = getParam<std::vector<SubdomainName>>("subdomain_names");
    _subdomain_ids= _mesh_ptr->getSubdomainIDs(_subdomain_names);
  }

  std::set<BoundaryID> _sideset_ids;
  if (isParamValid("sidesets"))
  {
    auto & sideset_names = getParam<std::vector<BoundaryName>>("sidesets");
    for (auto & sideset_name : sideset_names)
      // getBoundaryID cannot be called in the constructor.
      _sideset_ids.insert(_mesh_ptr->getBoundaryID(sideset_name));
  }
  else
  {
    mooseError("'sidesets' param input is not valid.");
  }

  // Loop over the elements
  for (const auto & elem : mesh.active_element_ptr_range())
  {
    if (!_subdomain_ids.empty())
    {
      // If subdomain IDs are available and the current element's ID is not one of them, skip this element.
      if (std::find(_subdomain_ids.begin(), _subdomain_ids.end(), elem->subdomain_id()) == _subdomain_ids.end()) 
        continue;
    }
    /// Checking all the nodes for each element if they are on the sidesets.
    for (unsigned int node = 0; node < elem->n_nodes(); node++)
    {
      /// Loop over all the sidesets specified.
      for (auto sideset_id : _sideset_ids)
      {
        // Check if the current node is on the current sideset.
        if (_mesh_ptr->isBoundaryNode(elem->get_node(node)->id(), sideset_id))
        {
          elem->subdomain_id() = _new_subdomain_id;
          continue; // Once you find this element is a neighbor, no more search is needed.
        }
      }
    }
  }

  // Assign subdomain name, if provided
  if (isParamValid("new_subdomain_name"))
    mesh.subdomain_name(_new_subdomain_id) = getParam<SubdomainName>("new_subdomain_name");
}

