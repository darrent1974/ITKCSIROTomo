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
#ifndef itkThresholdedMedianMaskImageFilter_h
#define itkThresholdedMedianMaskImageFilter_h

#include "itkThresholdedMedianImageFilter.h"
#include "itkProgressReporter.h"

namespace itk
{
/** \class ThresholdedMedianMaskImageFilter
 *
 * \brief stub.
 *
 *
 * \note This class has not yet been implemented
 *
 * \ingroup ITKCSIROTomo
 */
    template< typename TInputImage, typename TOutputImage >
    class ITK_TEMPLATE_EXPORT ThresholdedMedianMaskImageFilter : public ThresholdedMedianImageFilter< TInputImage, TOutputImage >
    {
    public:
        typedef ThresholdedMedianMaskImageFilter                            Self;
        typedef ThresholdedMedianImageFilter< TInputImage, TOutputImage >   Superclass;
        typedef SmartPointer< Self >                                        Pointer;
        typedef SmartPointer< const Self >                                  ConstPointer;

        itkStaticConstMacro( ImageDimension, unsigned int, TInputImage::ImageDimension );

        itkNewMacro(Self);
        itkTypeMacro(ThresholdedMedianMaskImageFilter, ThresholdedMedianImageFilter);

#ifdef ITK_USE_CONCEPT_CHECKING
        itkConceptMacro( FloatingPointPixel, ( itk::Concept::IsFloatingPoint< typename TInputImage::PixelType > ) );
#endif
    protected:
        ThresholdedMedianMaskImageFilter();
        virtual ~ThresholdedMedianMaskImageFilter() ITK_OVERRIDE {}

        void PrintSelf( std::ostream& os, Indent indent ) const ITK_OVERRIDE;
        void GenerateData() ITK_OVERRIDE;
    private:
        ITK_DISALLOW_COPY_AND_ASSIGN(ThresholdedMedianMaskImageFilter);
    };
}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkThresholdedMedianMaskImageFilter.hxx"
#endif

#endif // itkThresholdedMedianMaskImageFilter_h
