set(formulis_FOUND YES)

include(CMakeFindDependencyMacro)
find_dependency(fmt)

if(formulis_FOUND)
  include("${CMAKE_CURRENT_LIST_DIR}/formulisTargets.cmake")
endif()
