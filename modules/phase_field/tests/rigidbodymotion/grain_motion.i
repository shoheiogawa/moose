# test file for applyting advection term and observing rigid body motion of grains
[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 25
  ny = 15
  nz = 0
  xmax = 50
  ymax = 25
  zmax = 0
  elem_type = QUAD4
[]

[Variables]
  [./c]
    order = FIRST
    family = LAGRANGE
  [../]
  [./w]
    order = FIRST
    family = LAGRANGE
  [../]
  [./eta]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  [./vadv00]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./vadv01]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./vadv0_div]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[Kernels]
  [./c_res]
    type = SplitCHParsed
    variable = c
    f_name = F
    kappa_name = kappa_c
    w = w
    args = eta
  [../]
  [./w_res]
    type = SplitCHWRes
    variable = w
    mob_name = M
  [../]
  [./time]
    type = CoupledTimeDerivative
    variable = w
    v = c
  [../]
  [./motion]
    type = MultiGrainRigidBodyMotion
    variable = w
    c = c
    v = eta
  [../]
  [./eta_dot]
    type = TimeDerivative
    variable = eta
  [../]
  [./vadv_eta]
    type = SingleGrainRigidBodyMotion
    variable = eta
    c = c
    v = eta
  [../]
  [./acint_eta]
    type = ACInterface
    variable = eta
    mob_name = M
    args = c
    kappa_name = kappa_eta
  [../]
  [./acbulk_eta]
    type = ACParsed
    variable = eta
    mob_name = M
    f_name = F
    args = c
  [../]
[]

[AuxKernels]
  [./vadv00]
    type = MaterialStdVectorRealGradientAux
    variable = vadv00
    property = advection_velocity
  [../]
  [./vadv01]
    type = MaterialStdVectorRealGradientAux
    variable = vadv01
    property = advection_velocity
    component = 1
  [../]
  [./vadv0_div]
    type = MaterialStdVectorAux
    variable = vadv0_div
    property = advection_velocity_divergence
  [../]
[]

[Materials]
  [./pfmobility]
    type = GenericConstantMaterial
    block = 0
    prop_names = 'M    kappa_c  kappa_eta'
    prop_values = '5.0  2.0      0.1'
  [../]
  [./free_energy]
    type = DerivativeParsedMaterial
    block = 0
    args = 'c eta'
    constant_names = 'barr_height  cv_eq'
    constant_expressions = '0.1          1.0e-2'
    function = 16*barr_height*(c-cv_eq)^2*(1-cv_eq-c)^2+(c-eta)^2
    derivative_order = 2
  [../]
  [./advection_vel]
    type = GrainAdvectionVelocity
    block = 0
    grain_force = grain_force
    etas = eta
    c = c
    grain_data = grain_center
  [../]
[]

[VectorPostprocessors]
  [./centers]
    type = GrainCentersPostprocessor
    grain_data = grain_center
  [../]
  [./forces]
    type = GrainForcesPostprocessor
    grain_force = grain_force
  [../]
[]

[UserObjects]
  [./grain_center]
    type = ComputeGrainCenterUserObject
    etas = eta
    execute_on = 'initial timestep_end'
  [../]
  [./grain_force]
    type = ConstantGrainForceAndTorque
    execute_on = 'initial timestep_end'
    force = '0.5 0.0 0.0 '
    torque = '0.0 0.0 10.0 '
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Transient
  nl_max_its = 30
  scheme = bdf2
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -ksp_grmres_restart -sub_ksp_type -sub_pc_type -pc_asm_overlap'
  petsc_options_value = 'asm         31   preonly   lu      1'
  l_max_its = 30
  l_tol = 1.0e-4
  nl_rel_tol = 1.0e-10
  start_time = 0.0
  dt = 0.5
  num_steps = 1
[]

[Outputs]
  output_initial = true
  exodus = true
  print_perf_log = true
[]

[ICs]
  [./rect_c]
    y2 = 20.0
    y1 = 5.0
    inside = 1.0
    x2 = 30.0
    variable = c
    x1 = 10.0
    type = BoundingBoxIC
  [../]
  [./rect_eta]
    y2 = 20.0
    y1 = 5.0
    inside = 1.0
    x2 = 30.0
    variable = eta
    x1 = 10.0
    type = BoundingBoxIC
  [../]
[]
