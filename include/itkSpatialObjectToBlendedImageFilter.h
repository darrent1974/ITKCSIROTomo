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
#include "itkCheckedImageSpatialObject.h"
#include "itkGroupSpatialObject.h"

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
    template< typename TOutputImage >
    class ITK_TEMPLATE_EXPORT SpatialObjectToBlendedImageFilter : public SpatialObjectToImageFilter< GroupSpatialObject< TOutputImage::ImageDimension >, TOutputImage >
    {
    public:
        itkStaticConstMacro(OutputImageDimension, unsigned int, TOutputImage::ImageDimension);

        typedef SpatialObject< TOutputImage::ImageDimension >                                                   SpatialObjectType;
        typedef GroupSpatialObject< TOutputImage::ImageDimension>                                               GroupSpatialObjectType;
        typedef CheckedImageSpatialObject< TOutputImage::ImageDimension, typename TOutputImage::PixelType >     CheckedImageSpatialObjectType;

        /** Standard class typedefs. */
        typedef SpatialObjectToBlendedImageFilter                                   Self;
        typedef SpatialObjectToImageFilter< GroupSpatialObjectType, TOutputImage >  Superclass;
        typedef SmartPointer< Self >                                                Pointer;
        typedef SmartPointer< const Self >                                          ConstPointer;

        typedef TOutputImage                                                    OutputImageType;
        typedef typename OutputImageType::SizeType                              SizeType;
        typedef typename OutputImageType::PointType                             PointType;
        typedef typename OutputImageType::Pointer                               OutputImagePointer;
        typedef typename OutputImageType::ValueType                             ValueType;
        typedef typename OutputImageType::SpacingType                           SpacingType;
        typedef typename OutputImageType::DirectionType                         DirectionType;
        typedef typename OutputImageType::RegionType                            RegionType;

        /** Method for creation through the object factory. */
        itkNewMacro(Self);

        /** Run-time type information (and related methods). */
        itkTypeMacro(SpatialObjectToBlendedImageFilter, SpatialObjectToImageFilter);

        /** Superclass typedefs. */
        typedef typename Superclass::OutputImageRegionType                      OutputImageRegionType;

        /** Some convenient typedefs. */
        typedef typename CheckedImageSpatialObjectType::Pointer                 ImageSpatialObjectPointer;
        typedef typename CheckedImageSpatialObjectType::ConstPointer            ImageSpatialObjectConstPointer;
        typedef typename GroupSpatialObjectType::ChildrenListType               ChildrenListType;
        typedef typename SpatialObjectType::Pointer                             SpatialObjectTypePointer;

    protected:
        SpatialObjectToBlendedImageFilter();
        virtual ~SpatialObjectToBlendedImageFilter() ITK_OVERRIDE {}

        virtual void AllocateOutputs() ITK_OVERRIDE;
        virtual void GenerateData() ITK_OVERRIDE;

        virtual void BeforeThreadedGenerateData( void ) ITK_OVERRIDE;

        /** SpatialObjectToBlendedImageFilter can be implemented as a multithreaded filter.
         * Therefore, this implementation provides a ThreadedGenerateData()
         * routine which is called for each processing thread. The output
         * image data is allocated automatically by the superclass prior to
         * calling ThreadedGenerateData().  ThreadedGenerateData can only
         * write to the portion of the output image specified by the
         * parameter "outputRegionForThread"
         *
         * \sa ImageToImageFilter::ThreadedGenerateData(),
         *     ImageToImageFilter::GenerateData() */
        void ThreadedGenerateData( const RegionType & outputRegionForThread, ThreadIdType threadId ) ITK_OVERRIDE;

    private:
        ITK_DISALLOW_COPY_AND_ASSIGN(SpatialObjectToBlendedImageFilter);

        double m_ImageMin;
        double m_ImageMax;
    };
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSpatialObjectToBlendedImageFilter.hxx"
#endif

#endif // itkSpatialObjectToBlendedImageFilter_h
