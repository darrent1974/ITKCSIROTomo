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
#ifndef itkStitchingImageFilter_h
#define itkStitchingImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"

namespace itk
{
/** \class StitchingImageFilter
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup ITKCSIROTomo
 */

template< typename TImage >
class ITK_TEMPLATE_EXPORT StitchingImageFilter : public ImageToImageFilter< TImage, TImage >
{
public:
#ifdef TEMP_REMOVED
    using Self = NegLogCheckedImageFilter;
    using Superclass = ImageToImageFilter< TImage, TImage >;
    using Pointer = SmartPointer<Self>;
    using ConstPointer = SmartPointer<const Self>;
#else
    typedef StitchingImageFilter                        Self;
    typedef ImageToImageFilter< TImage, TImage >        Superclass;
    typedef SmartPointer< Self >                        Pointer;
    typedef SmartPointer< const Self >                  ConstPointer;
#endif

    itkStaticConstMacro( ImageDimension, unsigned int, TImage::ImageDimension );

    itkNewMacro(Self);
    itkTypeMacro(StitchingImageFilter, ImageToImageFilter);

    /** Image related typedefs. */
    typedef typename TImage::Pointer    ImagePointer;
    typedef const typename TImage::Pointer    ImageConstPointer;
    typedef typename TImage::RegionType RegionType;
    typedef typename TImage::SizeType   SizeType;
    typedef typename TImage::IndexType  IndexType;
    typedef typename TImage::PixelType  PixelType;
    typedef typename TImage::PointType  PointType;
    typedef typename NumericTraits< PixelType >::RealType RealType;

#ifdef ITK_USE_CONCEPT_CHECKING
    itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename TImage::PixelType > ) );
#endif

    // overlap in physical coordinates
    itkSetMacro( Overlap, PointType );
    itkGetConstMacro( Overlap, PointType );

protected:
    StitchingImageFilter();
    virtual ~StitchingImageFilter() {}

    void PrintSelf( std::ostream& os, Indent indent ) const ITK_OVERRIDE;
    virtual void GenerateData() ITK_OVERRIDE;

    /**
     * StitchingImageFilter produces an image which is a different resolution
     * than its input image.  As such, StitchingImageFilter needs to provide an
     * implementation for GenerateOutputInformation() in order to inform
     * the pipeline execution model.  The original documentation of this
     * method is below.
     *
     * \sa ProcessObject::GenerateOutputInformaton()
     */
    virtual void GenerateOutputInformation() ITK_OVERRIDE;

    /** StitchingImageFilter needs a smaller input requested region than
     * output requested region.  As such, PadImageFilterBase needs to
     * provide an implementation for GenerateInputRequestedRegion() in
     * order to inform the pipeline execution model.
     * \sa ProcessObject::GenerateInputRequestedRegion()  */
    virtual void GenerateInputRequestedRegion() ITK_OVERRIDE;
private:
    ITK_DISALLOW_COPY_AND_ASSIGN(StitchingImageFilter);

    PointType m_Overlap;

};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThresholdedMedianImageFilter.hxx"
#endif

#endif // itkStitchingImageFilter_h
