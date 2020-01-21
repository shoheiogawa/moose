//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "ParametricStudyMaterial.h"

registerMooseObject("MooseApp", ParametricStudyMaterial);

defineLegacyParams(ParametricStudyMaterial);

InputParameters
ParametricStudyMaterial::validParams()
{
  InputParameters params = Material::validParams();
  params.addClassDescription(
      "Material property for parametric study");
  params.addRequiredParam<std::string>(
      "mat_prop", "The name of the material property");
  params += ParametricStudyInterface<Material>::validParams();
  return params;
}

ParametricStudyMaterial::ParametricStudyMaterial(const InputParameters & parameters)
  : ParametricStudyInterface<Material>(parameters),
    _mat_prop_name(getParam<std::string>("mat_prop")),
    _mat_prop(declareProperty<Real>(_mat_prop_name))
{
}

void
ParametricStudyMaterial::computeQpProperties()
{
  _mat_prop[_qp] = getCurrentParamValue();
}

void
ParametricStudyMaterial::timestepSetup()
{
  setCurrentParamValue();
  _console << "Parametric study for material property: " + _mat_prop_name << " = ";
  showParamInfoToConsole();
}
