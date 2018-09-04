[Mesh]
  type = GeneratedMesh
  nx = 4
  ny = 4
  dim = 2
  elem_type = 'TRI3'
  block_id = 0
  block_name = 'original'
[]

[MeshModifiers]
  [create_on_left]
    type = SideNeighborSubdomain
    sidesets = 'left'
    new_subdomain_id = 1000
    new_subdomain_name = 'on_left'
  []
[]
