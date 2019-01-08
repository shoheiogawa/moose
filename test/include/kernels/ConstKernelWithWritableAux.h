//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef CONST_KERNEL_WITH_WRITABLE_AUX_BC_H
#define CONST_KERNEL_WITH_WRITABLE_AUX_BC_H

#include "Kernel.h"

class ConstKernelWithWritableAux;

template <>
InputParameters validParams<ConstKernelWithWritableAux>();

/**
 * Implements a simple constant Neumann BC where grad(u)=value on the boundary.
 * Uses the term produced from integrating the diffusion operator by parts.
 */
class ConstKernelWithWritableAux : public Kernel
{
public:
  /**
   * Factory constructor, takes parameters so that all derived classes can be built using the same
   * constructor.
   */
  ConstKernelWithWritableAux(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;

  VariableValue & _aux_var;

  /// Value of grad(u) on the boundary.
  const Real & _value;
};

#endif // CONST_KERNEL_WITH_WRITABLE_AUX_BC_H
