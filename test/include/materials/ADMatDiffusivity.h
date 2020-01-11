//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ADMaterial.h"

template <ComputeStage>
class ADMatDiffusivity;

declareADValidParams(ADMatDiffusivity);

/**
 * An AD material with a controllable parameters
 */
template <ComputeStage compute_stage>
class ADMatDiffusivity : public ADMaterial<compute_stage>
{
public:
  static InputParameters validParams();

  ADMatDiffusivity(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

  ADMaterialProperty(Real) & _ad_mat_diffusivity;

  const Real & _coef;

  usingMaterialMembers;
};
