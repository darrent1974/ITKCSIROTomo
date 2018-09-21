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
#include "itkVectorImage.h"

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
    template< typename TImage, typename TWeighting >
    class ITK_TEMPLATE_EXPORT VerticalStitchingImageFilter : public ImageToImageFilter< TImage, TImage >
    {
    public:
        typedef VerticalStitchingImageFilter                Self;
        typedef ImageToImageFilter< TImage, TImage >        Superclass;
        typedef SmartPointer< Self >                        Pointer;
        typedef SmartPointer< const Self >                  ConstPointer;

        itkStaticConstMacro( ImageDimension, unsigned int, TImage::ImageDimension );
        itkStaticConstMacro( WeightingImageDimension, unsigned int, TWeighting::ImageDimension );

        itkNewMacro(Self)
        itkTypeMacro(VerticalStitchingImageFilter, ImageToImageFilter)

        /** Image related typedefs. */
        typedef typename TImage::RegionType                         RegionType;
        typedef typename TImage::SizeType                           SizeType;
        typedef typename TImage::IndexType                          IndexType;
        typedef typename TImage::PixelType                          PixelType;
        typedef typename TImage::PointType                          PointType;
        typedef typename TImage::SpacingType                        SpacingType;

        typedef typename TWeighting::PixelType                      WeightingPixelType;

        typedef itk::VectorImage< PixelType, WeightingImageDimension >      WeightingImageType;
        typedef typename WeightingImageType::Pointer                        WeightingImageTypePointer;

        itkSetMacro( ComputeWeighting, bool )
        itkGetConstMacro( ComputeWeighting, bool )
        virtual void ComputeWeightingOn() { m_ComputeWeighting = true; }
        virtual void ComputeWeightingOff() { m_ComputeWeighting = false; }

        itkSetMacro( Rescale, bool )
        itkGetConstMacro( Rescale, bool )
        virtual void RescaleOn() { m_Rescale = true; }
        virtual void RescaleOff() { m_Rescale = false; }

        // Shift in physical coordinates
        itkSetMacro( VerticalShift, double )
        itkGetConstMacro( VerticalShift, double )

        // Trim amounts in physical coordinates
        itkSetMacro( TrimPointMin, PointType )
        itkGetConstMacro( TrimPointMin, PointType )
        itkSetMacro( TrimPointMax, PointType )
        itkGetConstMacro( TrimPointMax, PointType )

        // Weighting vector images
        itkSetMacro( WeightingAlpha, WeightingImageTypePointer )
        itkGetConstMacro( WeightingAlpha, WeightingImageTypePointer )
        itkSetMacro( WeightingBeta, WeightingImageTypePointer )
        itkGetConstMacro( WeightingBeta, WeightingImageTypePointer )

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

        RegionType ComputeTrimRegion( typename TImage::ConstPointer pImage );
        typename TImage::Pointer CreateRegionCopy( typename TImage::ConstPointer pImage, typename TImage::RegionType region );

        virtual void CreateWeightingVectorImages( std::vector<typename TImage::Pointer> & vecImages );

    private:
        ITK_DISALLOW_COPY_AND_ASSIGN(VerticalStitchingImageFilter);

        bool                                       m_ComputeWeighting;
        bool                                       m_Rescale;

        RegionType                                 m_RegionNonOverlap;
        RegionType                                 m_RegionOverlapLower;
        RegionType                                 m_RegionOverlapUpper;
        RegionType                                 m_RegionWeighting;

        double                                     m_VerticalShift;
        PointType                                  m_TrimPointMin;
        PointType                                  m_TrimPointMax;
        WeightingImageTypePointer                  m_WeightingAlpha;
        WeightingImageTypePointer                  m_WeightingBeta;

        unsigned int                               m_VerticalShiftPixels;
    };
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVerticalStitchingImageFilter.hxx"
#endif

#endif // itkVerticalStitchingImageFilter_h
