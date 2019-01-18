[Mesh]
  type = FileMesh
  file = poisson_with_sink_out.e
[]

[Variables]
  [u]
  []
[]

[Functions]
  [solution_func]
    type = SolutionFunction
    solution = solution_uo
  []
[]

[AuxVariables]
  [u_0]
  []
  [v_x]
    family = MONOMIAL
  []
  [v_y]
    family = MONOMIAL
  []
[]

[AuxKernels]
  [solution_aux]
    type = SolutionAux
    variable = u_0
    from_variable = u
    solution = solution_uo
    execute_on = 'INITIAL'
  []
  [set_v_x_aux]
    type = LevelSetSignedDistancVelocity
    psi_0 = 'u_0'
    epsilon = 0.1
    component = x
    psi = 'u'
    variable = v_x
  []
  [set_v_y_aux]
    type = LevelSetSignedDistancVelocity
    psi_0 = 'u_0'
    epsilon = 0.1
    component = y
    psi = 'u'
    variable = v_y
  []
[]

[Kernels]
  [ls_advection]
    type = LevelSetAdvection
    velocity_y = 'v_y'
    velocity_x = 'v_x'
    variable = u
  []
  [ls_signed_dist_source]
    type = LevelSetSignedDistanceSource
    psi_0 = 'u_0'
    epsilon = 0.1
    variable = u
  []
  [time_derivative]
    type = TimeDerivative
    variable = u
  []
[]

[ICs]
  [solution_func_ic]
    type = FunctionIC
    function = solution_func
    variable = u
  []
[]

[BCs]
  [zero]
    type = DirichletBC
    variable = u
    boundary = 'zero'
    value = 0.0
  []
[]

[UserObjects]
  [solution_uo]
    type = SolutionUserObject
    timestep = LATEST
    system_variables = 'u'
    mesh = poisson_with_sink_out.e
  []
[]

[Executioner]
  type = Transient
  petsc_options_iname = '-pc_type -sub_pc_type'
  dt = 0.1
  petsc_options_value = 'asm ilu'
  solve_type = PJFNK
[]

[Outputs]
  exodus = true
[]

[VectorPostprocessors]
  [u_along_x_center_y]
    type = LineValueSampler
    num_points = 100
    end_point = '1.0 0.5 0.0'
    variable = 'u'
    sort_by = x
    start_point = '0.0 0.5 0.0'
  []
[]

[Preconditioning]
  [smp_full]
    type = SMP
    full = true
  []
[]
