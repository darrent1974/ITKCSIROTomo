# the top-level README is used for describing this module, just
# re-used it for documentation here
get_filename_component(MY_CURRENT_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(READ "${MY_CURRENT_DIR}/README.rst" DOCUMENTATION)

itk_module(ITKCSIROTomo
  DEPENDS
    ITKCommon
	ITKSmoothing
	ITKSpatialObjects
	ITKIOImageBase
  TEST_DEPENDS
    ITKTestKernel
	ITKTransform
  DESCRIPTION
    "${DOCUMENTATION}"
  EXCLUDE_FROM_DEFAULT
  ENABLE_SHARED
)
