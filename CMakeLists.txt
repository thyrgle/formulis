cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    formulis
    VERSION 0.1.0
    DESCRIPTION "Create event listeners with formulae. Listen for when formulae change values."
    HOMEPAGE_URL "https://thyrgle.github.io/formulis/"
    LANGUAGES NONE
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(formulis_formulis INTERFACE)
add_library(formulis::formulis ALIAS formulis_formulis)

set_property(
    TARGET formulis_formulis PROPERTY
    EXPORT_NAME formulis
)

target_include_directories(
    formulis_formulis ${warning_guard}
    INTERFACE
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(formulis_formulis INTERFACE cxx_std_17)

find_package(fmt REQUIRED)
target_link_libraries(formulis_formulis INTERFACE fmt::fmt)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT formulis_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of formulis"
  )
endif()

include(cmake/dev-mode.cmake)
