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
#ifndef itkCheckedImageSpatialObject_h
#define itkCheckedImageSpatialObject_h

#include "itkImageSpatialObject.h"

namespace itk
{
/** \class CheckedImageSpatialObject
 * \brief Implementation of an image as spatial object with extra buffer checking.
 *
 * This class combines functionnalities from a spatial object,
 * and an image.
 *
 * \sa SpatialObject CompositeSpatialObject
 * \ingroup ITKCSIROTomo
 */

    template< unsigned int TDimension = 3, typename TPixelType = unsigned char >
    class ITK_TEMPLATE_EXPORT CheckedImageSpatialObject:  public ImageSpatialObject< TDimension, TPixelType >
    {
    public:
        typedef double												ScalarType;
        typedef CheckedImageSpatialObject< TDimension, TPixelType >	Self;
        typedef ImageSpatialObject< TDimension, TPixelType >        Superclass;
        typedef SmartPointer< Self >								Pointer;
        typedef SmartPointer< const Self >							ConstPointer;

        typedef TPixelType                                                PixelType;
        typedef Image< PixelType, TDimension >                            ImageType;
        typedef typename ImageType::ConstPointer                          ImagePointer;
        typedef typename ImageType::IndexType                             IndexType;
        typedef typename ImageType::RegionType                            RegionType;
        typedef typename Superclass::TransformType                        TransformType;
        typedef typename Superclass::PointType                            PointType;
        typedef typename Superclass::BoundingBoxType                      BoundingBoxType;
        typedef InterpolateImageFunction< ImageType >                     InterpolatorType;

        typedef NearestNeighborInterpolateImageFunction< ImageType > NNInterpolatorType;

        typedef VectorContainer< IdentifierType, PointType > PointContainerType;
        typedef typename PointContainerType::Pointer         PointContainerPointer;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro(ImageSpatialObject, ImageSpatialObject);

        /** Returns true if the point is inside, false otherwise. */
        bool IsInside( const PointType & point, unsigned int depth, char *name ) const ITK_OVERRIDE;

        /** Test whether a point is inside or outside the object
        *  For computational speed purposes, it is faster if the method does not
        *  check the name of the class and the current depth */
        bool IsInside( const PointType & point ) const;
    protected:
        ITK_DISALLOW_COPY_AND_ASSIGN(CheckedImageSpatialObject);

        CheckedImageSpatialObject();
    };
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkCheckedImageSpatialObject.hxx"
#endif

#endif //itkCheckedImageSpatialObject_h
