//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Material.h"
#include "ParametricStudyInterface.h"

class ParametricStudyMaterial;

/**
 * Material property for parametric study
 */
class ParametricStudyMaterial
  : public ParametricStudyInterface<Material>
{
public:
  static InputParameters validParams();

  ParametricStudyMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  virtual void timestepSetup() override;

  /// Name of the property to be computed
  std::string _mat_prop_name;

  /// Material property to be computed
  MaterialProperty<Real> & _mat_prop;
};
