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
#ifndef itkVerticalStitchingImageFilter_h
#define itkVerticalStitchingImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"

namespace itk
{
/** \class VerticalStitchingImageFilter
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup ITKCSIROTomo
 */

template< typename TImage >
class ITK_TEMPLATE_EXPORT VerticalStitchingImageFilter : public ImageToImageFilter< TImage, TImage >
{
public:
	typedef VerticalStitchingImageFilter                        Self;
	typedef ImageToImageFilter< TImage, TImage >				Superclass;
    typedef SmartPointer< Self >                                Pointer;
    typedef SmartPointer< const Self >                          ConstPointer;

    itkStaticConstMacro( ImageDimension, unsigned int, TImage::ImageDimension );

    itkNewMacro(Self);
	itkTypeMacro(VerticalStitchingImageFilter, ImageToImageFilter);

    /** Image related typedefs. */
    typedef typename TImage::RegionType                         OutputRegionType;
    typedef typename TImage::SizeType                           OutputSizeType;
    typedef typename TImage::IndexType                          OutputIndexType;
    typedef typename TImage::PixelType                          OutputPixelType;
    typedef typename TImage::PointType                          OutputPointType;
    typedef typename TImage::SpacingType                        OutputSpacingType;

#ifdef ITK_USE_CONCEPT_CHECKING
    itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename TImage::PixelType > ) );
#endif

    // Shift in physical coordinates
	itkSetMacro( VerticalShift, InputSpacingType );
	itkGetConstMacro( VerticalShift, InputSpacingType );

    // Trim amounts in physical coordinates
    itkSetMacro( TrimPointMin, InputPointType );
    itkGetConstMacro( TrimPointMin, InputPointType );
    itkSetMacro( TrimPointMax, InputPointType );
    itkGetConstMacro( TrimPointMax, InputPointType );

protected:
	VerticalStitchingImageFilter();
	virtual ~VerticalStitchingImageFilter() ITK_OVERRIDE {}

    void PrintSelf( std::ostream& os, Indent indent ) const ITK_OVERRIDE;
    virtual void GenerateData() ITK_OVERRIDE;

    /**
	 * VerticalStitchingImageFilter produces an image which is a different resolution
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

    OutputRegionType ComputeTrimRegion( typename TImage::ConstPointer pImage );
    typename TImage::Pointer CreateRegionCopy( typename TImage::ConstPointer pImage, typename TImage::RegionType region );
    typename TImage::ConstPointer GetScalarInput( unsigned int idxInput, unsigned int idxVector );
private:
	ITK_DISALLOW_COPY_AND_ASSIGN(VerticalStitchingImageFilter);

	InputSpacingType                                m_VerticalShift;
    InputPointType                                  m_TrimPointMin;
    InputPointType                                  m_TrimPointMax;
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVerticalStitchingImageFilter_h.hxx"
#endif

#endif // itkVerticalStitchingImageFilter_h
