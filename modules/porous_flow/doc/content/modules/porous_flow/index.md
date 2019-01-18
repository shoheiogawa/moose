# Porous Flow

The PorousFlow module is a library of physics for fluid and heat flow in porous
media. It is formulated in an extremely general manner, so is capable of solving
problems with an arbitrary number of phases and fluid components.

By simply adding pieces of physics together in an input file, the PorousFlow
module enables the user to model problems with any combination of fluid, heat
and geomechanics.

## Other sources of information

This documentation is based on earlier latex documentation, and we are still in the process of transferring the latex into the online format.  You may find the latex documentation and its associated PDF files at `porous_flow/doc/theory`, `porous_flow/doc/tests` and in the `porous_flow/test/tests` directories.

## Theoretical foundation

The equations governing motion of fluid and heat in porous media that are implemented
in `Kernels` in the PorousFlow module.

- [Governing equations](governing_equations.md)

## Available models

Several different flow models are available in PorousFlow.

General formulations for the following cases are possible:

- [Single phase](singlephase.md)
- [Multiphase](multiphase.md)

Specialised formulations for miscible two-phase flow are also provided, that use
a [persistent](persistent_variables.md) set of primary variables and a [compositional flash](compositional_flash.md) to calculate the partitioning
of fluid components amongst fluid phases:

- [Water and non-condensable gas](waterncg.md)
- [Brine and CO$_2$](brineco2.md)

## Material laws

Material laws implemented in PorousFlow.

- [Capillary pressure](capillary_pressure.md)
- [Relative permeability](relative_permeability.md)
- [Permeability](porous_flow/permeability.md)
- [Porosity](porosity.md)
- [Diffusivity](diffusivity.md)

## Fluid equation of states

PorousFlow uses formulations contained in the [Fluid Properties](/fluid_properties/index.md) module to calculate fluid properties
such as density or viscosity.

- [Using fluid properties](fluids.md)
- [Available fluids](/fluid_properties/index.md)

## Boundary conditions

Several boundary conditions useful for many simulations are provided.

- [Boundaries](boundaries.md)

## Point and line sources and sinks

A number of fluid and/or heat sources/sinks are available for use in PorousFlow.

- [Point and line sources and sinks](sinks.md)

## Implementation details

Details about numerical issues.

- [Mass lumping](mass_lumping.md)
- [Upwinding](upwinding.md)
- [Preconditioning and solvers](solvers.md)
- [Convergence criteria](convergence.md)
- [Nonlinear convergence problems](nonlinear_convergence_problems.md)
- [Numerical diffusion](numerical_diffusion.md)
- [A worked example of Kuzmin-Turek stabilization](kt_worked.md)

## The Dictator

The [`PorousFlowDictator`](PorousFlowDictator.md) is a `UserObject`
that holds information about the nonlinear variables used in the PorousFlow module,
as well as the number of fluid phases and fluid components in each simulation.

Other PorousFlow objects, such as `Kernels` or `Materials` query the `PorousFlowDictator`
to make sure that only valid fluid components or phases are used.

!alert note
A `PorousFlowDictator` must be present in all simulations!

## Examples

We are currently in the process of building a few key examples of PorousFlow.

- [PorousFlow tutorial](tutorial_00.md)
- [Flow through an explicitly fractured medium and flow through a fracture network](flow_through_fractured_media.md)
- [Cold CO$_{2}$ injection into an elastic reservoir --- a THM problem](co2_example.md)
- [Underground mining - an HM problem](coal_mining.md)
- [CO$_2$ storage benchmark problems](co2_intercomparison.md)

## Additional MOOSE objects

The PorousFlow module also includes a number of additional MOOSE objects to aid
users in extracting calculated values for visualising results.

### AuxKernels

The following `AuxKernels` can be used to save properties and data to `AuxVariables`,
which can then be used as input for other MOOSE objects, or saved to output files and
used to visualise results.

- [`PorousFlowDarcyVelocityComponent`](PorousFlowDarcyVelocityComponent.md):
  Calculates the Darcy velocity of the fluid

- [`PorousFlowPropertyAux`](PorousFlowPropertyAux.md):
  Extracts properties from the model.

### Postprocessors

A number of `Postprocessors` are available:

- [`PorousFlowFluidMass`](PorousFlowFluidMass.md): Calculates the mass
  of a fluid component $\kappa$
- [`PorousFlowHeatEnergy`](PorousFlowHeatEnergy.md): Calculates the heat energy
