/*=========================================================================
 *
 *  Copyright Insight Software Consortium
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
#ifndef itkMaskedMedianImageFilter_h
#define itkMaskedMedianImageFilter_h

#include "itkBoxImageFilter.h"
#include "itkImage.h"

namespace itk
{
/** \class MaskedMedianImageFitler
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup ITKCSIROTomo
 */

    template< typename TInputImage, typename TOutputImage, typename TMaskImage >
    class ITK_TEMPLATE_EXPORT MaskedMedianImageFilter : public BoxImageFilter< TInputImage, TOutputImage >
    {
    public:
        /** Extract dimension from input and output image. */
        itkStaticConstMacro(InputImageDimension, unsigned int,
                            TInputImage::ImageDimension);
        itkStaticConstMacro(OutputImageDimension, unsigned int,
                            TOutputImage::ImageDimension);

        /** Convenient typedefs for simplifying declarations. */
        typedef TInputImage                                             InputImageType;
        typedef TOutputImage                                            OutputImageType;
        typedef TMaskImage                                              MaskImageType;

        typedef MaskedMedianImageFilter                                 Self;
        typedef BoxImageFilter< InputImageType, OutputImageType >       Superclass;
        typedef SmartPointer< Self >                                    Pointer;
        typedef SmartPointer< const Self >                              ConstPointer;

        typedef typename MaskImageType::ConstPointer                    MaskImageConstPointer;


        itkNewMacro(Self)
        itkTypeMacro(MaskedMedianImageFilter, BoxImageFilter)

        /** Image related typedefs. */
        typedef typename InputImageType::PixelType                      InputPixelType;
        typedef typename OutputImageType::PixelType                     OutputPixelType;
        typedef typename MaskImageType::PixelType                       MaskPixelType;

        typedef typename InputImageType::RegionType                     InputImageRegionType;
        typedef typename OutputImageType::RegionType                    OutputImageRegionType;
        typedef typename MaskImageType::RegionType                      MaskImageRegionType;

        typedef typename InputImageType::SizeType                       InputSizeType;

    #ifdef ITK_USE_CONCEPT_CHECKING
      // Begin concept checking
      itkConceptMacro( SameDimensionCheck,
                       ( Concept::SameDimension< InputImageDimension, OutputImageDimension > ) );
      itkConceptMacro( InputConvertibleToOutputCheck,
                       ( Concept::Convertible< InputPixelType, OutputPixelType > ) );
      itkConceptMacro( InputLessThanComparableCheck,
                       ( Concept::LessThanComparable< InputPixelType > ) );
      // End concept checking
    #endif

      itkSetMacro( MaskImage, MaskImageConstPointer )
      itkGetConstMacro( MaskImage, MaskImageConstPointer )

    protected:
        MaskedMedianImageFilter();
        virtual ~MaskedMedianImageFilter() ITK_OVERRIDE {}

        /** MedianImageFilter can be implemented as a multithreaded filter.
         * Therefore, this implementation provides a ThreadedGenerateData()
         * routine which is called for each processing thread. The output
         * image data is allocated automatically by the superclass prior to
         * calling ThreadedGenerateData().  ThreadedGenerateData can only
         * write to the portion of the output image specified by the
         * parameter "outputRegionForThread"
         *
         * \sa ImageToImageFilter::ThreadedGenerateData(),
         *     ImageToImageFilter::GenerateData() */
        void ThreadedGenerateData(const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId) ITK_OVERRIDE;


    private:
        ITK_DISALLOW_COPY_AND_ASSIGN(MaskedMedianImageFilter);

        MaskImageConstPointer m_MaskImage;
    };
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkMaskedMedianImageFilter.hxx"
#endif

#endif // itkMaskedMedianImageFilter_h
