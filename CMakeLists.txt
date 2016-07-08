CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
CMAKE_POLICY(SET CMP0020 NEW)
PROJECT(Parameters)
set(link_libs "")

SET(Boost_REQUIRED_COMPONENTS filesystem system thread chrono date_time log log_setup)
if(${USE_PYTHON})
    list(APPEND Boost_REQUIRED_COMPONENTS python)
endif(${USE_PYTHON})

IF(NOT WIN32)        
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fpermissive")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
ENDIF(NOT WIN32)
FIND_PACKAGE(Boost REQUIRED COMPONENTS ${Boost_REQUIRED_COMPONENTS})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

IF(Boost_FOUND)
  IF(NOT WIN32)
    ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB) 
  ENDIF(NOT WIN32)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF()
IF(${USE_PYTHON})
  find_package(PythonLibs QUIET)
  if(PythonLibs_FOUND)
    add_definitions(-DHAVE_BOOST_PYTHON)
    list(APPEND link_libs "${PYTHON_LIBRAY}")
  endif()
endif()
if(NOT Signals_INCLUDE_DIRS)
  find_package(Signals)
endif()
  include_directories(${Signals_INCLUDE_DIRS})

FILE(GLOB_RECURSE hdr "include/*.hpp" "include/*.h")
FILE(GLOB_RECURSE src "src/*.c" "src/*.cpp")

SET(Parameters_UI TRUE CACHE BOOL "Enable UI generators")
SET(Parameters_persistence TRUE CACHE BOOL "Enable Persistence")
SET(Parameters_persistence_CV TRUE CACHE BOOL "Enable opencv Persistence")
SET(Parameters_UI_qt TRUE CACHE BOOL "Enable qt UI generators")
SET(Parameters_persistence_PROTOBUF FALSE CACHE BOOL "Enable protobuf")

