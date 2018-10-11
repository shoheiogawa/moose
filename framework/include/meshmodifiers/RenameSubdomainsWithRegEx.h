//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef RENAME_SUBDOMAINS_WITH_REG_EX_H
#define RENAME_SUBDOMAINS_WITH_REG_EX_H

#include "MeshModifier.h"

class RenameSubdomainsWithRegEx;

template <>
InputParameters validParams<RenameSubdomainsWithRegEx>();

/**
 * MeshModifier for re-naming subdomains with regular expression
 */
class RenameSubdomainsWithRegEx : public MeshModifier
{
public:
  /**
   * Class constructor
   * @param parameters The input parameters
   */
  RenameSubdomainsWithRegEx(const InputParameters & parameters);

protected:
  virtual void modify() override;

private:
  std::string _input_subdomain_name;
  std::string _output_subdomain_name;
  unsigned int _num_of_replace;
  bool _as_int;
  bool _as_float;
};

#endif // RENAME_SUBDOMAINS_WITH_REG_EX_H
