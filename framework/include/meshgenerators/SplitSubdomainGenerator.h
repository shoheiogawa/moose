//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MeshGenerator.h"
#include "MooseEnum.h"

#include "libmesh/bounding_box.h"

#include <string>
#include <queue>
#include <unordered_set>

// Forward declarations
class SplitSubdomainGenerator;

template <>
InputParameters validParams<SplitSubdomainGenerator>();

namespace libMesh
{
class BoundingBox;
}

/**
 * MeshGenerator for splitting unconnected subdomain of given IDs
 */
class SplitSubdomainGenerator : public MeshGenerator
{
public:
  SplitSubdomainGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate() override;

protected:
  /// Search for connected elements recursively
  void connectedElementSearch(std::queue<const Elem *> & elem_queue,
                              std::unordered_set<dof_id_type> & connected_element_ids,
                              std::unordered_set<dof_id_type> & visited_element_ids);

  /// Input mesh pointer
  std::unique_ptr<MeshBase> & _input;

  /// The IDs of the subdomains on which this mesh generator works
  std::vector<subdomain_id_type> _subdomain_ids;

  /// The std::unordered_set version of _subdomain_ids for to use `count` function
  std::unordered_set<subdomain_id_type> _subdomain_id_set;

  /// The subdomain ID to be set for the first isolated domain
  /// This ID is incremented for one isolated domain after another.
  subdomain_id_type _new_subdomain_id_start;

  /// The number used in the new subdomain name of the first split domain 
  /// to distinguish this particular isolated domain.
  /// This number is incremented for one isolated domain after another.
  unsigned int _new_name_id_start;

  /// Prefix to be added in front of subdomain IDs
  std::string _prefix;

  /// Suffix to be added after subdomain IDs
  std::string _suffix;

  /// 0s are added in front of the connected domain IDs
  /// to make the number of digits this number.
  unsigned int _num_digits;
};
