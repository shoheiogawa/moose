//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "SplitSubdomainGenerator.h"
#include "CastUniquePointer.h"

#include "libmesh/elem.h"
#include "libmesh/remote_elem.h"

registerMooseObject("MooseApp", SplitSubdomainGenerator);

template <>
InputParameters
validParams<SplitSubdomainGenerator>()
{
  InputParameters params = validParams<MeshGenerator>();

  params.addRequiredParam<MeshGeneratorName>("input", "The mesh we want to modify");
  params.addClassDescription("Split the sub-domains with given sub-domain IDs based "
                             "on spacial connectivity.");
  params.addParam<std::vector<subdomain_id_type>>("subdomain_ids",
                                "The IDs of the subdomains on which "
                                "this mesh generator works.");
  params.addRequiredParam<subdomain_id_type>("new_subdomain_id_start",
                                "The subdomain ID to set for the first split domain. "
                                "This ID is incremented for one after another.");
  params.addRequiredParam<unsigned int>("new_name_id_start",
                                "The number used in the subdomain name of the first split domain. "
                                "This number is incremented for one after another.");
  params.addParam<std::string>("prefix", "", "Prefix to be added in front of subdomain ID");
  params.addParam<std::string>("suffix", "", "Suffix to be added after subdomain ID");
  params.addParam<unsigned int>("num_digits", "0s are added in front of connected ID "
      "to make the number of digits this number, e.g., if num_digits = 5, 123 will be 00123.");

  return params;
}

SplitSubdomainGenerator::SplitSubdomainGenerator(const InputParameters & parameters)
  : MeshGenerator(parameters),
    _input(getMesh("input")),
    _subdomain_ids(parameters.get<std::vector<subdomain_id_type>>("subdomain_ids")),
    _subdomain_id_set(_subdomain_ids.begin(), _subdomain_ids.end()),
    _new_subdomain_id_start(parameters.get<subdomain_id_type>("new_subdomain_id_start")),
    _new_name_id_start(parameters.get<unsigned int>("new_name_id_start")),
    _prefix(parameters.get<std::string>("prefix")),
    _suffix(parameters.get<std::string>("suffix")),
    _num_digits(parameters.get<unsigned int>("num_digits"))
{
}

std::unique_ptr<MeshBase>
SplitSubdomainGenerator::generate()
{
  // This algorithm doesn't work with multiple cores even on replicated mesh
  // because if in parallel, each processor potential look at different unconnected
  // domains. To make this code in parallel, it is necessary to match which domain on one
  // processor is connected to which domain in the mesh of other processors.
  // This can be possibly done by storing the element ID of remote elements if they
  // have the same subdomain IDs and storing the processor ID of the remote elements.
  // After all elements are processed in each processor, check the element IDs of each
  // set on one processor with those on the processor with the stored IDs.

  std::unique_ptr<MeshBase> mesh = std::move(_input);

  if (typeid(_input).name() == typeid(std::unique_ptr<DistributedMesh>).name())
    mooseError("SplitSubdomainGenerator only works with ReplicatedMesh");

  if (mesh->n_processors() > 1)
  {
    mooseWarning("SplitSubdomainGenerator is using only a single processor"
                 " due to its algorithm");
    if (mesh->processor_id() != 0)
      return dynamic_pointer_cast<MeshBase>(mesh);
  }

  std::unordered_set<dof_id_type> connected_element_ids;
  std::unordered_set<dof_id_type> visited_element_ids;

  std::queue<const Elem *> elem_queue;

  auto new_subdomain_id = _new_subdomain_id_start;

  unsigned int new_name_id = isParamValid("new_name_id_start") ? _new_name_id_start : 0;

  // Loop over the elements
  std::cout << "Showing elements ID if the last two digits are 0\n";
  for (auto & elem : mesh->element_ptr_range())
  {
    subdomain_id_type curr_subdomain = elem->subdomain_id();

    if (elem->id() % 10000 == 0)
      std::cout << elem->id() << ", ";
    // We only need to loop over elements in the master subdomain
    if (_subdomain_id_set.count(curr_subdomain) == 0)
      continue;

    // We only start with elements we haven't visited so far
    if (visited_element_ids.count(elem->id()) != 0)
      continue;

    std::cout << "Starting searching in a new isolated domain: " << new_subdomain_id << "\n";

    connected_element_ids.clear();
    elem_queue.empty();

    elem_queue.push(elem);
    while (!elem_queue.empty())
    {
      auto elem = elem_queue.front();
      elem_queue.pop();

      connected_element_ids.insert(elem->id());
      visited_element_ids.insert(elem->id());
      for (unsigned int side = 0; side < elem->n_sides(); side++)
      {
        const Elem * neighbor = elem->neighbor_ptr(side);

        if (neighbor != NULL && _subdomain_id_set.count(neighbor->subdomain_id()) > 0)
        {
          // This neighbor element is connected to the current element.
          if (visited_element_ids.count(neighbor->id()) == 1)
          {
            continue; // Already visited this neighbor element
          }
          else
          {
            elem_queue.push(neighbor);
          }
        }
      }
    }
    //connectedElementSearch(elem_queue, connected_element_ids, visited_element_ids);

    // Change the subdomain ID of the whole connected domain
    for (const auto & elem_id : connected_element_ids)
    {
      Elem * elem = mesh->query_elem_ptr(elem_id);
      elem->subdomain_id() = new_subdomain_id;
    }

    // Assign a subdomain name, if provided
    if (isParamValid("new_name_id_start"))
    {
      std::string subdomain_name = std::to_string(new_name_id);
      if (isParamValid("num_digits"))
      {
        int num_digits = subdomain_name.length();
        int diff = _num_digits - num_digits;
        if (diff > 0)
          for (int i = 0; i < diff; i++)
            subdomain_name = '0' + subdomain_name;
      }
      mesh->subdomain_name(new_subdomain_id) = _prefix + subdomain_name + _suffix;
    }

    // New connected domain has been registered and prepare for the next one
    new_subdomain_id++;
    new_name_id++;
  }
  return dynamic_pointer_cast<MeshBase>(mesh);
}

void
SplitSubdomainGenerator::connectedElementSearch(std::queue<const Elem *> & elem_queue,
                       std::unordered_set<dof_id_type> & connected_element_ids,
                       std::unordered_set<dof_id_type> & visited_element_ids)
{
  while (!elem_queue.empty())
  {
    auto elem = elem_queue.front();
    elem_queue.pop();

    connected_element_ids.insert(elem->id());
    visited_element_ids.insert(elem->id());
    for (unsigned int side = 0; side < elem->n_sides(); side++)
    {
      const Elem * neighbor = elem->neighbor_ptr(side);

      if (neighbor != NULL && _subdomain_id_set.count(neighbor->subdomain_id()) > 0)
      {
        // This neighbor element is connected to the current element.
        if (visited_element_ids.count(neighbor->id()) == 1)
        {
          continue; // Already visited this neighbor element
        }
        else
        {
          elem_queue.push(neighbor);
        }
      }
    }
  }
}
