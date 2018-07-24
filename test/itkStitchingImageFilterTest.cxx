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
#include "itkRegionOfInterestImageFilter.h"

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

    ImageType::SpacingType spacingImage;
    spacingImage.Fill( 0.1f );

    itk::ImageFileReader< ImageType >::Pointer pImageFileReader( itk::ImageFileReader< ImageType >::New() );
//#define FIXED_IMAGES
#ifdef FIXED_IMAGES
    // Create input image to avoid test dependencies.
    ImageType::SizeType size;
    size.Fill( 120 );
    ImageType::Pointer pImage1( ImageType::New() );
    pImage1->SetRegions( size );
    pImage1->SetSpacing( spacingImage );
    pImage1->Allocate();
    pImage1->FillBuffer( 100.0f );

    ImageType::Pointer pImage2( ImageType::New() );
    pImage2->SetRegions( size );
    pImage2->SetSpacing( spacingImage );
    pImage2->Allocate();
    pImage2->FillBuffer( 200.0f );

    ImageType::Pointer pImage3( ImageType::New() );
    pImage3->SetRegions( size );
    pImage3->SetSpacing( spacingImage );
    pImage3->Allocate();
    pImage3->FillBuffer( 150.0f );

    ImageType::Pointer pImage4( ImageType::New() );
    pImage4->SetRegions( size );
    pImage4->SetSpacing( spacingImage );
    pImage4->Allocate();
    pImage4->FillBuffer( 50.0f );
#else
    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_0.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage1( pImageFileReader->GetOutput() );
    pImage1->DisconnectPipeline();

    ImageType::SizeType size( pImage1->GetLargestPossibleRegion().GetSize() );

    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_1.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage2( pImageFileReader->GetOutput() );
    pImage2->DisconnectPipeline();

    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_2.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage3( pImageFileReader->GetOutput() );
    pImage3->DisconnectPipeline();

    pImageFileReader->SetFileName( "/mnt/Data/development/InsightSoftwareConsortium/ITKCSIROTomo/darkcorrectedflat_3.mhd" );
    pImageFileReader->Update();
    ImageType::Pointer pImage4( pImageFileReader->GetOutput() );
    pImage4->DisconnectPipeline();
#endif

    using FilterType = itk::StitchingImageFilter< ImageType >;
    FilterType::Pointer pFilter( FilterType::New() );

    FilterType::SpacingType spacingShift;
    spacingShift[0] = 0.0;

#ifdef FIXED_IMAGES
    spacingShift[1] = 5;
#else
    spacingShift[1] = 15.0;
#endif

    EXERCISE_BASIC_OBJECT_METHODS( pFilter, StitchingImageFilter, ImageToImageFilter );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pFilter->SetInput( 0, pImage1 );
    pFilter->SetInput( 1, pImage2 );
    pFilter->SetInput( 2, pImage3 );
    pFilter->SetInput( 3, pImage4 );
    pFilter->SetShift( spacingShift );

    // Trim each input image by 1.0mm on the top and bottom prior to stitching
    ImageType::PointType pointTrimMin;
    pointTrimMin[0] = 0.0;
#ifdef FIXED_IMAGES
    pointTrimMin[1] = 1.0;
#else
    pointTrimMin[1] = 0.2;
#endif
    pFilter->SetTrimPointMin( pointTrimMin );

    ImageType::PointType pointTrimMax;
    pointTrimMax[0] = static_cast<double>( size[0] ) * spacingImage[0];
#ifdef FIXED_IMAGES
    pointTrimMax[1] = ( static_cast<double>( size[1] ) * spacingImage[1] ) - 1.0;
#else
    pointTrimMax[1] = ( static_cast<double>( size[1] ) * spacingImage[1] ) - 1.1;
#endif
    pFilter->SetTrimPointMax( pointTrimMax );

    try
    {
        pFilter->Update();

        typedef itk::ImageFileWriter< ImageType > WriterType;
        WriterType::Pointer pWriter( WriterType::New() );
        pWriter->SetFileName( "stitched.mhd" );
        pWriter->SetInput( pFilter->GetOutput() );
        TRY_EXPECT_NO_EXCEPTION( pWriter->Update() );
    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
