
#include "tensorField.h"
#include "fieldBase.impl.h"

#include "tensor.h"

#include "grid.h"
#include "tetGrid.h"

namespace mpm
{

template <typename MeshT>
void AbstractTensorField< MeshT >::get_sym_tensor(const Vec3 &x, Mat3 &tensor) const
{
  tensor_view( Base::eval_at(x) ).get( tensor ) ;
}

template <typename MeshT>
void AbstractTensorField< MeshT >::add_sym_tensor(const Vec3 &x, Mat3 &tensor) const
{
  tensor_view( Base::eval_at(x) ).add( tensor ) ;
}

template class FieldBase< AbstractTensorField< Grid > > ;
template class AbstractTensorField< Grid > ;
template class FieldBase< AbstractTensorField< TetGrid > > ;
template class AbstractTensorField< TetGrid > ;

}

