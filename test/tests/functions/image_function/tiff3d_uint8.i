[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 30
  ny = 25
  nz = 20
  zmax = 20
  ymax = 25
  xmax = 30
[]

[Variables]
  [u]
  []
[]

[Functions]
  [image_func]
    type = ImageFunction
    file = tiff/tiff3d/tiff3d_uint8.tiff
    file_suffix = tiff
    dimensions = '30 25 20'
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = 'left'
    value = 0
  []
  [right]
    type = DirichletBC
    variable = u
    boundary = 'right'
    value = 1
  []
[]

[ICs]
  [u_ic]
    type = FunctionIC
    function = image_func
    variable = u
  []
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  type = Transient
  num_steps = 1
  dt = 0.1
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]

[Materials]
  [define_image_func_mat]
    type = GenericFunctionMaterial
    output_properties = 'image_func_mat'
    outputs = 'exodus'
    prop_values = 'image_func'
    prop_names = 'image_func_mat'
  []
[]
