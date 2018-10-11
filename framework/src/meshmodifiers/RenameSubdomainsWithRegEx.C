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
  params.addParam<unsigned int>("num_of_replace", 1, "Number of format specifiers in output_subdomain_name");
  params.addParam<bool>("as_int", false, "Treat matched values as an integer instead of a string.");
  params.addParam<bool>("as_float", false, "Treat matched values as a float instead of a string.");
  return params;
}

RenameSubdomainsWithRegEx::RenameSubdomainsWithRegEx(const InputParameters & parameters)
  : MeshModifier(parameters),
  _input_subdomain_name(getParam<std::string>("input_subdomain_name")),
  _output_subdomain_name(getParam<std::string>("output_subdomain_name")),
  _num_of_replace(getParam<unsigned int>("num_of_replace")),
  _as_int(getParam<bool>("as_int")),
  _as_float(getParam<bool>("as_float"))
{
  if (_num_of_replace > 4)
    paramError("num_of_replace", "Not implemented yet for num_of_replace > 4");
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
    std::smatch smatch_results;
    std::vector<std::string> matches;
    if (std::regex_search(subdomain_name, smatch_results, re) && smatch_results.size() > 1) {
      _console << "Matched\n";
      matches.resize(smatch_results.size() - 1);
      for (unsigned int i = 0; i < matches.size(); ++i)
      {
        _console << smatch_results.str(i + 1) << "\n";
        matches.push_back(smatch_results.str(i + 1));
      }
    } 
    else 
    {
      continue;
    }

    if (_num_of_replace > matches.size())
      mooseError("Number of matches is smaller than format specifiers.");

    char name_buffer[256]; // for sprintf
    if (_as_int)
    {
      switch (_num_of_replace)
      {
        case 1 : sprintf(name_buffer, _output_subdomain_name.c_str(), stoi(matches[0]));
        case 2 : sprintf(name_buffer, _output_subdomain_name.c_str(), stoi(matches[0]), stoi(matches[1]));
        case 3 : sprintf(name_buffer, _output_subdomain_name.c_str(), stoi(matches[0]), stoi(matches[1]), stoi(matches[2]));
        case 4 : sprintf(name_buffer, _output_subdomain_name.c_str(), stoi(matches[0]), stoi(matches[1]), stoi(matches[2]), stoi(matches[3]));
        default : mooseError("Not implemented yet for num_of_replace > 4");
      }
    }
    else if (_as_float)
    {
      switch (_num_of_replace)
      {
        case 1 : sprintf(name_buffer, _output_subdomain_name.c_str(), stof(matches[0]));
        case 2 : sprintf(name_buffer, _output_subdomain_name.c_str(), stof(matches[0]), stof(matches[1]));
        case 3 : sprintf(name_buffer, _output_subdomain_name.c_str(), stof(matches[0]), stof(matches[1]), stof(matches[2]));
        case 4 : sprintf(name_buffer, _output_subdomain_name.c_str(), stof(matches[0]), stof(matches[1]), stof(matches[2]), stof(matches[3]));
        default : mooseError("Not implemented yet for num_of_replace > 4");
      }
    }
    else
    {
      switch (_num_of_replace)
      {
        case 1 : sprintf(name_buffer, _output_subdomain_name.c_str(), matches[0].c_str());
        case 2 : sprintf(name_buffer, _output_subdomain_name.c_str(), matches[0].c_str(), matches[1].c_str());
        case 3 : sprintf(name_buffer, _output_subdomain_name.c_str(), matches[0].c_str(), matches[1].c_str(), matches[2].c_str());
        case 4 : sprintf(name_buffer, _output_subdomain_name.c_str(), matches[0].c_str(), matches[1].c_str(), matches[2].c_str(), matches[3].c_str());
        default : mooseError("Not implemented yet for num_of_replace > 4");
      }
    }
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

