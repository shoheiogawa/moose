[Mesh]
  file = create_mesh_in.e 
[]

[AddBlockDeleterMeshModifierWithRegEx]
  block_name = "(small_.*)"
  new_boundary = true
  new_boundary_prefix = "pre"
  new_boundary_suffix = "suf"
[]