SET(definitions_)
SET(USING_UI_ FALSE)
SET(UI_INCLUDES_ "")
SET(UI_LIBS)
SET(UI_LIB_DIR_REL_)
SET(UI_LIB_DIR_DEB_)
SET(MOC "")
ADD_DEFINITIONS("-DPARAMTERS_GENERATE_CONVERTERS")
IF(Parameters_UI)
  ADD_DEFINITIONS("-DPARAMETERS_GENERATE_UI")
  IF(Parameters_UI_qt)
    FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets Core)
    IF(Qt5_FOUND)
      SET(USING_UI_ "QT")
      ADD_DEFINITIONS("-DQt5_FOUND")
      QT5_WRAP_CPP(MOC ${hdr})
      SOURCE_GROUP("MOC"  FILES ${MOC})
      INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
      LIST(APPEND UI_INCLUDES_ "${Qt5Gui_INCLUDE_DIRS};${Qt5Widgets_INCLUDE_DIRS};${Qt5Core_INCLUDE_DIRS}")
            
      GET_TARGET_PROPERTY(qt_gui_rel Qt5::Gui IMPORTED_IMPLIB_RELEASE)
      IF(${qt_gui_rel} MATCHES "NOTFOUND")
        GET_TARGET_PROPERTY(qt_gui_rel Qt5::Gui LOCATION)
      ENDIF()
      GET_TARGET_PROPERTY(qt_gui_deb Qt5::Gui IMPORTED_IMPLIB_DEBUG)
      IF(${qt_gui_deb} MATCHES "NOTFOUND")
        GET_TARGET_PROPERTY(qt_gui_deb Qt5::Gui LOCATION)
      ENDIF()
      GET_TARGET_PROPERTY(qt_widgets_rel Qt5::Widgets IMPORTED_IMPLIB_RELEASE)
      IF(${qt_widgets_rel} MATCHES "NOTFOUND")
        GET_TARGET_PROPERTY(qt_widgets_rel Qt5::Widgets LOCATION)
      ENDIF()
      GET_TARGET_PROPERTY(qt_widgets_deb Qt5::Widgets IMPORTED_IMPLIB_DEBUG)
      IF(${qt_widgets_deb} MATCHES "NOTFOUND")
        GET_TARGET_PROPERTY(qt_widgets_deb Qt5::Widgets LOCATION)
      ENDIF()
      GET_TARGET_PROPERTY(qt_core_deb Qt5::Core IMPORTED_IMPLIB_DEBUG)
      IF(${qt_core_deb} MATCHES "NOTFOUND")
        GET_TARGET_PROPERTY(qt_core_deb Qt5::Core LOCATION)
      ENDIF()
      GET_TARGET_PROPERTY(qt_core_rel Qt5::Core IMPORTED_IMPLIB_RELEASE)
      IF(${qt_core_rel} MATCHES "NOTFOUND")
        GET_TARGET_PROPERTY(qt_core_rel Qt5::Core LOCATION)
      ENDIF()
            
      # Directories
      GET_FILENAME_COMPONENT(qt_gui_dir_rel ${qt_gui_rel} DIRECTORY)
      GET_FILENAME_COMPONENT(qt_gui_dir_deb ${qt_gui_deb} DIRECTORY)
      GET_FILENAME_COMPONENT(qt_widgets_dir_rel ${qt_widgets_rel} DIRECTORY)
      GET_FILENAME_COMPONENT(qt_widgets_dir_deb ${qt_widgets_deb} DIRECTORY)
      GET_FILENAME_COMPONENT(qt_core_dir_rel ${qt_core_rel} DIRECTORY)
      GET_FILENAME_COMPONENT(qt_core_dir_deb ${qt_core_deb} DIRECTORY)
            
      LIST(APPEND UI_LIB_DIR_REL_ ${qt_widgets_dir_rel})
      LIST(APPEND UI_LIB_DIR_REL_ ${qt_gui_dir_rel})
      LIST(APPEND UI_LIB_DIR_DEB_ ${qt_widgets_dir_deb})
      LIST(APPEND UI_LIB_DIR_DEB_ ${qt_widgets_dir_reb})
      LIST(APPEND UI_LIB_DIR_DEB_ ${qt_core_dir_deb})
      LIST(APPEND UI_LIB_DIR_DEB_ ${qt_core_dir_rel})
            
      IF(WIN32)
        GET_FILENAME_COMPONENT(qt_gui_rel ${qt_gui_rel} NAME)
        GET_FILENAME_COMPONENT(qt_gui_deb ${qt_gui_deb} NAME)
            
        GET_FILENAME_COMPONENT(qt_widgets_rel ${qt_widgets_rel} NAME)
        GET_FILENAME_COMPONENT(qt_widgets_deb ${qt_widgets_deb} NAME)
            
        GET_FILENAME_COMPONENT(qt_core_rel ${qt_core_rel} NAME)
        GET_FILENAME_COMPONENT(qt_core_deb ${qt_core_deb} NAME)
      ENDIF()
            
      LIST(APPEND UI_LIBS Qt5::Gui)
      LIST(APPEND UI_LIBS Qt5::Widgets)
      LIST(APPEND UI_LIBS Qt5::Core)            
      ELSE(Qt5_FOUND)

      ENDIF(Qt5_FOUND)
  ENDIF(Parameters_UI_qt)
ELSE(Parameters_UI)
  SET(Parameters_UI_qt FALSE CACHE BOOL "Enable qt UI generators" FORCE)
  LIST(REMOVE_ITEM src "src/UI/Qt.cpp")
  MESSAGE("Disabling QT ui generators")
ENDIF(Parameters_UI)




