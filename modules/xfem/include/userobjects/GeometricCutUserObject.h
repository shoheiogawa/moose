/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#ifndef GEOMETRICCUTUSEROBJECT_H
#define GEOMETRICCUTUSEROBJECT_H

// MOOSE includes
#include "GeneralUserObject.h"

// libMesh includes
#include "libmesh/libmesh_common.h"
#include "libmesh/libmesh.h" // libMesh::invalid_uint
#include "libmesh/elem.h"

using namespace libMesh;

struct CutEdge
{
  unsigned int id1;
  unsigned int id2;
  Real distance;
  unsigned int host_side_id;
};

struct CutFace
{
  unsigned int face_id;
  std::vector<unsigned int> face_edge;
  std::vector<Real> position;
};

// Forward declarations
class GeometricCutUserObject;

template <>
InputParameters validParams<GeometricCutUserObject>();

class GeometricCutUserObject : public GeneralUserObject
{
public:
  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  GeometricCutUserObject(const InputParameters & parameters);

  virtual void initialize(){};
  virtual void execute(){};
  virtual void finalize(){};

  virtual bool active(Real time) const = 0;

  virtual bool
  cutElementByGeometry(const Elem * elem, std::vector<CutEdge> & cut_edges, Real time) const = 0;
  virtual bool
  cutElementByGeometry(const Elem * elem, std::vector<CutFace> & cut_faces, Real time) const = 0;

  virtual bool cutFragmentByGeometry(std::vector<std::vector<Point>> & frag_edges,
                                     std::vector<CutEdge> & cut_edges,
                                     Real time) const = 0;
  virtual bool cutFragmentByGeometry(std::vector<std::vector<Point>> & frag_faces,
                                     std::vector<CutFace> & cut_faces,
                                     Real time) const = 0;

  Real cutFraction(unsigned int cut_num, Real time) const;

protected:
  std::vector<std::pair<Real, Real>> _cut_time_ranges;
};

#endif // GEOMETRICCUTUSEROBJECT_H
