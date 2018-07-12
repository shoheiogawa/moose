//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef SIDE_NEIGHBOR_MARKER_H
#define SIDE_NEIGHBOR_MARKER_H

#include "Marker.h"

class SideNeighborMarker;

template <>
InputParameters validParams<SideNeighborMarker>();

class SideNeighborMarker : public Marker
{
public:
  SideNeighborMarker(const InputParameters & parameters);

protected:
  virtual MarkerValue computeElementMarker() override;
  bool searchForSidesets(const Elem * elem, unsigned int depth);

  MarkerValue _marker_value;
  std::set<BoundaryID> _mark_sideset_ids;
  BoundaryInfo & _boundary_info;

  unsigned int _depth;
};

#endif /* SIDE_NEIGHBOR_MARKER_H */
