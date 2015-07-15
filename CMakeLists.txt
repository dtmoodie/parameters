CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

PROJECT(Parameters)
IF(WIN32)
	set(Boost_USE_STATIC_LIBS        OFF)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME     OFF)
ELSE(WIN32)
	set(Boost_USE_STATIC_LIBS        OFF)
	set(Boost_USE_MULTITHREADED      ON)
	set(Boost_USE_STATIC_RUNTIME     OFF)
ENDIF(WIN32)
FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system thread chrono date_time)
IF(Boost_FOUND)
	ADD_DEFINITIONS(-DBoost_FOUND)
	ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB) 
	INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
ENDIF()


FILE(GLOB_RECURSE hdr "include/*.hpp" "include/*.h")
FILE(GLOB_RECURSE src "src/*.c" "src/*.cpp")

SET(Parameters_UI TRUE CACHE BOOL "Enable UI generators")
SET(Parameters_persistence TRUE CACHE BOOL "Enable Persistence")
SET(Parameters_persistence_CV TRUE CACHE BOOL "Enable opencv Persistence")
SET(Parameters_UI_qt TRUE CACHE BOOL "Enable qt UI generators")

SET(USING_UI_ FALSE)
SET(UI_INCLUDES_ "")
SET(UI_LIBS_)
SET(MOC "")
IF(${Parameters_UI})
	IF(${Parameters_UI_qt})
		FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets)
		IF(${Qt5_FOUND})
			SET(USING_UI_ "QT")
			ADD_DEFINITIONS(-DQt5_FOUND)
			ADD_DEFINITIONS(-DPARAMTERS_USE_UI)
			QT5_WRAP_CPP(MOC ${hdr} ${src})
			INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
			GET_PROPERTY(qt_core_ TARGET Qt5::Core PROPERTY IMPORTED_IMPLIB_RELEASE)
			GET_PROPERTY(qt_gui_ TARGET Qt5::Gui PROPERTY IMPORTED_IMPLIB_RELEASE)
			GET_PROPERTY(qt_widgets_ TARGET Qt5::Widgets PROPERTY IMPORTED_IMPLIB_RELEASE)
			#LIST(APPEND UI_LIBS_ "Qt5::Core Qt5::Gui Qt5::Widgets")
			LIST(APPEND UI_LIBS_ ${qt_core_})
			LIST(APPEND UI_LIBS_ ${qt_gui_})
			LIST(APPEND UI_LIBS_ ${qt_widgets_})
		ENDIF()
	ENDIF()
ENDIF()
SET(PERSISTENCE_INCLUDES_ "")
SET(PERSITENCE_LIBS_)
IF(${Parameters_persistence})
	FIND_PACKAGE(OpenCV COMPONENTS core)
	IF(${OpenCV_FOUND})
		ADD_DEFINITIONS(-DOPENCV_FOUND)
		INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
		LIST(APPEND PERSISTENCE_LIBS_ opencv_core)
	ENDIF()
ENDIF()


message(STATUS
"============== libParameter =============
	Boost libs: ${Boost_LIBRARIES}
	UI: ${USING_UI_}
 " )

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include")
 

LIST(APPEND Parameters_LIBRARIES "${Boost_LIBRARIES};${PERSISTENCE_LIBS_};${UI_LIBS_}")

add_library(libParameter SHARED ${hdr} ${src} ${MOC})
TARGET_LINK_LIBRARIES(libParameter ${Parameters_LIBRARIES})
LIST(APPEND Parameters_LIBRARIES libParameter)

add_executable(test_parameter "test/parameter_test.cpp")
TARGET_LINK_LIBRARIES(test_parameter libParameter ${Parameters_LIBRARIES})
ADD_DEPENDENCIES(test_parameter libParameter)

IF(${Qt5_FOUND})
	FILE(GLOB qt_src "test/qt_policy_test/src/*.cpp")
	FILE(GLOB qt_hdr "test/qt_policy_test/include/*.h")
	FILE(GLOB forms "test/qt_policy_test/ui/*.ui")
	QT5_WRAP_CPP(MOC_qt ${qt_hdr})
	QT5_WRAP_UI(UIS ${forms})
	INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/test/qt_policy_test/include")
	add_executable(test_param_qt ${MOC_qt} ${UIS} ${qt_src} ${qt_hdr})
	
	TARGET_LINK_LIBRARIES(test_param_qt 
		Qt5::Core
		Qt5::Gui
		Qt5::Widgets
		opencv_core
		${Boost_LIBRARIES}
		libParameter
	)

	TARGET_LINK_LIBRARIES(test_parameter 
		Qt5::Core
		Qt5::Gui
		Qt5::Widgets
		opencv_core
		${Boost_LIBRARIES}
		libParameter
	)
add_dependencies(test_param_qt libParameter)
ENDIF()

SET(Parameters_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include;${UI_INCLUDES_}" CACHE STRING "" FORCE)
SET(Parameters_LIBRARIES ${Parameters_LIBRARIES} CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS ${CMAKCE_CURRENT_BINARY_DIR} CACHE STRING "" FORCE)
  