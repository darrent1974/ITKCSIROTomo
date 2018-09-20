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

#include "itkThresholdedMedianImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

#define THRESHOLD_LOWER 0.0
#define THRESHOLD_UPPER 100.0
#define FILTER_RADIUS 2

using ImageType = itk::Image< float, 2 >;
using ThresholdedMedianImageFilterType = itk::ThresholdedMedianImageFilter< ImageType, ImageType >;
using ImageFileReaderType = itk::ImageFileReader< ImageType >;
using ImageFileWriterType = itk::ImageFileWriter< ImageType >;

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

int itkThresholdedMedianImageFilterTest( int argc, char * argv[] )
{
    if( argc < 3 )
    {
        std::cerr << "Missing parameters." << std::endl;
        std::cerr << "Usage: " << argv[0] << " inputImageFile outputImageFile" << std::endl;
        return EXIT_FAILURE;
    }

    // Setup reader for input file
    ImageFileReaderType::Pointer pImageFileReader( ImageFileReaderType::New() );
    pImageFileReader->SetFileName( argv[1] );

    // Create the filter
    ThresholdedMedianImageFilterType::Pointer pThresholdedMedianImageFilter( ThresholdedMedianImageFilterType::New() );
    EXERCISE_BASIC_OBJECT_METHODS( pThresholdedMedianImageFilter, ThresholdedMedianImageFilter, BoxImageFilter );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pThresholdedMedianImageFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pThresholdedMedianImageFilter->SetInput( pImageFileReader->GetOutput() );

    pThresholdedMedianImageFilter->SetThresholdLower( THRESHOLD_LOWER );
    TEST_SET_GET_VALUE( THRESHOLD_LOWER, pThresholdedMedianImageFilter->GetThresholdLower() );

    pThresholdedMedianImageFilter->SetThresholdUpper( THRESHOLD_UPPER );
    TEST_SET_GET_VALUE( THRESHOLD_UPPER, pThresholdedMedianImageFilter->GetThresholdUpper() );

    ThresholdedMedianImageFilterType::RadiusType radiusFilter;
    radiusFilter.Fill( FILTER_RADIUS );
    pThresholdedMedianImageFilter->SetRadius( radiusFilter );
    TEST_SET_GET_VALUE( radiusFilter, pThresholdedMedianImageFilter->GetRadius() );

    // Setup writer for output file
    ImageFileWriterType::Pointer pImageFileWriter( ImageFileWriterType::New() );
    pImageFileWriter->SetFileName( argv[2] );
    pImageFileWriter->SetInput( pThresholdedMedianImageFilter->GetOutput() );
    pImageFileWriter->Update();

    TRY_EXPECT_NO_EXCEPTION( pImageFileWriter->Update() );

    std::cout << "Test finished." << std::endl;

    return EXIT_SUCCESS;
}
