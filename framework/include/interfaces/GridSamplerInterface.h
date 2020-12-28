//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "MooseTypes.h"
#include "ExecFlagEnum.h"
#include "MooseEnum.h"
#include "InputParameters.h"
#include "SubProblem.h"
#include "Transient.h"

// Forward declarations
template <typename T>
class GridSamplerInterface;

template <typename T>
class GridSamplerInterface
  : public T
{
public:
  GridSamplerInterface(const InputParameters & parameters);

  static InputParameters validParams();

protected:
  virtual void initialize();
  virtual void execute();
  virtual void finalize();
  virtual void threadJoin(const UserObject & y);

  using T::coupledComponents;
  using T::declareVector;
  using T::_qrule;
  using T::_q_point;
  using T::_JxW;
  using T::_coord;

  /// Override this function to define the way to compute volume
  virtual Real computeQpVolume() const;

  /// Quantity to compute: average or summation
  const std::string _calc_type;

  /// The number of cells in each direction
  std::vector<unsigned int> _num_cells;

  /// The coordinate of the grid's origin
  std::vector<Real> _grid_origin;

  /// The length of the whole grid.
  std::vector<Real> _grid_length;

  /// The number of total cells
  const unsigned int _total_num_cells;

  /// The length of each cell in each direction,
  /// i.e. _grid_length[i] / _num_cells[i]
  std::vector<Real> _cell_length;

  /// The number of variables measured
  const unsigned int _num_variables;

  /// The variables to be measured
  std::vector<const VariableValue *> _variables;

  /// The material properties measured
  std::vector<std::string> _mat_prop_names;
  std::vector<const MaterialProperty<Real> *> _mat_props;

  /// The number of material properties measured
  const unsigned int num_mat_props;

  /// Current quadrature point used in computeQpVolume()
  unsigned int _qp;

  /// Value to assign to empty cells
  const Real _empty_cell_value;

  /// Cell IDs
  VectorPostprocessorValue & _cell_id;

  /// The cell index in x-direction
  VectorPostprocessorValue & _cell_index_x;

  /// The cell index in y-direction
  VectorPostprocessorValue & _cell_index_y;

  /// The cell index in z-direction
  VectorPostprocessorValue & _cell_index_z;

  /// The x-coordinate of the center of cells
  VectorPostprocessorValue & _cell_center_x;

  /// The y-coordinate of the center of cells
  VectorPostprocessorValue & _cell_center_y;

  /// The z-coordinate of the center of cells
  VectorPostprocessorValue & _cell_center_z;

  /// The cell length in x-direction
  VectorPostprocessorValue & _cell_length_x;

  /// The cell length in y-direction
  VectorPostprocessorValue & _cell_length_y;

  /// The cell length in z-direction
  VectorPostprocessorValue & _cell_length_z;

  /// The volume of each cell
  VectorPostprocessorValue & _cell_volume;

  /// The volume of subdomain (or side set) of interest in each cell
  VectorPostprocessorValue & _volume;

  /// Aggregated values for variables
  std::vector<VectorPostprocessorValue *> _sum_variables;

  /// Aggregated values for material properties
  std::vector<VectorPostprocessorValue *> _sum_mat_props;

  /// The number of QP points of each cell
  /// (the values in this vector is Real.)
  VectorPostprocessorValue & _qp_count;
};

template <typename T>
InputParameters
GridSamplerInterface<T>::validParams()
{
  InputParameters params = emptyInputParameters();
  params.addClassDescription(
      "This template class provides a way to compute cell average "
      "or integral on a grid");
  params.addCoupledVar("variables",
      "Variables to be averaged (or integrated) in each cell on a grid. "
      "Multiple names are accepted.");
  params.addParam<std::vector<std::string>>("mat_props",
      "Real material properties to be averaged (or integrated) in each cell on "
      "a grid. Multiple names are accepted.");
  params.addRequiredParam<std::vector<unsigned int>>(
      "num_cells", "The number of cells in each direction");
  params.addRequiredParam<std::vector<Real>>("grid_origin",
      "The coordinate of the point at left, bottom, and front,"
      " i.e., left, bottom, and front coordinate of the first cell");
  params.addRequiredParam<std::vector<Real>>("grid_length",
      "The length of grid (all cells combined) in each direction");
  params.addParam<Real>("empty_cell_value", -1.0,
      "Value to assign to cells with no element falling in");
  MooseEnum calc_types("average integral");
  params.addParam<MooseEnum>("calc_type", calc_types,
      "Quantity computed by this vector post processor: average or integral");
  return params;
}

