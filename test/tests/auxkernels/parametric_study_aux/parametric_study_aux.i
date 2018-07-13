[Mesh]
  type = GeneratedMesh
  nx = 2
  ny = 2
  nz = 2
  dim = 2
[]

[Variables]
  [u]
  []
[]

[AuxVariables]
  [u_aux]
  []
[]

[Kernels]
  [diffusion]
    type = Diffusion
    variable = u
  []
[]

[AuxKernels]
  [parametric_study_u_aux]
    type = ParametricStudyAux
    variable = u_aux
    range_func = '1.0 2.0 0.2'
    tolerance = 1e-3
    sort = ascending
    value_list = '2.1 -1.0 1.7'
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = 'left'
    value = 0.0
  []
  [right]
    type = PostprocessorDirichletBC
    variable = u
    boundary = 'right'
    postprocessor = average_u_aux
  []
[]

[Executioner]
  type = Transient
  start_time = 0
  end_time = 8
  dt = 0.5
  solve_type = PJFNK
  nl_rel_tol = 1e-03
  nl_abs_tol = 1e-20
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]

[Postprocessors]
  [average_u_aux]
    type = SideAverageValue
    variable = 'u_aux'
    execute_on = 'TIMESTEP_BEGIN'
    boundary = 'right'
  []
[]

[Adaptivity]
  cycles_per_step = 2
  marker = marker
  max_h_level = 2
  [Indicators]
    [indicator]
      type = GradientJumpIndicator
      variable = u
    []
  []
  [Markers]
    [marker]
      type = ErrorFractionMarker
      indicator = indicator
      coarsen = 0.1
      refine = 0.7
    []
  []
[]
