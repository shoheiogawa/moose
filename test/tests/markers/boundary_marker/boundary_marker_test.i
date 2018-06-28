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
  steps = 2
  marker = left_top_boundary_marker
  [Markers]
    [left_top_boundary_marker]
      type = BoundaryMarker
      range = 2
      mark = REFINE
      boundaries = 'left top'
    []
  []
[]

[Outputs]
  exodus = true
[]

