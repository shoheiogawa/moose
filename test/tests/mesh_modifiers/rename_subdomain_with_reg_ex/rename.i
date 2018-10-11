[Mesh]
  file = ./create_mesh_in.e
[]

[MeshModifiers]
  [rename_small]
    type = RenameSubdomainsWithRegEx
    input_subdomain_name = 'small_(.*)'
    output_subdomain_name = 'small_%05d'
    as_int = true
  []
[]
