[Kernels]
  [coef_diff]
    type = Diffusion
    variable = u
  []
[]

[Mesh]
  type = GeneratedMesh
  nx = 10
  ny = 10
  dim = 2
[]

[BCs]
  [right_preset_bc]
    type = PresetBC
    variable = u
    boundary = 'right'
    value = 0.0
  []
  [left_neumann_bc]
    type = NeumannWithWritableAuxBC
    variable = u
    boundary = 'left'
    value = 2.0
    aux_var = 'bc_value'
  []
[]

[Executioner]
  type = Steady
[]

[Variables]
  [u]
  []
[]

[Outputs]
  exodus = true
[]

[AuxVariables]
  [bc_value]
  []
[]

[Postprocessors]
  [bc_value_average_on_left]
    type = AverageNodalVariableValue
    variable = 'bc_value'
    boundary = 'left'
  []
[]
