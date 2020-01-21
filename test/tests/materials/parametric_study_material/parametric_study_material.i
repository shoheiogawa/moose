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
  [mat_diffusion]
    type = MatDiffusion
    variable = u
    diffusivity = param_mat
  []
[]

[Materials]
  [parametric_mat]
    type = ParametricStudyMaterial
    range_func = '1.0 2.0 0.2'
    value_list = '0.8 1.2 1.3'
    mat_prop = param_mat
    outputs = exodus
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
    type = FunctionNeumannBC
    variable = u
    function = 1.0
    boundary = 'right'
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
  [average_param_mat]
    type = ElementIntegralMaterialProperty
    mat_prop = param_mat
  []
[]