template <typename T>
GridSamplerInterface<T>::GridSamplerInterface(const InputParameters & parameters)
  : T(parameters),
    _calc_type(T::template getParam<MooseEnum>("calc_type")),
    _num_cells(T::template getParam<std::vector<unsigned int>>("num_cells")),
    _grid_origin(T::template getParam<std::vector<Real>>("grid_origin")),
    _grid_length(T::template getParam<std::vector<Real>>("grid_length")),
    _total_num_cells(std::accumulate(
          _num_cells.begin(), _num_cells.end(), 1, std::multiplies<int>())),
    _num_variables(this->coupledComponents("variables")),
    _variables(_num_variables),
    _mat_prop_names(T::template getParam<std::vector<std::string>>("mat_props")),
    num_mat_props(_mat_prop_names.size()),
    _empty_cell_value(T::template getParam<Real>("empty_cell_value")),
    _cell_id(declareVector("cell_id")),
    _cell_index_x(declareVector("cell_index_x")),
    _cell_index_y(declareVector("cell_index_y")),
    _cell_index_z(declareVector("cell_index_z")),
    _cell_center_x(declareVector("cell_center_x")),
    _cell_center_y(declareVector("cell_center_y")),
    _cell_center_z(declareVector("cell_center_z")),
    _cell_length_x(declareVector("cell_length_x")),
    _cell_length_y(declareVector("cell_length_y")),
    _cell_length_z(declareVector("cell_length_z")),
    _cell_volume(declareVector("cell_volume")),
    _volume(declareVector("volume")),
    _sum_variables(_num_variables),
    _sum_mat_props(num_mat_props),
    _qp_count(declareVector("qp_count"))
{
  if (!(_calc_type == "average" || _calc_type == "integral"))
    mooseError("The sampling scheme given is not supported. "
               "Choose one from 'average' or 'integral'.");

  _cell_length.resize(_num_cells.size());
  for (auto i = beginIndex(_cell_length); i < _cell_length.size(); ++i)
  {
    _cell_length[i] = _grid_length[i] / Real(_num_cells[i]);
  }

  // Correct the variables and prepare vectors
  for (auto i = beginIndex(_sum_variables); i < _num_variables; ++i)
  {
    _variables[i] = &this->coupledValue("variables", i);
    _sum_variables[i] = &declareVector(this->getVar("variables", i)->name());
  }

  // Collect the material properties to _mat_props
  // and prepare vectors to store the material property average (integral) values
  _mat_props.resize(num_mat_props);
  for (auto i = beginIndex(_sum_mat_props); i < num_mat_props; ++i)
  {
    _mat_props[i] = &T::template getMaterialProperty<Real>(_mat_prop_names[i]);
    _sum_mat_props[i] = &declareVector(_mat_prop_names[i]);
  }

  _cell_index_x.resize(_total_num_cells);
  _cell_index_y.resize(_total_num_cells);
  _cell_index_z.resize(_total_num_cells);
  _cell_center_x.resize(_total_num_cells);
  _cell_center_y.resize(_total_num_cells);
  _cell_center_z.resize(_total_num_cells);
  _cell_length_x.resize(_total_num_cells);
  _cell_length_y.resize(_total_num_cells);
  _cell_length_z.resize(_total_num_cells);
  _cell_volume.resize(_total_num_cells);
  _cell_id.resize(_total_num_cells);
  _qp_count.resize(_total_num_cells);

  for (unsigned int iz = 0; iz < _num_cells[2]; ++iz)
  {
    for (unsigned int iy = 0; iy < _num_cells[1]; ++iy)
    {
      for (unsigned int ix = 0; ix < _num_cells[0]; ++ix)
      {
        // Flatten 1D index of each cell
        auto cell_id =
          ix + iy * _num_cells[0] + iz * (_num_cells[0] * _num_cells[1]);
        _cell_index_x[cell_id] = ix;
        _cell_index_y[cell_id] = iy;
        _cell_index_z[cell_id] = iz;
        _cell_center_x[cell_id] = (ix + 0.5) * _cell_length[0] + _grid_origin[0];
        _cell_center_y[cell_id] = (iy + 0.5) * _cell_length[1] + _grid_origin[1];
        _cell_center_z[cell_id] = (iz + 0.5) * _cell_length[2] + _grid_origin[2];
        _cell_length_x[cell_id] = _cell_length[0];
        _cell_length_y[cell_id] = _cell_length[1];
        _cell_length_z[cell_id] = _cell_length[2];
        _cell_volume[cell_id] =
            std::accumulate(_cell_length.begin(), _cell_length.end(),
                            1.0, std::multiplies<Real>());
        _cell_id[cell_id] = cell_id;
        _qp_count[cell_id] = 0.0;
      }
    }
  }
}

template <typename T>
void
GridSamplerInterface<T>::initialize()
{
  // Reset the bin for the variables
  for (auto vec_ptr : _sum_variables)
    vec_ptr->assign(_total_num_cells, 0.0);

  // Reset the bin for the material properties
  for (auto vec_ptr : _sum_mat_props)
    vec_ptr->assign(_total_num_cells, 0.0);

  // Initialize the volume
  _volume.assign(_total_num_cells, 0.0);

  // Initialize the qp counts
  _qp_count.assign(_total_num_cells, 0.0);
}

