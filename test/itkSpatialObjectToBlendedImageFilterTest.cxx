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

#include "itkImageSpatialObject.h"
#include "itkSpatialObjectToBlendedImageFilter.h"

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

int itkSpatialObjectToBlendedImageFilterTest( int argc, char * argv[] )
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
    using ImageSpatialObjectType = itk::ImageSpatialObject< uintDimension, PixelType >;


    using FilterType = itk::SpatialObjectToBlendedImageFilter< ImageSpatialObjectType, ImageType >;
    FilterType::Pointer pFilter( FilterType::New() );
    EXERCISE_BASIC_OBJECT_METHODS( pFilter, SpatialObjectToBlendedImageFilter, SpatialObjectToImageFilter );

    // Create input image to avoid test dependencies
    ImageType::SizeType size;
    size.Fill( 128 );
    ImageType::Pointer pImage( ImageType::New() );
    pImage->SetRegions( size );
    pImage->Allocate();
    pImage->FillBuffer( 0.0f );

    // Create an image spatial object and attach image
    ImageSpatialObjectType::Pointer pImageSpatialObject( ImageSpatialObjectType::New() );
    pImageSpatialObject->SetImage( pImage );

    pFilter->SetInput( pImageSpatialObject  );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pFilter->SetInput( pImageSpatialObject );

    try
    {
        pFilter->Update();
    }
    catch( itk::ExceptionObject & error )
    {
        std::cerr << "Error: " << error << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
