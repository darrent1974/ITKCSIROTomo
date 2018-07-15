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
#ifndef itkNegLogCheckedImageFilter_hxx
#define itkNegLogCheckedImageFilter_hxx

#include "itkNegLogCheckedImageFilter.h"

#include "itkUnaryFunctorImageFilter.h"
#include "itkMath.h"
#include "itkNumericTraits.h"


namespace itk
{

    namespace Functor
    {
        template<typename TInput, typename TOutput>
        class NegLogChecked
        {
        public:
            NegLogChecked() {}
            ~NegLogChecked() {}
            bool operator!=(const NegLogChecked &) const
            {
                return false;
            }

            bool operator==(const NegLogChecked & other) const
            {
                return !( *this != other );
            }

            inline TOutput operator()( const TInput & A ) const
            {
                if( itk::NumericTraits<TInput>::IsNonpositive( A ) )
                    return itk::NumericTraits<TOutput>::ZeroValue();
                else
                    return static_cast<TOutput>( -std::log( static_cast<double>( A ) ) );
            }
        };
    }

    template< typename TImage > 
    NegLogCheckedImageFilter< TImage >::NegLogCheckedImageFilter()
    {

    }

    template< typename TImage > 
    void NegLogCheckedImageFilter< TImage >::PrintSelf( std::ostream& os, Indent indent ) const
    {
        Superclass::PrintSelf( os, indent );
    }

    template< typename TImage >
    void NegLogCheckedImageFilter< TImage >::GenerateData()
    {
    #ifdef TEMP_REMOVED
        using FunctorFilterType = UnaryFunctorImageFilter< TImage, TImage, Functor::NegLogChecked<typename TImage::PixelType, typename TImage::PixelType > >;
    #else
        typedef UnaryFunctorImageFilter< TImage, TImage, Functor::NegLogChecked<typename TImage::PixelType, typename TImage::PixelType > > FunctorFilterType;
    #endif

        typename FunctorFilterType::Pointer pFunctorFilter( FunctorFilterType::New() );

        pFunctorFilter->SetInput( this->GetInput() );
        pFunctorFilter->Update();

        this->GetOutput()->Graft( pFunctorFilter->GetOutput() );
    }
}

#endif // itkNegLogCheckedImageFilter_hxx
