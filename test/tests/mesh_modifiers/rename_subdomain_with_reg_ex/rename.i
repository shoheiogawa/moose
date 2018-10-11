[Mesh]
  file = ./create_mesh_in.e
[]

[MeshModifiers]
  [rename_small]
    type = RenameSubdomainsWithRegEx
    input_subdomain_name = 'small_(.*)_(.*)'
    output_subdomain_name = 'small_%05d_%05d'
    num_of_replace = 2
    as_int = true
  []
[]
