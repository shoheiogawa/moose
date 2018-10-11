[Mesh]
  type = GeneratedMesh
  block_id = '0'
  block_name = 'big'
  nx = 10
  ny = 10
  dim = 2
[]

[MeshModifiers]
  [create_small_1]
    type = SubdomainBoundingBox
    block_id = 11
    top_right = '0.201 0.201 0.0'
    block_name = small_1_1
    bottom_left = '0.099 0.099 0.0'
  []
  [create_small_2]
    type = SubdomainBoundingBox
    block_id = 12
    top_right = '0.901 0.201 0.0'
    block_name = small_1_2
    bottom_left = '0.799 0.099 0.0'
  []
  [create_small_3]
    type = SubdomainBoundingBox
    block_id = 13
    top_right = '0.201 0.901 0.0'
    block_name = small_2_1
    bottom_left = '0.099 0.799 0.0'
  []
[]
