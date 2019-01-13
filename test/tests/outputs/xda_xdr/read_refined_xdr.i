[Mesh]
  type = FileMesh
  file = xda_xdr_out_0000_mesh.xdr
  #file = xda_xdr_out.e
  uniform_refine = 1
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
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  xda = true
  xdr = true
  exodus = true
[]

[Adaptivity]
  initial_steps = 1
  cycles_per_step = 0
  initial_marker = coarsen_inside
  [Markers]
    [coarsen_inside]
      type = BoxMarker
      inside = COARSEN
      top_right = '0.75 0.75 0.75'
      outside = DONT_MARK
      bottom_left = '0.25 0.25 0.25'
    []
  []
[]
