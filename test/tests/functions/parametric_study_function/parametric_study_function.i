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

[Kernels]
  [diffusion]
    type = Diffusion
    variable = u
  []
[]

[Functions]
  [parametric_study_func]
    type = ParametricStudyFunction
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
    type = FunctionDirichletBC
    variable = u
    boundary = 'right'
    function = parametric_study_func
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
  [average_func]
    type = SideAverageValue
    variable = 'u'
    #execute_on = 'TIMESTEP_BEGIN'
    boundary = 'right'
  []
[]
