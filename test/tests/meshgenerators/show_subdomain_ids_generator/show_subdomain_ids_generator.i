[Mesh]
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmax = 1
    ymax = 1
    #uniform_refine = 2
  []
  [subdomain1]
    type = SubdomainBoundingBoxGenerator
    input = gmg
    bottom_left = '0.5 0.5 0'
    block_id = 1
    top_right = '0.9 0.9 0'
  []
  [subdomain5]
    type = SubdomainBoundingBoxGenerator
    input = subdomain1
    bottom_left = '0.1 0.1 0'
    block_id = 5
    top_right = '0.3 0.3 0'
  []
  [show_subdomain_ids]
    type = ShowSubdomainIDsGenerator
    input = subdomain5
  []
[]

[Outputs]
  exodus = true
[]
