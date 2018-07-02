[Mesh]
  type = FileMesh
  file = square.e
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
  nl_rel_tol = 1e-04 # Large for adaptivity
[]

[Adaptivity]
  initial_steps = 2
  initial_marker = left_top_side_neighbor_marker
  [Markers]
    [left_top_side_neighbor_marker]
      type = SideNeighborMarker
      depth = 2
      mark = REFINE
      sidesets = 'left top'
    []
  []
[]

[Outputs]
  exodus = true
[]

