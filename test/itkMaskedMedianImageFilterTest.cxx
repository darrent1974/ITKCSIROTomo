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

#include "itkMaskedMedianImageFilter.h"
#include "itkCommand.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

using ImageType = itk::Image< float, 2 >;
using MaskImageType = itk::Image< unsigned char, 2 >;
using ImageFileReaderType = itk::ImageFileReader< ImageType >;
using ImageFileWriterType = itk::ImageFileWriter< ImageType >;
using MaskImageFileReaderType = itk::ImageFileReader< MaskImageType >;
using MaskedMedianImageFilterType = itk::MaskedMedianImageFilter< ImageType, ImageType, MaskImageType >;

#define FILTER_RADIUS 2

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

int itkMaskedMedianImageFilterTest( int argc, char * argv[] )
{
    if( argc < 4 )
    {
        std::cerr << "Missing parameters." << std::endl;
        std::cerr << "Usage: " << argv[0] << " inputImageFile inputMaskImageFile outputImageFile" << std::endl;
        return EXIT_FAILURE;
    }

    // Setup reader for input file
    ImageFileReaderType::Pointer pImageFileReader( ImageFileReaderType::New() );
    pImageFileReader->SetFileName( argv[1] );

    // Setup reader for input mask file
    MaskImageFileReaderType::Pointer pMaskImageFileReader( MaskImageFileReaderType::New() );
    pMaskImageFileReader->SetFileName( argv[2] );
    pMaskImageFileReader->Update();

    // Create the filter
    MaskedMedianImageFilterType::Pointer pMaskedMedianImageFilter( MaskedMedianImageFilterType::New() );
    EXERCISE_BASIC_OBJECT_METHODS( pMaskedMedianImageFilter, MaskedMedianImageFilter, BoxImageFilter );

    MaskedMedianImageFilterType::RadiusType radiusFilter;
    radiusFilter.Fill( FILTER_RADIUS );
    pMaskedMedianImageFilter->SetRadius( radiusFilter );
    TEST_SET_GET_VALUE( radiusFilter, pMaskedMedianImageFilter->GetRadius() );

    ShowProgress::Pointer pShowProgress( ShowProgress::New() );
    pMaskedMedianImageFilter->AddObserver( itk::ProgressEvent(), pShowProgress );
    pMaskedMedianImageFilter->SetInput( pImageFileReader->GetOutput() );
    pMaskedMedianImageFilter->SetMaskImage( pMaskImageFileReader->GetOutput() );

    // Setup writer for output file
    ImageFileWriterType::Pointer pImageFileWriter( ImageFileWriterType::New() );
    pImageFileWriter->SetFileName( argv[3] );
    pImageFileWriter->SetInput( pMaskedMedianImageFilter->GetOutput() );
    pImageFileWriter->Update();

    TRY_EXPECT_NO_EXCEPTION( pImageFileWriter->Update() );

    std::cout << "Test finished." << std::endl;

    return EXIT_SUCCESS;
}
