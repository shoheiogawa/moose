//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Marker.h"

class SideNeighborMarker;

template <>
InputParameters validParams<SideNeighborMarker>();

class SideNeighborMarker : public Marker
{
public:
  static InputParameters validParams();

  SideNeighborMarker(const InputParameters & parameters);

protected:
  virtual MarkerValue computeElementMarker() override;

  bool searchForSidesets(const Elem * elem, bool previous_elem,
                         unsigned int previous_elem_id, unsigned int depth);

  /// Value for what to do with marked elements
  MarkerValue _marker_value;

  /// Store the IDs of side sets where this marker works on
  std::set<BoundaryID> _mark_sideset_ids;

  /// Store a boundary info object once
  BoundaryInfo & _boundary_info;

  /// The nubmer of elements (jumps) from the side sets to be refined
  unsigned int _depth;
};
