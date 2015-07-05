CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(Parameters)

FIND_PACKAGE(Boost)
FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets)
FIND_PACKAGE(OpenCV COMPONENTS core)

FILE(GLOB_RECURSE hdr "include/*.hpp" "include/*.h")
FILE(GLOB_RECURSE src "src/*.c" "src/*.cpp")

IF(Qt5_FOUND)
 ADD_DEFINITIONS(-DQt5_FOUND)
 QT5_WRAP_CPP(MOC ${hdr} ${src})
ENDIF()

IF(Boost_FOUND)
 ADD_DEFINITIONS(-DBoost_FOUND)
ENDIF()


INCLUDE_DIRECTORIES(
 ${Boost_INCLUDE_DIRS} 
 "${CMAKE_CURRENT_SOURCE_DIR}/src" 
 "${CMAKE_CURRENT_SOURCE_DIR}/include"
 ${Qt5Gui_INCLUDE_DIRS} 
 ${Qt5Widgets_INCLUDE_DIRS} 
 ${OpenCV_INCLUDE_DIRS}
 )
 
SET(Parameters_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)

add_library(libParameter ${hdr} ${src} ${MOC})

add_executable(test_parameter "test/parameter_test.cpp")
TARGET_LINK_LIBRARIES(test_parameter libParameter ${OpenCV_LIBS})
ADD_DEPENDENCIES(test_parameter libParameter)


IF(${Qt5_FOUND})
 FILE(GLOB qt_src "test/qt_policy_test/src/*.cpp")
 FILE(GLOB qt_hdr "test/qt_policy_test/include/*.h")
 FILE(GLOB forms "test/qt_policy_test/ui/*.ui")
 QT5_WRAP_CPP(MOC_qt ${qt_src} ${qt_hdr})
 QT5_WRAP_UI(UIS ${forms})
 INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/test/qt_policy_test/include")

 add_executable(test_param_qt ${MOC_qt} ${UIS} ${qt_src} ${qt_hdr})
 TARGET_LINK_LIBRARIES(test_param_qt 
  Qt5::Core
  Qt5::Gui
  Qt5::Widgets
  ${OpenCV_LIBS}
  libParameter
  )
 add_dependencies(test_param_qt libParameter)
ENDIF()


  