SET(PERSISTENCE_INCLUDES_ "")
SET(PERSITENCE_LIBS)
SET(PERSISTENCE_LIB_DIR_REL_)
SET(PERSISTENCE_LIB_DIR_DEB_)
IF(${Parameters_persistence})
  ADD_DEFINITIONS(-DPARAMTERS_GENERATE_PERSISTENCE)
  SET(PERSISTENCE_LIBS "")

  find_package(CUDA REQUIRED)
  get_filename_component(_cuda_lib_dir "${CUDA_CUDART_LIBRARY}" DIRECTORY)
  link_directories(${_cuda_lib_dir})
  set_target_properties(${OpenCV_LIBS} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO RELEASE)
  IF(${OpenCV_FOUND})
    ADD_DEFINITIONS(-DHAVE_OPENCV)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
        
    GET_TARGET_PROPERTY(cv_core_lib_deb opencv_core IMPORTED_IMPLIB_DEBUG)
    IF(${cv_core_lib_deb} MATCHES "NOTFOUND")
      GET_TARGET_PROPERTY(cv_core_lib_deb opencv_core LOCATION)
    ENDIF()
      GET_TARGET_PROPERTY(cv_core_lib_rel opencv_core IMPORTED_IMPLIB_RELEASE)
    IF(${cv_core_lib_rel} MATCHES "NOTFOUND")
      GET_TARGET_PROPERTY(cv_core_lib_rel opencv_core LOCATION)
    ENDIF()

    GET_FILENAME_COMPONENT(cv_core_lib_dir_rel_ ${cv_core_lib_rel} DIRECTORY)
    GET_FILENAME_COMPONENT(cv_core_lib_dir_deb_ ${cv_core_lib_deb} DIRECTORY)
        
    LIST(APPEND PERSISTENCE_LIB_DIR_REL_ ${cv_core_lib_dir_rel_})
    LIST(APPEND PERSISTENCE_LIB_DIR_DEB_ ${cv_core_lib_dir_deb_})
    IF(WIN32)
      GET_FILENAME_COMPONENT(cv_core_lib_deb ${cv_core_lib_deb} NAME)
      GET_FILENAME_COMPONENT(cv_core_lib_rel ${cv_core_lib_rel} NAME)
    ENDIF()
    LIST(APPEND PERSISTENCE_LIBS opencv_core)
  ENDIF() #opencv
  
  find_package(Protobuf QUIET)
  if(Protobuf_FOUND AND Parameters_persistence_PROTOBUF)
    include_directories(${PROTOBUF_INCLUDE_DIRS})
    file(GLOB_RECURSE proto_files "protobuf/*.proto")
    PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS ${proto_files})
    message("Creating protobuf serialization routines\n Input proto files: ${proto_files} \n Generated headers: ${PROTO_HDRS} \n Generated Source: ${PROTO_SRCS}")
    file(COPY 
        ${PROTO_HDRS} 
        DESTINATION ${CMAKE_CURRENT_LIST_DIR}/include/parameters/Persistence/)
    list(APPEND src ${PROTO_SRCS})
    list(APPEND hdr ${PROTO_HDRS})
    add_definitions(-DHAVE_PROTOBUF)
    LIST(APPEND PERSISTENCE_LIBS optimized;${PROTOBUF_LIBRARY};debug;${PROTOBUF_LIBRARY_DEBUG})
  endif() # Protobuf
  
  find_package(cereal REQUIRED)
  if(cereal_FOUND)
      include_directories(${cereal_INCLUDE_DIRS})
      add_definitions(-DHAVE_CEREAL)

  endif()

ENDIF()

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include")
SET(boost_lib_)
SET(boost_libs_)
FOREACH(lib ${Boost_LIBRARIES})
  IF(${lib} MATCHES "debug")
    SET(boost_lib_ "debug")
  ELSE()
    IF(${lib} MATCHES "optimized")
      SET(boost_lib_ "optimized")
    ELSE()
      GET_FILENAME_COMPONENT(lib "${lib}" NAME)
      LIST(APPEND boost_libs_ "${boost_lib_};${lib}")
    ENDIF()
  ENDIF()
ENDFOREACH()


SET(Parameters_LIBRARIES "${PERSISTENCE_LIBS};${UI_LIBS};${boost_libs_}")
SET(Parameters_LIB_DIRS_RELEASE "${UI_LIB_DIR_REL_};${PERSISTENCE_LIB_DIR_REL_}")
SET(Parameters_LIB_DIRS_DEBUG "${UI_LIB_DIR_DEB_};${PERSISTENCE_LIB_DIR_DEB_}")

IF(Boost_LIBRARY_DIR_RELEASE)
  LIST(APPEND Parameters_LIB_DIRS_RELEASE ${Boost_LIBRARY_DIR_RELEASE})
