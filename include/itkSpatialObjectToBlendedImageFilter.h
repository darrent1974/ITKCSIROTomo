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
#ifndef itkSpatialObjectToBlendedImageFilter_h
#define itkSpatialObjectToBlendedImageFilter_h

#include "itkSpatialObjectToImageFilter.h"
#include "itkConceptChecking.h"

namespace itk
{
/** \class SpatialObjectToBlendedImageFilter
*
* \brief stub.
*
*
* \note This class has not yet been implemented
*
* \ingroup ITKCSIROTomo
*/
template< typename TInputSpatialObject, typename TOutputImage >
class ITK_TEMPLATE_EXPORT SpatialObjectToBlendedImageFilter : public SpatialObjectToImageFilter< TInputSpatialObject, TOutputImage >
{
public:
    /** Standard class typedefs. */
    typedef SpatialObjectToBlendedImageFilter                               Self;
    typedef SpatialObjectToImageFilter< TInputSpatialObject, TOutputImage > Superclass;
    typedef SmartPointer< Self >                                            Pointer;
    typedef SmartPointer< const Self >                                      ConstPointer;

    typedef TOutputImage                                                    OutputImageType;
    typedef typename OutputImageType::SizeType                              SizeType;
    typedef typename OutputImageType::PointType                             PointType;
    typedef typename OutputImageType::Pointer                               OutputImagePointer;
    typedef typename OutputImageType::ValueType                             ValueType;
    typedef typename OutputImageType::SpacingType                           SpacingType;
    typedef typename OutputImageType::DirectionType                         DirectionType;

    /** Method for creation through the object factory. */
    itkNewMacro(Self);

    /** Run-time type information (and related methods). */
    itkTypeMacro(SpatialObjectToBlendedImageFilter, SpatialObjectToImageFilter);

    /** Superclass typedefs. */
    typedef typename Superclass::OutputImageRegionType                      OutputImageRegionType;

    /** Some convenient typedefs. */
    typedef TInputSpatialObject                                             InputSpatialObjectType;
    typedef typename InputSpatialObjectType::Pointer                        InputSpatialObjectPointer;
    typedef typename InputSpatialObjectType::ConstPointer                   InputSpatialObjectConstPointer;
    typedef typename TInputSpatialObject::ChildrenListType                  ChildrenListType;

    /** ImageDimension constants */
    itkStaticConstMacro( ObjectDimension, unsigned int,	InputSpatialObjectType::ObjectDimension );
    itkStaticConstMacro( OutputImageDimension, unsigned int, TOutputImage::ImageDimension );
protected:
    SpatialObjectToBlendedImageFilter();
    ~SpatialObjectToBlendedImageFilter() ITK_OVERRIDE {}

    virtual void GenerateData() ITK_OVERRIDE;

private:
    ITK_DISALLOW_COPY_AND_ASSIGN(SpatialObjectToBlendedImageFilter);
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpatialObjectToBlendedImageFilter.hxx"
#endif

#endif // itkSpatialObjectToBlendedImageFilter_h
