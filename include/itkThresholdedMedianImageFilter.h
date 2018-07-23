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

#include "itkBoxImageFilter.h"
#include "itkImage.h"

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
class ITK_TEMPLATE_EXPORT ThresholdedMedianImageFilter : public BoxImageFilter< TInputImage, TOutputImage >
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
    typedef ThresholdedMedianImageFilter                            Self;
    typedef BoxImageFilter< InputImageType, OutputImageType >       Superclass;
    typedef SmartPointer< Self >                                    Pointer;
    typedef SmartPointer< const Self >                              ConstPointer;
#endif


    itkNewMacro(Self);
    itkTypeMacro(ThresholdedMedianImageFilter, BoxImageFilter);

    /** Image related typedefs. */
    typedef typename InputImageType::PixelType  InputPixelType;
    typedef typename OutputImageType::PixelType OutputPixelType;

    typedef typename InputImageType::RegionType  InputImageRegionType;
    typedef typename OutputImageType::RegionType OutputImageRegionType;

    typedef typename InputImageType::SizeType InputSizeType;

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

  itkSetMacro( ThresholdLower, double );
  itkGetConstMacro( ThresholdLower, double );
  itkSetMacro( ThresholdUpper, double );
  itkGetConstMacro( ThresholdUpper, double );
  itkSetMacro( Iterations, unsigned int );
  itkGetConstMacro( Iterations, unsigned int );

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

    double                      m_ThresholdLower;
    double                      m_ThresholdUpper;
    unsigned int                m_Iterations;
};
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThresholdedMedianImageFilter.hxx"
#endif

#endif // itkThresholdedMedianImageFilter_h
