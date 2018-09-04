[Mesh]
  type = GeneratedMesh
  block_id = '0'
  block_name = 'original'
  nx = 4
  ny = 4
  dim = 2
  elem_type = TRI3
[]

[MeshModifiers]
  [create_on_left]
    type = SideNeighborSubdomain
    sidesets = 'left'
    subdomain_names = 'left_bottom_part'
    new_subdomain_id = 1000
    new_subdomain_name = on_left
    depends_on = 'create_left_bottom_block'
  []
  [create_left_bottom_block]
    type = SubdomainBoundingBox
    block_id = 1
    top_right = '+0.5 +0.5 +0.1'
    block_name = 'left_bottom_part'
    bottom_left = '-0.1 -0.1 -0.1'
  []
[]
