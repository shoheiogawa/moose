//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ADMatDiffusivity.h"

registerADMooseObject("MooseTestApp", ADMatDiffusivity);

defineADLegacyParams(ADMatDiffusivity);

template <ComputeStage compute_stage>
InputParameters
ADMatDiffusivity<compute_stage>::validParams()
{
  InputParameters params = ADMaterial<compute_stage>::validParams();
  params.addRequiredParam<MaterialPropertyName>("ad_mat_diffusivity",
                                                "Name of the ad material property this material defines");
  params.addRequiredParam<Real>("coef", "Coefficient");
  params.declareControllable("coef");
  return params;
}

template <ComputeStage compute_stage>
ADMatDiffusivity<compute_stage>::ADMatDiffusivity(const InputParameters & parameters)
  : ADMaterial<compute_stage>(parameters),
    _ad_mat_diffusivity(declareADProperty<Real>(getParam<MaterialPropertyName>("ad_mat_diffusivity"))),
    _coef(getParam<Real>("coef"))
{
}

template <ComputeStage compute_stage>
void
ADMatDiffusivity<compute_stage>::computeQpProperties()
{
  _ad_mat_diffusivity[_qp] = _coef * 10.0;
}
