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
  /// Block ID to assign to the region
  SubdomainID _block_id;
};

#endif // SIDE_NEIGHBOR_SUBDOMAIN_H
