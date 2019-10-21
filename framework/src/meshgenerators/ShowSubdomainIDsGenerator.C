//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ShowSubdomainIDsGenerator.h"
#include "CastUniquePointer.h"

#include "libmesh/elem.h"
#include "MooseMeshUtils.h"
#include "MooseMesh.h"

#include <set>

registerMooseObject("MooseApp", ShowSubdomainIDsGenerator);

template <>
InputParameters
validParams<ShowSubdomainIDsGenerator>()
{
  InputParameters params = validParams<MeshGenerator>();

  params.addRequiredParam<MeshGeneratorName>("input", "The mesh of interest");
  return params;
}

ShowSubdomainIDsGenerator::ShowSubdomainIDsGenerator(const InputParameters & parameters)
  : MeshGenerator(parameters), _input(getMesh("input"))
{
}

std::unique_ptr<MeshBase>
ShowSubdomainIDsGenerator::generate()
{
  std::unique_ptr<MeshBase> mesh = std::move(_input);

  std::set<subdomain_id_type> subdomain_ids;
  mesh->subdomain_ids(subdomain_ids);
  _console << "Existing subdomain IDs: ";
  for (auto subdomain_id : subdomain_ids)
    _console << subdomain_id << ", ";
  _console << "\n";

  return dynamic_pointer_cast<MeshBase>(mesh);
}
