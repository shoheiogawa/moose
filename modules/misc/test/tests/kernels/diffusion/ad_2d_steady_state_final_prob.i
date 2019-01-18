# This test solves a 2D steady state heat equation
# The error is found by comparing to the analytical solution

# Note that the thermal conductivity, specific heat, and density in this problem
# Are set to 1, and need to be changed to the constants of the material being
# Analyzed

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 30
  ny = 30
  xmax = 2
  ymax = 2
[]

[Variables]
  [./T]
  [../]
[]

[ADKernels]
  [./HeatDiff]
    type = ADMatDiffusion
    variable = T
    diffusivity = diffusivity
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = T
    boundary = left
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = T
    boundary = right
    value = 0
  [../]
  [./bottom]
    type = DirichletBC
    variable = T
    boundary = bottom
    value = 0
  [../]
  [./top]
    type = FunctionDirichletBC
    variable = T
    boundary = top
    function = '10*sin(pi*x*0.5)'
  [../]
[]

[Materials]
  [./k]
    type = GenericConstantMaterial
    prop_names = diffusivity
    prop_values = 1
  [../]
[]


[Postprocessors]
  [./nodal_error]
    type = NodalL2Error
    function = '10/(sinh(pi))*sin(pi*x*0.5)*sinh(pi*y*0.5)'
    variable = T
  [../]
  [./elemental_error]
    type = ElementL2Error
    function = '10/(sinh(pi))*sin(pi*x*0.5)*sinh(pi*y*0.5)'
    variable = T
  [../]
[]

[Preconditioning]
  [./full]
    type = SMP
    full = true
  [../]
[]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
[]
