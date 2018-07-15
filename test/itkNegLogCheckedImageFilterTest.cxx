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

#include "itkNegLogCheckedImageFilter.h"

#include "itkCommand.h"
#include "itkImageFileWriter.h"
#include "itkTestingMacros.h"

namespace{
class ShowProgress : public itk::Command
{
public:
  itkNewMacro( ShowProgress );

  void
  Execute( itk::Object* caller, const itk::EventObject& event ) override
  {
    Execute( (const itk::Object*)caller, event );
  }

  void
  Execute( const itk::Object* caller, const itk::EventObject& event ) override
  {
    if ( !itk::ProgressEvent().CheckEvent( &event ) )
      {
      return;
      }
    const auto* processObject = dynamic_cast< const itk::ProcessObject* >( caller );
    if ( !processObject )
      {
      return;
      }
    std::cout << " " << processObject->GetProgress();
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

  const unsigned int Dimension = 2;
  using PixelType = float;
  using ImageType = itk::Image< PixelType, Dimension >;

  using FilterType = itk::NegLogCheckedImageFilter< ImageType >;
  FilterType::Pointer filter = FilterType::New();

  EXERCISE_BASIC_OBJECT_METHODS( filter, NegLogCheckedImageFilter, ImageToImageFilter );

  // Create input image to avoid test dependencies.S
  ImageType::SizeType size;
  size.Fill( 128 );
  ImageType::Pointer image = ImageType::New();
  image->SetRegions( size );
  image->Allocate();
  image->FillBuffer( 0.0f );

  ShowProgress::Pointer showProgress = ShowProgress::New();
  filter->AddObserver( itk::ProgressEvent(), showProgress );
  filter->SetInput(image);

  try
  {
    filter->Update();
  }
  catch( itk::ExceptionObject & error )
  {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
