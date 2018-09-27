[Mesh]
  file = create_mesh_in.e 
[]

[AddBlockDeleterMeshModifierWithRegExAction]
  block_name = "small_.*"
  new_boundary = true
  new_boundary_prefix = "pre"
  new_boundary_suffix = "suf"
[]

