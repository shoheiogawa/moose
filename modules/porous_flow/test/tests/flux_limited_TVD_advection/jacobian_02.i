# Checking the Jacobian of Flux-Limited TVD Advection, using flux_limiter_type = none
[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 2
  xmin = 0
  xmax = 1
  ny = 2
  ymin = -1
  ymax = 2
  bias_y = 1.5
  nz = 2
  zmin = 1
  zmax = 2
  bias_z = 0.8
[]

[Variables]
  [./u]
  [../]
[]

[ICs]
  [./u]
    type = RandomIC
    variable = u
  [../]
[]

[Kernels]
  [./flux]
    type = FluxLimitedTVDAdvection
    variable = u
    advective_flux_calculator = fluo
  [../]
[]

[UserObjects]
  [./fluo]
    type = AdvectiveFluxCalculator
    flux_limiter_type = superbee
    u = u
    velocity = '1 -2 1.5'
  [../]
[]


[Preconditioning]
  active = smp
  [./smp]
    type = SMP
    full = true
    petsc_options = '-snes_test_display'
    petsc_options_iname = '-snes_type'
    petsc_options_value = 'test'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = Newton
  end_time = 1
  num_steps = 1
  dt = 1
[]
