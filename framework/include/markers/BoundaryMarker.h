//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef BOUNDARY_MARKER_H
#define BOUNDARY_MARKER_H

#include "Marker.h"

class BoundaryMarker;

template <>
InputParameters validParams<BoundaryMarker>();

class BoundaryMarker : public Marker
{
public:
  BoundaryMarker(const InputParameters & parameters);

protected:
  virtual MarkerValue computeElementMarker() override;
  //virtual void markerSetup() override;
  bool searchForBoundaries(const Elem * elem, unsigned int depth);

  MarkerValue _marker_value;
  std::set<BoundaryID> _mark_boundary_ids;
  BoundaryInfo & _boundary_info;

  unsigned int _depth;
};

#endif /* BOUNDARY_MARKER_H */
