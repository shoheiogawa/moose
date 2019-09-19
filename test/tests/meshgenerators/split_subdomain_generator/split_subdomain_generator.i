[Mesh]
  #inactive = 'split_subdomain_id_1_2'
  [gmg]
    type = GeneratedMeshGenerator
    dim = 2
    nx = 10
    ny = 10
    xmax = 1
    ymax = 1
    #uniform_refine = 2
  []
  [rename]
    type = RenameBlockGenerator
    input = gmg
    old_block_id = 0
    new_block_name = 'rest'
  []
  [subdomain_bottom_left]
    type = SubdomainBoundingBoxGenerator
    input = rename
    bottom_left = '0 0 0'
    block_id = 1
    top_right = '0.3 0.3 0'
    location = INSIDE
  []
  [subdomain_top_right]
    type = SubdomainBoundingBoxGenerator
    input = subdomain_bottom_left
    bottom_left = '0.7 0.7 0'
    block_id = 1
    top_right = '1.0 1.0 0'
    location = INSIDE
  []
  [subdomain_middle]
    type = SubdomainBoundingBoxGenerator
    input = subdomain_top_right
    bottom_left = '0.2 0.4 0'
    block_id = 2
    top_right = '0.5 0.9 0'
    location = INSIDE
  []
  [split_subdomain_id_1_2]
    type = SplitSubdomainGenerator
    input = subdomain_middle
    subdomain_ids = '1 2'
    new_subdomain_id_start = 10
    new_name_id_start = 0
    prefix = 'part_'
    suffix = '_subdomain'
    num_digits = 3
  []
[]

[Outputs]
  exodus = true
[]
