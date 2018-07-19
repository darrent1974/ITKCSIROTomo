itk_module_test()

set(CSIROTomoTests
  itkNegLogCheckedImageFilterTest.cxx
  itkThresholdedMedianImageFilterTest.cxx
  itkSpatialObjectToBlendedImageFilterTest.cxx
  itkStitchingImageFilterTest.cxx )

CreateTestDriver(CSIROTomo "${CSIROTomo-Test_LIBRARIES}" "${CSIROTomoTests}")

itk_add_test(NAME itkNegLogCheckedImageFilterTest
	COMMAND CSIROTomoTestDriver itkNegLogCheckedImageFilterTest)

itk_add_test(NAME itkThresholdedMedianImageFilterTest
	COMMAND CSIROTomoTestDriver itkThresholdedMedianImageFilterTest)

itk_add_test(NAME itkSpatialObjectToBlendedImageFilterTest
	COMMAND CSIROTomoTestDriver itkSpatialObjectToBlendedImageFilterTest)

itk_add_test(NAME itkStitchingImageFilterTest
	COMMAND CSIROTomoTestDriver
	itkStitchingImageFilterTest
	DATA{Input/StitchImage1.mhd}
	DATA{Input/StitchImage1.raw}
	DATA{Input/StitchImage2.mhd}
	DATA{Input/StitchImage2.raw}
	${ITK_TEST_OUTPUT_DIR}/StitchResult.mhd)

