itk_module_test()

set(CSIROTomoTests
  itkNegLogCheckedImageFilterTest.cxx
  itkThresholdedMedianImageFilterTest.cxx
#  itkStitchingImageFilterTest.cxx
)

message( STATUS "test libs: " ${CSIROTomo-Test_LIBRARIES} )

CreateTestDriver(CSIROTomo "${CSIROTomo-Test_LIBRARIES}" "${CSIROTomoTests}")

itk_add_test(NAME itkNegLogCheckedImageFilterTest
	COMMAND CSIROTomoTestDriver itkNegLogCheckedImageFilterTest)

itk_add_test(NAME itkThresholdedMedianImageFilterTest
	COMMAND CSIROTomoTestDriver itkThresholdedMedianImageFilterTest)

#itk_add_test(NAME itkSpatialObjectToBlendedImageFilterTest
#	COMMAND CSIROTomoTestDriver itkSpatialObjectToBlendedImageFilterTest)

#itk_add_test(NAME itkStichingImageFilterTest
#	COMMAND CSIROTomoTestDriver itkStichingImageFilterTest)