ELSE()
  IF(Boost_LIBRARY_DIR)
    LIST(APPEND Parameters_LIB_DIRS_RELEASE ${Boost_LIBRARY_DIR})
  ELSE()
    LIST(APPEND Parameters_LIB_DIRS_RELEASE ${Boost_LIBRARYDIR})
  ENDIF()
ENDIF()
IF(Boost_LIBRARY_DIR_DEBUG)
  LIST(APPEND Parameters_LIB_DIRS_DEBUG ${Boost_LIBRARY_DIR_DEBUG})
ENDIF()


message(STATUS
"============== libParameter =============
  Boost libs: ${boost_libs_}
  UI: ${USING_UI_}
  qt_gui: ${qt_gui_}
  qt_widgets: ${qt_widgets_}
  Release lib dirs: ${Parameters_LIB_DIRS_RELEASE}
  Debug lib dirs: ${Parameters_LIB_DIRS_DEBUG}
  Parameter Libs: ${Parameters_LIBRARIES}
")
LINK_DIRECTORIES(${Parameters_LIB_DIRS_RELEASE})
LINK_DIRECTORIES(${Parameters_LIB_DIRS_DEBUG})

add_library(parameters SHARED ${hdr} ${src} ${MOC})
add_dependencies(parameters signals)
TARGET_LINK_LIBRARIES(parameters 
    ${Parameters_LIBRARIES}
    signals
)
LIST(APPEND Parameters_LIBRARIES parameters)


find_package(Boost QUIET COMPONENTS unit_test_framework)
if(Boost_UNIT_TEST_FRAMEWORK_FOUND)
  file(GLOB_RECURSE test_srcs "test/*.cpp")
  include(CTest)
  enable_testing()
  foreach(fn ${test_srcs})
    find_package(CUDA REQUIRED)
    get_filename_component(_cuda_lib_dir "${CUDA_CUDART_LIBRARY}" DIRECTORY)
    link_directories(${_cuda_lib_dir})
    get_filename_component(name ${fn} NAME_WE)
    add_executable(${name} "${fn}")
    TARGET_LINK_LIBRARIES(${name} parameters ${Parameters_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    ADD_DEPENDENCIES(${name} parameters)
    set_target_properties(${name} PROPERTIES FOLDER Tests/parameters)
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
    add_test(${name} ${name})
    if(MSVC)
      CONFIGURE_FILE("test/Test.vcxproj.user.in" ${CMAKE_BINARY_DIR}/EagleLib/parameters/${name}.vcxproj.user @ONLY)
    endif()
  endforeach()
endif()


IF(${Qt5_FOUND})
  FILE(GLOB qt_src "example/example_qt_ui/src/*.cpp")
  FILE(GLOB qt_hdr "example/example_qt_ui/include/*.h")
  FILE(GLOB forms "example/example_qt_ui/ui/*.ui")
  QT5_WRAP_CPP(MOC_qt ${qt_hdr})
  QT5_WRAP_UI(UIS ${forms})
  INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/example/example_qt_ui/include")
  add_executable(test_param_qt ${MOC_qt} ${UIS} ${qt_src} ${qt_hdr})
  set_target_properties(test_param_qt PROPERTIES FOLDER examples)
  set_target_properties(test_param_qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
  TARGET_LINK_LIBRARIES(test_param_qt 
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    opencv_core
    parameters
  )


add_dependencies(test_param_qt parameters)
ENDIF()
LIST(REMOVE_DUPLICATES Parameters_LIB_DIRS_RELEASE)
LIST(REMOVE_DUPLICATES Parameters_LIB_DIRS_DEBUG)


SET(Parameters_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include;${UI_INCLUDES_}" CACHE STRING "" FORCE)
SET(Parameters_LIBRARIES "parameters;signals;${link_libs}" CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS_RELEASE "${Parameters_LIB_DIRS_RELEASE};${CMAKE_CURRENT_BINARY_DIR}"  CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS_DEBUG "${Parameters_LIB_DIRS_DEBUG};${CMAKE_CURRENT_BINARY_DIR}"  CACHE STRING "" FORCE)
SET(Parameters_DEFS "${definitions_}" CACHE STRING "" FORCE)

install(TARGETS parameters
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

install(DIRECTORY include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.h")

install(DIRECTORY include/
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp")
