//* This file is result of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "RenameSubdomainsWithRegEx.h"
#include "MooseMesh.h"

#include <cstdio> // sprintf
#include <regex>

registerMooseObject("MooseApp", RenameSubdomainsWithRegEx);

template <>
InputParameters
validParams<RenameSubdomainsWithRegEx>()
{
  InputParameters params = validParams<MeshModifier>();
  params.addRequiredParam<std::string>("input_subdomain_name",
      "Regular expression used to search for subdomains.");
  params.addRequiredParam<std::string>("output_subdomain_name",
      "Output subdomain name. This can have %s for matched value with the regular expression.");
  params.addParam<bool>("as_int", false, "Treat matched values as an integer instead of a string.");
  params.addParam<bool>("as_float", false, "Treat matched values as a float instead of a string.");
  return params;
}

RenameSubdomainsWithRegEx::RenameSubdomainsWithRegEx(const InputParameters & parameters)
  : MeshModifier(parameters),
  _input_subdomain_name(getParam<std::string>("input_subdomain_name")),
  _output_subdomain_name(getParam<std::string>("output_subdomain_name")),
  _as_int(getParam<bool>("as_int")),
  _as_float(getParam<bool>("as_float"))
{
  if (_as_int && _as_float)
    paramError("as_float", "as_int is specified to be true too. Only one of them should be to be true.");
}

void
RenameSubdomainsWithRegEx::modify()
{
  // Check that we have access to the mesh
  if (!_mesh_ptr)
    mooseError("_mesh_ptr must be initialized before calling RenameSubdomainsWithRegEx::modify()");

  // Getting the list of subdomain IDs in the mesh file and printing subdomain names to the console
  auto subdomain_ids = _mesh_ptr->meshSubdomains();
  std::vector<SubdomainName> subdomain_names;
  _console << "Subdomain names in the mesh file : ";
  for (auto subdomain_id : subdomain_ids)
  {
    auto subdomain_name = _mesh_ptr->getSubdomainName(subdomain_id);
    subdomain_names.push_back(subdomain_name);
    _console << subdomain_name << ", ";
  }
  _console << "\n";

  // Renaming subdomain names using the regular expression
  std::regex re(_input_subdomain_name);
  for (const auto & subdomain_id : subdomain_ids)
  {
    auto subdomain_name = _mesh_ptr->getSubdomainName(subdomain_id);
    std::smatch smatch_result;
    std::string match_result;
    if (std::regex_search(subdomain_name, smatch_result, re) && smatch_result.size() > 1) {
      match_result = smatch_result.str(1);
    } else {
      continue;
    }
    char name_buffer[256]; // for sprintf
    if (_as_int)
    {
      sprintf(name_buffer, _output_subdomain_name.c_str(), stoi(match_result));
    }
    else if (_as_float)
      sprintf(name_buffer, _output_subdomain_name.c_str(), stof(match_result));
    else
      sprintf(name_buffer, _output_subdomain_name.c_str(), match_result.c_str());
    std::string new_subdomain_name(name_buffer);
    _mesh_ptr->getMesh().subdomain_name(subdomain_id) = new_subdomain_name;
  }
  
  // Printing subdomain names to the console
  subdomain_names.resize(0);
  _console << "Subdomain names after modification : ";
  for (auto subdomain_id : subdomain_ids)
  {
    auto subdomain_name = _mesh_ptr->getSubdomainName(subdomain_id); 
    _console << subdomain_name << ", ";
  }
  _console << "\n";
}

