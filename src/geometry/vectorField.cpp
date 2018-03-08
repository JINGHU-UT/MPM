
#include "vectorField.h"
#include "fieldBase.impl.h"

#include "tensor.h"

#include "grid.h"
#include "tetGrid.h"

namespace mpm
{

template <typename MeshT>
void AbstractVectorField< MeshT >::get_spi_tensor(const Vec3 &x, Mat3 &tensor) const
{
  tensor_view( Base::eval_at(x) ).get( tensor ) ;
}

template <typename MeshT>
void AbstractVectorField< MeshT >::add_spi_tensor(const Vec3 &x, Mat3 &tensor) const
{
  tensor_view( Base::eval_at(x) ).add( tensor ) ;
}

template class FieldBase< AbstractVectorField< Grid > > ;
template class AbstractVectorField< Grid > ;
template class FieldBase< AbstractVectorField< TetGrid > > ;
template class AbstractVectorField< TetGrid > ;

}

