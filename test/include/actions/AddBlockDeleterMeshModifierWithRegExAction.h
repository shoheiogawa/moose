//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef ADD_BLOCK_DELETER_MESH_MODIFIER_WITH_REG_EX_ACTION_H
#define ADD_BLOCK_DELETER_MESH_MODIFIER_WITH_REG_EX_ACTION_H

#include "Action.h"

// Forward Declaration
class AddBlockDeleterMeshModifierWithRegExAction;

template <>
InputParameters validParams<AddBlockDeleterMeshModifierWithRegExAction>();

class AddBlockDeleterMeshModifierWithRegExAction
  : public Action
{
public:
  AddBlockDeleterMeshModifierWithRegExAction(InputParameters params);

  virtual void act() override;

private:
  std::string _block_name;

  // Params for creating new boudnaries after blocks deleted.
  bool _new_boundary;
  std::string _new_boundary_prefix;
  std::string _new_boundary_suffix;
};

#endif // ADD_BLOCK_DELETER_MESH_MODIFIER_WITH_REG_EX_ACTION_H
