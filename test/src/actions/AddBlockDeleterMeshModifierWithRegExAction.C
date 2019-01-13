//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "AddBlockDeleterMeshModifierWithRegExAction.h"
#include "MooseApp.h"
#include "MooseMesh.h"
#include "Factory.h"
#include <regex>

registerMooseAction("MooseTestApp", AddBlockDeleterMeshModifierWithRegExAction, "meta_action");

template <>
InputParameters
validParams<AddBlockDeleterMeshModifierWithRegExAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::string>("block_name", "Regular expression used to search for blocks.");
  params.addParam<bool>("new_boundary", false, "Create new boundaries using the surface of deleted bocks.");
  params.addParam<std::string>("new_boundary_prefix", "", "Prefix to be added at the beginning of deleted block name.");
  params.addParam<std::string>("new_boundary_suffix", "", "Suffix to be added at the end of deleted block name.");

  return params;
}

AddBlockDeleterMeshModifierWithRegExAction::AddBlockDeleterMeshModifierWithRegExAction(InputParameters params)
  : Action(params),
  _block_name(getParam<std::string>("block_name")),
  _new_boundary(getParam<bool>("new_boundary")),
  _new_boundary_prefix(getParam<std::string>("new_boundary_prefix")),
  _new_boundary_suffix(getParam<std::string>("new_boundary_suffix"))
{
}

void
AddBlockDeleterMeshModifierWithRegExAction::act()
{
  auto & mesh_ptr = _app.actionWarehouse().mesh();
  if (!mesh_ptr)
    mooseError("mesh_ptr must be initialized before calling AddBlockDeleterMeshModifierWithRegExAction::act()");

  auto subdomain_ids = mesh_ptr->meshSubdomains();

  _console << "Deleting following blocks : \n";
  std::regex re(_block_name);
  std::smatch match_result;
  for (auto subdomain_id : subdomain_ids)
  {
    auto subdomain_name = mesh_ptr->getSubdomainName(subdomain_id);

    bool to_be_deleted = std::regex_match(subdomain_name, match_result, re);
    if (to_be_deleted)
    {
      _console << subdomain_name;
      InputParameters params = _factory.getValidParams("BlockDeleter");
      params.set<SubdomainID>("block_id") = subdomain_id;
      if (_new_boundary)
      {
        BoundaryName new_boudnary_name = BoundaryName(_new_boundary_prefix + subdomain_name + _new_boundary_suffix);
        params.set<BoundaryName>("new_boundary") = new_boudnary_name;
        _console << " (New boundary : " << new_boudnary_name << ")";
      }
      _app.addMeshModifier("BlockDeleter", "delete_" + subdomain_name, params);
      _console << "\n";
    }
  }
}

