[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 8
    ny = 8
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
  nl_rel_tol = 1e-4
  l_tol = 1e-2
[]

[Adaptivity]
  steps = 2
  marker = left_top_neighbor
  [Markers]
    [left_top_neighbor]
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
