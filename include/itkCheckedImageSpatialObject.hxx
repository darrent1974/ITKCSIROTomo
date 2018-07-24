/*=========================================================================
 *
 *  Copyright
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkCheckedImageSpatialObject_hxx
#define itkCheckedImageSpatialObject_hxx

#include "itkCheckedImageSpatialObject.h"
#include "itkSize.h"
#include "itkDefaultConvertPixelTraits.h"

namespace itk
{
/** Constructor */
template< unsigned int TDimension, typename PixelType >
CheckedImageSpatialObject< TDimension,  PixelType >::CheckedImageSpatialObject()
	: ImageSpatialObject< TDimension, PixelType >()
{
  this->SetTypeName("CheckedImageSpatialObject");
}

/** Test whether a point is inside or outside the object
 *  For computational speed purposes, it is faster if the method does not
 *  check the name of the class and the current depth */
template< unsigned int TDimension, typename PixelType >
bool CheckedImageSpatialObject< TDimension,  PixelType >::IsInside( const PointType & point ) const
{
  if( !this->GetBounds()->IsInside( point ) )
    return false;


  if( !this->SetInternalInverseTransformToWorldToIndexTransform() )
    return false;

  PointType transformedPoint( this->GetInternalInverseTransform()->TransformPoint( point ) );

  typename InterpolatorType::ContinuousIndexType index;

  for( unsigned int i = 0; i < TDimension; i++ )
	index[i] = transformedPoint[i];

  return Superclass::m_Interpolator->IsInsideBuffer( index );
}


/** Return true if the given point is inside the image */
template< unsigned int TDimension, typename PixelType >
bool
CheckedImageSpatialObject< TDimension,  PixelType >
::IsInside(const PointType & point, unsigned int depth, char *name) const
{
  if ( name == ITK_NULLPTR )
    {
    if ( IsInside(point) )
      {
      return true;
      }
    }
  else if ( strstr(typeid( Self ).name(), name) )
    {
    if ( IsInside(point) )
      {
      return true;
      }
    }

  return Superclass::IsInside(point, depth, name);
}

} // end namespace itk

#endif // itkCheckedImageSpatialObject_hxx
