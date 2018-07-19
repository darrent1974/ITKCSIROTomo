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

#include "itkStitchingImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

namespace
{
    class ShowProgress : public itk::Command
    {
    public:
        itkNewMacro( ShowProgress );

        void Execute( itk::Object* caller, const itk::EventObject& event ) override
        {
            Execute( (const itk::Object*)caller, event );
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

int itkStitchingImageFilterTest( int argc, char * argv[] )
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

    using FilterType = itk::StitchingImageFilter< ImageType >;
    FilterType::Pointer pFilter( FilterType::New() );

    EXERCISE_BASIC_OBJECT_METHODS( pFilter, StitchingImageFilter, ImageToImageFilter );

#ifdef TEMP_REMOVED
    typedef itk::ImageFileReader< ImageType > ReaderType;
    ReaderType::Pointer pReader( ReaderType::New() );
    pReader->SetFileName( argv[1] );
    pReader->Update();
    ImageType::Pointer pImage1( pReader->GetOutput() );

    pReader->SetFileName( argv[2] );
    pReader->Update();
    ImageType::Pointer pImage2( pReader->GetOutput() );
#endif

    ImageType::SpacingType spacingImage;
    spacingImage.Fill( 0.1 );

    // Create input image to avoid test dependencies.
    ImageType::SizeType size;
    size.Fill( 120 );
    ImageType::Pointer pImage1( ImageType::New() );
    pImage1->SetRegions( size );
    pImage1->SetSpacing( spacingImage );
    pImage1->Allocate();
    pImage1->FillBuffer( 0.1f );

    ImageType::Pointer pImage2( ImageType::New() );
    pImage2->SetRegions( size );
    pImage2->SetSpacing( spacingImage );
    pImage2->Allocate();
    pImage2->FillBuffer( 0.2f );

    ImageType::Pointer pImage3( ImageType::New() );
    pImage3->SetRegions( size );
    pImage3->SetSpacing( spacingImage );
    pImage3->Allocate();
    pImage3->FillBuffer( 0.3f );

    FilterType::SpacingType spacingShift;
    spacingShift[0] = 0.0;
    spacingShift[1] = 4.0;
    spacingShift[2] = 0.0;

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pFilter->SetInput( 0, pImage1 );
    pFilter->SetInput( 1, pImage2 );
    pFilter->SetInput( 2, pImage3 );
    pFilter->SetShift( spacingShift );

    try
    {
        pFilter->Update();
#ifdef TEMP_REMOVED
        typedef itk::ImageFileWriter< ImageType > WriterType;
        WriterType::Pointer pWriter( WriterType::New() );
        pWriter->SetFileName( "stiched.mhd" );
        pWriter->SetInput( pFilter->GetOutput() );
        TRY_EXPECT_NO_EXCEPTION( pWriter->Update() );
#endif
    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
