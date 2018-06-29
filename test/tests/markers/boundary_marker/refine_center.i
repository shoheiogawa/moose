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
  nl_rel_tol = 1e-03 # Large for adaptivity
[]

[Adaptivity]
  steps = 2
  marker = mark_right_near_center
  initial_steps = 0
  initial_marker = mark_right_near_center
  [Markers]
    [mark_right_near_center]
      type = BoundaryMarker
      block = 'right'
      range = 2
      mark = REFINE
      boundaries = 'center'
    []
  []
[]

[Outputs]
  exodus = true
[]

[MeshModifiers]
  [left]
    type = SubdomainBoundingBox
    block_id = 1
    force_prepare = true
    top_right = '0.5 1.0 1e-3'
    block_name = left
    bottom_left = '-1e-3 -1e-3 -1e-3'
  []
  [right]
    type = SubdomainBoundingBox
    block_id = 2
    force_prepare = true
    top_right = '1.001 1.001 1e-3'
    block_name = right
    bottom_left = '0.5 -1e-3 -1e-3'
  []
  [center_sideset]
    type = SideSetsBetweenSubdomains
    master_block = 'left'
    force_prepare = true
    depends_on = 'right left'
    new_boundary = 'center'
    paired_block = 'right'
  []
[]
