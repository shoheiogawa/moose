//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef ADVECTIVEFLUXCALCULATOR_H
#define ADVECTIVEFLUXCALCULATOR_H

#include "ElementLoopUserObject.h"

/**
 * Computes K_ij and R+ and R- detailed in
 * D Kuzmin and S Turek "High-resolution FEM-TVD schemes based on a fully multidimensional flux
 * limiter" Journal of Computational Physics 198 (2004) 131-158
 *
 * K_ij is a measure of flux from node i to node j
 * R^+_i and R^-_i quantify how much antidiffusion to allow around node i
 **/
class AdvectiveFluxCalculator;

template <>
InputParameters validParams<AdvectiveFluxCalculator>();

class AdvectiveFluxCalculator : public ElementLoopUserObject
{
public:
  AdvectiveFluxCalculator(const InputParameters & parameters);

  /// Size _kij, if needed
  virtual void timestepSetup() override;

  /// Call ElementLoopUserObject::meshChanged() and set _init_k_and_compute_valence to true
  virtual void meshChanged() override;

  /// Zeroes _kij
  virtual void pre() override;

  /// Compute contributions to _kij from the current element
  virtual void computeElement() override;

  /**
   * Returns the value of R_{i}^{+}, Eqn (49) of KT
   * @param node_i nodal id
   */
  Real rPlus(dof_id_type node_i) const;

  /**
   * Returns the value of R_{i}^{-}, Eqn (49) of KT
   * @param node_i nodal id
   */
  Real rMinus(dof_id_type node_i) const;

  /**
   * Returns the value of k_ij as computed by KT Eqns (18)-(20).
   * @param node_i id of i^th node
   * @param node_j id of j^th node
   * @return k_ij of KT
   */
  Real getKij(dof_id_type node_i, dof_id_type node_j) const;

  /**
   * Returns the valence of the i-j edge.
   * Valence is the number of times the edge is encountered in a loop over elements (that have appropriate subdomain_id, if the user has employed the "blocks=" parameter) seen by this processor
   * @param node_i id of i^th node
   * @param node_j id of j^th node
   * @return valence of the i-j edge
   */
  unsigned getValence(dof_id_type node_i, dof_id_type node_j) const;

  /**
   * flux limiter, L, on Page 135 of Kuzmin and Turek
   */
  Real limitFlux(Real a, Real b) const;

protected:
  /// advection velocity
  RealVectorValue _velocity;

  /// the nodal values of u
  MooseVariable * _u_nodal;

  /// the moose variable number of u
  unsigned _u_var_num;

  /// Kuzmin-Turek shape function
  const VariablePhiValue & _phi;

  /// grad(Kuzmin-Turek shape function)
  const VariablePhiGradient & _grad_phi;

  /// whether _kij needs to be sized appropriately and valence computed
  bool _init_k_and_compute_valence;

  /**
   * Determines Flux Limiter type (Page 135 of Kuzmin and Turek)
   * "None" means that limitFlux=0 always, which implies zero antidiffusion will be added
   */
  const enum class FluxLimiterTypeEnum { MinMod, VanLeer, MC, superbee, None } _flux_limiter_type;

  /**
   * Kuzmin-Turek K_ij matrix.  Along with R+ and R-, this is the key quantity computed
   * by this UserObject.
   * _kij[i][j] = k_ij corresponding to the i-j node pair.
   * Because it explicitly holds info which nodes are paired with which other nodes, it is
   * also used to perform: given a node_id, loop over all neighbouring nodes.
   * This occurs in the computation of R+ and R-.
   */
  std::map<dof_id_type, std::map<dof_id_type, Real>> _kij;

  /// For an pre-allocated K_ij, zero all its entries
  void zeroKij();

  /**
   * _valence[(i, j)] = number of times, in a loop over elements owned by this processor,
   * and are part of the block-restricted blocks of this UserObject, that the i-j edge is encountered
   */
  std::map<std::pair<dof_id_type, dof_id_type>, unsigned> _valence;

  /// Signals to the PQPlusMinus method what should be computed
  enum class PQPlusMinusEnum
  {
    PPlus,
    PMinus,
    QPlus,
    QMinus
  };

  /**
   * Returns the value of P_{i}^{+}, P_{i}^{-}, Q_{i}^{+} or Q_{i}^{-} (depending on pq_plus_minus)
   * which are defined in Eqns (47) and (48) of KT
   * @param node_i nodal id
   * @param pq_plus_minus indicates whether P_{i}^{+}, P_{i}^{-}, Q_{i}^{+} or Q_{i}^{-} should be
   * returned
   */
  Real PQPlusMinus(dof_id_type node_i, const PQPlusMinusEnum pq_plus_minus) const;

};

#endif // ADVECTIVEFLUXCALCULATOR_H
