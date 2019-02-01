[Mesh]
  type = GeneratedMesh
  nx = 4
  ny = 4
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
    value_list = '0.8 1.2 1.3'
    unique_list = false
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
  dt = 1
  solve_type = PJFNK
  nl_rel_tol = 1e-03
  nl_abs_tol = 1e-12
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]

[Postprocessors]
  [average_u_aux]
    type = SideAverageValue
    variable = 'u_aux'
    #execute_on = 'TIMESTEP_BEGIN'
    boundary = 'right'
  []
[]
