//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef SIDE_NEIGHBOR_SUBDOMAIN_H
#define SIDE_NEIGHBOR_SUBDOMAIN_H

#include "MeshModifier.h"

class SideNeighborSubdomain;

template <>
InputParameters validParams<SideNeighborSubdomain>();

/**
 * MeshModifier for defining a subdomain near the sidesets specified.
 */
class SideNeighborSubdomain : public MeshModifier
{
public:
  SideNeighborSubdomain(const InputParameters & parameters);

  virtual void modify() override;

private:

  // SubdomainID and SubdomainName are to find which subdomains this mesh modifier works to.
  std::vector<SubdomainID> _subdomain_ids;
  std::vector<SubdomainName> _subdomain_names;

  // Subdomain ID to assign to the newly created subdomain.
  SubdomainID _new_subdomain_id;
};

#endif // SIDE_NEIGHBOR_SUBDOMAIN_H
