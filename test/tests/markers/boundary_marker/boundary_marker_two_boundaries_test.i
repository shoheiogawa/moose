[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  nz = 0
  zmax = 0
  elem_type = QUAD4
[]

[Variables]
  [u]
    order = FIRST
    family = LAGRANGE
  []
[]

[Kernels]
  [diff]
    type = Diffusion
    variable = u
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

[Executioner]
  type = Steady
  solve_type = PJFNK
[]

[Adaptivity]
  initial_steps = 2
  initial_marker = left_top_boundary_marker
  [Markers]
    [left_top_boundary_marker]
      type = BoundaryMarker
      distance = 0
      mark = REFINE
      boundaries = 'left top'
    []
  []
[]

[Outputs]
  exodus = true
[]
