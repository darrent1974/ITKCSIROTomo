itk_module_test()

set(CSIROTomoTests
  itkNegLogCheckedImageFilterTest.cxx
  itkThresholdedMedianImageFilterTest.cxx
  itkSpatialObjectToBlendedImageFilterTest.cxx)

CreateTestDriver(CSIROTomo "${CSIROTomo-Test_LIBRARIES}" "${CSIROTomoTests}")

itk_add_test(NAME itkNegLogCheckedImageFilterTest
	COMMAND CSIROTomoTestDriver itkNegLogCheckedImageFilterTest)

itk_add_test(NAME itkThresholdedMedianImageFilterTest
	COMMAND CSIROTomoTestDriver itkThresholdedMedianImageFilterTest)

itk_add_test(NAME itkSpatialObjectToBlendedImageFilterTest
	COMMAND CSIROTomoTestDriver itkSpatialObjectToBlendedImageFilterTest)

