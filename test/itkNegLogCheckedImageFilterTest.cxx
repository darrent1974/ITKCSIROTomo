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

#include "itkNegLogCheckedImageFilter.h"

#include "itkCommand.h"
#include "itkTestingMacros.h"
#include "itkNumericTraits.h"
#include "itkMath.h"

using ImageType = itk::Image< float, 1 >;
using NegLogCheckedImageFilterType = itk::NegLogCheckedImageFilter< ImageType >;

namespace
{
    class ShowProgress : public itk::Command
    {
    public:
        itkNewMacro( ShowProgress )

        void Execute( itk::Object* caller, const itk::EventObject& event ) override
        {
            Execute( dynamic_cast< const itk::Object* >( caller ), event );
        }

        void Execute( const itk::Object* caller, const itk::EventObject& event ) override
        {
            if ( !itk::ProgressEvent().CheckEvent( &event ) )
                return;

            const auto* pProcessObject( dynamic_cast< const itk::ProcessObject* >( caller ) );

            if ( !pProcessObject )
                return;

            std::cout << " " << pProcessObject->GetProgress();
        }
    };
}

int itkNegLogCheckedImageFilterTest( int argc, char * argv[] )
{
    if( argc < 1 )
    {
        std::cerr << "Usage: " << argv[0];
        std::cerr << std::endl;
        return EXIT_FAILURE;
    }

    NegLogCheckedImageFilterType::Pointer pNegLogCheckedImageFilter( NegLogCheckedImageFilterType::New() );

    EXERCISE_BASIC_OBJECT_METHODS( pNegLogCheckedImageFilter, NegLogCheckedImageFilter, ImageToImageFilter );

    ImageType::SizeType size;
    size[0] = 4;
    ImageType::Pointer pInputImage( ImageType::New() );
    pInputImage->SetRegions( size );
    pInputImage->Allocate();

    ImageType::IndexType indexTest;
    indexTest.Fill( 0 );

    ImageType::PixelType valTest;

    // Set 3 pixel values in input image to -1, 0, 1, 2 for -log testing
    for( valTest = static_cast< ImageType::PixelType >( -1.0 ); valTest <= static_cast< ImageType::PixelType >( 2.0 ); valTest++ )
    {
        std::cout << "Input pixel " << indexTest[0] << ", value = " << valTest << std::endl;
        pInputImage->SetPixel( indexTest, valTest );
        indexTest[0]++;
    }

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pNegLogCheckedImageFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pNegLogCheckedImageFilter->SetInput( pInputImage );

    TRY_EXPECT_NO_EXCEPTION( pNegLogCheckedImageFilter->Update() );

    // Test filtered values
    ImageType::Pointer pOutputImage( pNegLogCheckedImageFilter->GetOutput() );

    indexTest.Fill( 0 );

    std::cout << "Output pixel " << indexTest[0] << ", value = " << pOutputImage->GetPixel( indexTest ) << ", expecting 0.0" << std::endl;
    TEST_EXPECT_TRUE ( itk::Math::ExactlyEquals( pOutputImage->GetPixel( indexTest ), 0.0 ) );
    indexTest[0]++;

    std::cout << "Output pixel " << indexTest[0] << ", value = " << pOutputImage->GetPixel( indexTest ) << ", expecting 0.0" << std::endl;
    TEST_EXPECT_TRUE ( itk::Math::ExactlyEquals( pOutputImage->GetPixel( indexTest ), 0.0 ) );
    indexTest[0]++;

    std::cout << "Output pixel " << indexTest[0] << ", value = " << pOutputImage->GetPixel( indexTest ) << ", expecting " << static_cast< ImageType::PixelType >( -std::log( 1.0 ) ) << std::endl;
    TEST_EXPECT_TRUE ( itk::Math::FloatAlmostEqual( pOutputImage->GetPixel( indexTest ),  static_cast< ImageType::PixelType >( -std::log( 1.0 ) ) ) );
    indexTest[0]++;

    std::cout << "Output pixel " << indexTest[0] << ", value = " << pOutputImage->GetPixel( indexTest ) << ", expecting " << static_cast< ImageType::PixelType >( -std::log( 2.0 ) ) << std::endl;
    TEST_EXPECT_TRUE ( itk::Math::FloatAlmostEqual( pOutputImage->GetPixel( indexTest ),  static_cast< ImageType::PixelType >( -std::log( 2.0 ) ) ) );

    std::cout << "Test finished." << std::endl;

    return EXIT_SUCCESS;
}