template <typename T>
void
GridSamplerInterface<T>::execute()
{
  // loop over all the quadrature points
  for (_qp = 0; _qp < _qrule->n_points(); ++_qp)
  {
    // For each QP, we calculate the bin where the current QP falls in.
    // If it is outside of the whole grid, skip this accumulation below via "continue".
    Real qp_cell_location_x = (_q_point[_qp](0) - _grid_origin[0]) / _cell_length[0];
    if ((qp_cell_location_x < 0.0) || (Real(_num_cells[0]) <= qp_cell_location_x)) continue;
    auto qp_cell_index_x = (unsigned int)qp_cell_location_x;

    Real qp_cell_location_y = (_q_point[_qp](1) - _grid_origin[1]) / _cell_length[1];
    if ((qp_cell_location_y < 0.0) || (Real(_num_cells[1]) <= qp_cell_location_y)) continue;
    auto qp_cell_index_y = (unsigned int)qp_cell_location_y;

    Real qp_cell_location_z = (_q_point[_qp](2) - _grid_origin[2]) / _cell_length[2];
    if ((qp_cell_location_z < 0.0) || (Real(_num_cells[2]) <= qp_cell_location_z)) continue;
    auto qp_cell_index_z = (unsigned int)qp_cell_location_z;

    // Calculate an 1D-flatten cell ID
    auto qp_cell_id = qp_cell_index_x + qp_cell_index_y * _num_cells[0]
                        + qp_cell_index_z * (_num_cells[0] * _num_cells[1]);

    auto qp_volume = computeQpVolume();

    // The following lines add the volume contribution by the current QP
    // For the variables:
    for (auto i = decltype(_num_variables)(0); i < _num_variables; ++i)
    {
      (*_sum_variables[i])[qp_cell_id] += (*_variables[i])[_qp] * qp_volume;
    }

    // For the material properties:
    for (auto i = decltype(num_mat_props)(0); i < num_mat_props; ++i)
    {
      (*_sum_mat_props[i])[qp_cell_id] += (*_mat_props[i])[_qp] * qp_volume;
    }
    // Increase the volume of the bin
    _volume[qp_cell_id] += qp_volume;
    _qp_count[qp_cell_id] += 1.0; // Note: _qp_count is in Real.
  }
}

template <typename T>
void
GridSamplerInterface<T>::threadJoin(const UserObject & y)
{
  const GridSamplerInterface<T> & uo = static_cast<const GridSamplerInterface<T> &>(y);
  mooseAssert(uo._volume.size() == _volume.size(),
      "Inconsistent volume vector lengths across the threads.");

  for (auto i = beginIndex(_volume); i < _total_num_cells; ++i)
  {
    _volume[i] += uo._volume[i];
    _qp_count[i] += uo._qp_count[i];

    // For the variables:
    for (auto j = beginIndex(_sum_variables); j < _num_variables; ++j)
    {
      (*_sum_variables[j])[i] += (*uo._sum_variables[j])[i];
    }

    // For the material properties:
    for (auto j = beginIndex(_sum_mat_props); j < num_mat_props; ++j)
    {
      (*_sum_mat_props[j])[i] += (*uo._sum_mat_props[j])[i];
    }
  }
}

template <typename T>
void
GridSamplerInterface<T>::finalize()
{
  // For the last thing, we find the values for output.
  // The integrated values will be kept as it is or
  // divided by the block volume in each cell for an average.
  // If there is no quadrature point in a cell,
  // _empty_cell_value is stored for output instead.

  this->gatherSum(_volume);
  this->gatherSum(_qp_count);

  // For the variables:
  for (auto j = beginIndex(_sum_variables); j < _num_variables; ++j)
  {
    this->gatherSum(*_sum_variables[j]);

    for (auto i = beginIndex(_volume); i < _total_num_cells; ++i)
    {
      if (_calc_type == "average")
      {
        // The following code makes the integral an average.
        // The average is valid if there are more than one valid QPs in each cell.
        (*_sum_variables[j])[i] =
          _qp_count[i] > 0.5 ?
            (*_sum_variables[j])[i] / static_cast<Real>(_volume[i]) : _empty_cell_value;
      }
      else if (_calc_type == "integral")
      {
        (*_sum_variables[j])[i] =
          _qp_count[i] > 0.5 ?  (*_sum_variables[j])[i] : _empty_cell_value;
      }
    }
  }

  // For the material properties:
  for (auto j = beginIndex(_sum_mat_props); j < num_mat_props; ++j)
  {
    this->gatherSum(*_sum_mat_props[j]);

    for (auto i = beginIndex(_volume); i < _total_num_cells; ++i)
    {
      if (_calc_type == "average")
      {
        // The following code makes the integral an average.
        // The average is valid if there are more than one valid QPs in each cell.
        (*_sum_mat_props[j])[i] =
          _qp_count[i] > 0.5 ?
            (*_sum_mat_props[j])[i] / static_cast<Real>(_volume[i]) : _empty_cell_value;
      }
      else if (_calc_type == "integral")
      {
        (*_sum_mat_props[j])[i] =
          _qp_count[i] > 0.5 ?
            (*_sum_mat_props[j])[i] : _empty_cell_value;
      }
    }
  }
}

template <typename T>
Real
GridSamplerInterface<T>::computeQpVolume() const
{
  return _JxW[_qp] * _coord[_qp];
}
