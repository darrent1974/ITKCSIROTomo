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
#ifndef itkThresholdedMedianImageFilter_h
#define itkThresholdedMedianImageFilter_h

#include "itkMedianImageFilter.h"

namespace itk
{
/** \class ThresholdedMedianImageFitler
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup ITKCSIROTomo
 */

template< typename TInputImage, typename TOutputImage >
class ITK_TEMPLATE_EXPORT ThresholdedMedianImageFilter : public MedianImageFilter< TInputImage, TOutputImage >
{
public:
    /** Extract dimension from input and output image. */
    itkStaticConstMacro(InputImageDimension, unsigned int,
                        TInputImage::ImageDimension);
    itkStaticConstMacro(OutputImageDimension, unsigned int,
                        TOutputImage::ImageDimension);

    /** Convenient typedefs for simplifying declarations. */
    typedef TInputImage  InputImageType;
    typedef TOutputImage OutputImageType;

#ifdef TEMP_REMOVED
    using Self = NegLogCheckedImageFilter;
    using Superclass = ImageToImageFilter< TImage, TImage >;
    using Pointer = SmartPointer<Self>;
    using ConstPointer = SmartPointer<const Self>;
#else
    typedef ThresholdedMedianImageFilter                        Self;
    typedef MedianImageFilter< InputImageType, OutputImageType > Superclass;
    typedef SmartPointer< Self >                                  Pointer;
    typedef SmartPointer< const Self >                            ConstPointer;
#endif


    itkNewMacro(Self);
    itkTypeMacro(ThresholdedMedianImageFilter, MedianImageFilter);

    /** Image related typedefs. */
    typedef typename InputImageType::PixelType  InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;

    typedef typename InputImageType::RegionType  InputImageRegionType;
    typedef typename OutputImageType::RegionType OutputImageRegionType;

    typedef typename InputImageType::SizeType InputSizeType;

#ifdef ITK_USE_CONCEPT_CHECKING
    //itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename InputImageType::PixelType > ) );
#endif

protected:
    ThresholdedMedianImageFilter();
    virtual ~ThresholdedMedianImageFilter() ITK_OVERRIDE {}

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
    ITK_DISALLOW_COPY_AND_ASSIGN(ThresholdedMedianImageFilter);
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStitchingImageFilter.hxx"
#endif

#endif // itkThresholdedMedianImageFilter_h
