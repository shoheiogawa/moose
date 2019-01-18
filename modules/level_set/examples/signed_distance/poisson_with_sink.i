[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 16
  ny = 16
[]

[MeshModifiers]
  [define_inside]
    type = SubdomainBoundingBox
    bottom_left = '0.24999999 0.24999999 0'
    block_id = 1
    block_name = inside
    top_right = '0.75000001 0.75000001 0'
    depends_on = 'define_outside'
  []
  [define_zero]
    type = SideSetsBetweenSubdomains
    master_block = 'inside'
    paired_block = 'outside'
    new_boundary = 'zero'
    depends_on = 'define_inside'
  []
  [define_outside]
    type = RenameBlock
    old_block_id = '0'
    new_block_name = 'outside'
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
  [source]
    type = BodyForce
    variable = u
  []
[]

[BCs]
  [zero_on_zero]
    type = DirichletBC
    variable = u
    boundary = 'zero'
    value = 0
  []
[]

[Executioner]
  type = Steady
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]

