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
#ifndef itkThresholdedMedianMaskImageFilter_hxx
#define itkThresholdedMedianMaskImageFilter_hxx

#include "itkThresholdedMedianMaskImageFilter.h"

#include "itkBinaryFunctorImageFilter.h"
#include "itkMath.h"
#include "itkNumericTraits.h"
#include "itkThresholdedMedianMaskImageFilter.h"

#ifdef PENDING_REMOVAL
#include "itkImageFileWriter.h"
#endif

namespace itk
{
    namespace Functor
    {
        template< typename TPixelTypeInput, typename PixelTypeOutput >
        class ThresholdedMask
        {
        public:
            void SetThresholdLower( const double dblThresholdLower ) { m_ThresholdLower = dblThresholdLower; }
            double GetThresholdLower() const { return m_ThresholdLower; }
            void SetThresholdUpper( const double dblThresholdUpper ) { m_ThresholdUpper = dblThresholdUpper; }
            double GetThresholdUpper() const { return m_ThresholdUpper; }

            ThresholdedMask() {}
            ~ThresholdedMask() {}
            bool operator!=(const ThresholdedMask &) const
            {
                return false;
            }

            bool operator==( const ThresholdedMask & other ) const
            {
                return !( *this != other );
            }

            inline PixelTypeOutput operator()( const TPixelTypeInput & A, const TPixelTypeInput & B ) const
            {
                return ( A < m_ThresholdLower * B || A > m_ThresholdUpper * B ) ? itk::NumericTraits<PixelTypeOutput>::OneValue() : itk::NumericTraits<PixelTypeOutput>::ZeroValue();
            }
        private:
            double                      m_ThresholdLower;
            double                      m_ThresholdUpper;
        };
    }

    template< typename TInputImage, typename TOutputImage >
    ThresholdedMedianMaskImageFilter< TInputImage, TOutputImage >::ThresholdedMedianMaskImageFilter()
    {

    }

    template< typename TInputImage, typename TOutputImage >
    void ThresholdedMedianMaskImageFilter< TInputImage, TOutputImage >::PrintSelf( std::ostream& os, Indent indent ) const
    {
        Superclass::PrintSelf( os, indent );
    }

    template< typename TInputImage, typename TOutputImage >
    void ThresholdedMedianMaskImageFilter< TInputImage, TOutputImage >::GenerateData()
    {
        typedef Functor::ThresholdedMask< typename TInputImage::PixelType, typename TInputImage::PixelType > FunctorThreholdedMaskType;
        typedef BinaryFunctorImageFilter< TInputImage, TInputImage, TOutputImage, FunctorThreholdedMaskType > FunctorFilterType;
        typedef ThresholdedMedianImageFilter< TInputImage, TInputImage >  ThresholdedMedianImageFilterType;

        // First, do thresholded median filtering on the input
        typename ThresholdedMedianImageFilterType::Pointer pThresholdedMedianFilter( ThresholdedMedianImageFilterType::New() );
        pThresholdedMedianFilter->SetThresholdLower( this->GetThresholdLower() );
        pThresholdedMedianFilter->SetThresholdUpper( this->GetThresholdUpper() );
        pThresholdedMedianFilter->SetInput( this->GetInput() );
        pThresholdedMedianFilter->SetRadius( this->GetRadius() );

        typename FunctorFilterType::Pointer pFunctorFilter( FunctorFilterType::New() );

        // Set functor bounds
        FunctorThreholdedMaskType & functorThreshold( pFunctorFilter->GetFunctor() );
        functorThreshold.SetThresholdLower( this->GetThresholdLower() );
        functorThreshold.SetThresholdUpper( this->GetThresholdUpper() );

        pFunctorFilter->SetInput1( this->GetInput() );
        pFunctorFilter->SetInput2( pThresholdedMedianFilter->GetOutput() );
        pFunctorFilter->Update();

        this->GetOutput()->Graft( pFunctorFilter->GetOutput() );
    }
}

#endif // itkThresholdedMedianMaskImageFilter_hxx
