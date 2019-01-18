//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef LEVEL_SET_LEVEL_SET_SIGNED_DISTANCE_SOURCE_SUPG_H
#define LEVEL_SET_LEVEL_SET_SIGNED_DISTANCE_SOURCE_SUPG_H

// MOOSE includes
#include "Kernel.h"
#include "LevelSetVelocityInterface.h"

// Forward declarations
class LevelSetSignedDistanceSourceSUPG;

template <>
InputParameters validParams<LevelSetSignedDistanceSourceSUPG>();

/**
 * SUPG stabilization term for a forcing signed distance source term.
 */
class LevelSetSignedDistanceSourceSUPG : public LevelSetVelocityInterface<Kernel>
{
public:
  LevelSetSignedDistanceSourceSUPG(const InputParameters & parameters);

protected:
  Real computeQpResidual() override;

  Real smoothStepFunction(Real psi) const;

  const VariableValue & _psi_0;

  /// Value for smoothed distance function
  const Real _epsilon;

  /// Scale for the interface velocity
  const Real _scale;
};

#endif // LEVEL_SET_LEVEL_SET_SIGNED_DISTANCE_SOURCE_SUPG_H
