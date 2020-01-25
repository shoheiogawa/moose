[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 5
    ny = 5
    nz = 5
  []
[]

[Variables]
  [u]
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = u
  []
[]

[Functions]
  [x_func]
    type = ParsedFunction
    value = x
  []
[]

[Materials]
  [x_mat]
    type = GenericFunctionMaterial
    prop_names = x_mat
    prop_values = x_func
  []
[]

[Executioner]
  type = Steady
[]

[Outputs]
  csv = true
  execute_on = TIMESTEP_END
[]

[VectorPostprocessors]
  [cell_element_average_x]
    type = ElementGridSampler
    grid_origin = '0.0 0.0 0.0'
    variables = u
    mat_props = x_mat
    grid_length = '1.0 1.0 1.0'
    num_cells = '5 1 1'
    calc_type = average
  []
  [cell_element_average_yz]
    type = ElementGridSampler
    grid_origin = '0.0 0.0 0.0'
    variables = u
    mat_props = x_mat
    grid_length = '0.2 1.0 1.0'
    num_cells = '1 5 5'
    calc_type = average
  []
  [cell_element_average_x_empty]
    type = ElementGridSampler
    grid_origin = '1.0 0.0 0.0'
    variables = u
    mat_props = x_mat
    grid_length = '1.0 1.0 1.0'
    num_cells = '5 1 1'
    calc_type = average
    empty_cell_value = -10.0
  []
  [cell_element_integral_x]
    type = ElementGridSampler
    grid_origin = '0.0 0.0 0.0'
    variables = u
    mat_props = x_mat
    grid_length = '1.0 1.0 1.0'
    num_cells = '5 1 1'
    calc_type = integral
  []
[]

[BCs]
  [left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0.0
  []
  [right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1.0
  []
[]
