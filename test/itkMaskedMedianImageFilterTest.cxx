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
    if( argc < 1 )
    {
        std::cerr << "Usage: " << argv[0];
        std::cerr << std::endl;
        return EXIT_FAILURE;
    }

    const unsigned int uintDimension( 2 );
    using PixelType = float;
    using ImageType = itk::Image< PixelType, uintDimension >;

    using FilterType = itk::ThresholdedMedianImageFilter< ImageType, ImageType >;
    FilterType::Pointer pFilter( FilterType::New() );

    EXERCISE_BASIC_OBJECT_METHODS( pFilter, ThresholdedMedianImageFilter, BoxImageFilter );

    itk::ImageFileReader< ImageType >::Pointer pImageFileReader( itk::ImageFileReader< ImageType >::New() );
    pImageFileReader->SetFileName( "stitched.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage( pImageFileReader->GetOutput() );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pFilter->SetInput( pImage );
    pFilter->SetThresholdLower( 0.5 );
    pFilter->SetThresholdUpper( 1.5 );

    FilterType::RadiusType radiusFilter;
    radiusFilter.Fill( 3 );
    pFilter->SetRadius( radiusFilter );

    try
    {
        pFilter->Update();

        itk::ImageFileWriter< ImageType >::Pointer pImageFileWriter( itk::ImageFileWriter< ImageType >::New() );
        pImageFileWriter->SetInput( pFilter->GetOutput() );
        pImageFileWriter->SetFileName( "median.mhd" );
        pImageFileWriter->Update();

    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
