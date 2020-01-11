[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[]

[Variables]
  [u]
  []
[]

[Kernels]
  [diff]
    type = MatDiffusion
    variable = u
    diffusivity = ad_mat_diffusivity
  []
  [time]
    type = TimeDerivative
    variable = u
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  []
  [right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  []
[]

[Materials]
  [ad_mat_diffusivity]
    type = ADMatDiffusivity
    ad_mat_diffusivity = ad_mat_diffusivity
    coef = 1.0
  []
[]

[Executioner]
  type = Transient
  num_steps = 10
  dt = 0.1
  dtmin = 0.1
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  csv = true
[]

[Functions]
  [func_coef]
    type = ParsedFunction
    value = '2*t + 0.1'
  []
[]

[Postprocessors]
  [coef]
    type = RealControlParameterReporter
    parameter = 'Materials/ad_mat_diffusivity/coef'
  []
[]

[Controls]
  [func_control]
    type = RealFunctionControl
    parameter = 'Materials/ad_mat_diffusivity/coef'
    function = 'func_coef'
    execute_on = 'initial timestep_begin'
  []
[]
