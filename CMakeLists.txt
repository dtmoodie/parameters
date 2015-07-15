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
SET(UI_LIBS)
SET(UI_LIB_DIR_REL_)
SET(UI_LIB_DIR_DEB_)
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
			
			GET_TARGET_PROPERTY(qt_gui_rel Qt5::Gui IMPORTED_IMPLIB_RELEASE)
			GET_TARGET_PROPERTY(qt_widgets_rel Qt5::Widgets IMPORTED_IMPLIB_RELEASE)
			GET_TARGET_PROPERTY(qt_gui_deb Qt5::Gui IMPORTED_IMPLIB_DEBUG)
			GET_TARGET_PROPERTY(qt_widgets_deb Qt5::Widgets IMPORTED_IMPLIB_DEBUG)
			
			LIST(APPEND UI_LIBS "optimized;${qt_gui_rel}")
			LIST(APPEND UI_LIBS "optimized;${qt_widgets_rel}")
			LIST(APPEND UI_LIBS "debug;${qt_gui_deb}")
			LIST(APPEND UI_LIBS "debug;${qt_widgets_deb}")
			
			# Directories
			GET_FILENAME_COMPONENT(qt_gui_dir_rel ${qt_gui_rel} DIRECTORY)
			GET_FILENAME_COMPONENT(qt_gui_dir_deb ${qt_gui_deb} DIRECTORY)
			GET_FILENAME_COMPONENT(qt_widgets_dir_rel ${qt_widgets_rel} DIRECTORY)
			GET_FILENAME_COMPONENT(qt_widgets_dir_deb ${qt_widgets_deb} DIRECTORY)
			
			LIST(APPEND UI_LIB_DIR_REL_ ${qt_widgets_dir_rel})
			LIST(APPEND UI_LIB_DIR_REL_ ${qt_gui_dir_rel})
			LIST(APPEND UI_LIB_DIR_DEB_ ${qt_widgets_dir_deb})
			LIST(APPEND UI_LIB_DIR_DEB_ ${qt_widgets_dir_reb})
			
		ENDIF()
	ENDIF()
ENDIF()
SET(PERSISTENCE_INCLUDES_ "")
SET(PERSITENCE_LIBS)
SET(PERSISTENCE_LIB_DIR_REL_)
SET(PERSISTENCE_LIB_DIR_DEB_)
IF(${Parameters_persistence})
	FIND_PACKAGE(OpenCV COMPONENTS core)
	IF(${OpenCV_FOUND})
		ADD_DEFINITIONS(-DOPENCV_FOUND)
		INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
		
		GET_TARGET_PROPERTY(cv_core_lib_deb opencv_core IMPORTED_IMPLIB_DEBUG)
		GET_TARGET_PROPERTY(cv_core_lib_rel opencv_core IMPORTED_IMPLIB_RELEASE)
		
		LIST(APPEND PERSITENCE_LIBS "debug;${cv_core_lib_deb}")
		LIST(APPEND PERSITENCE_LIBS "optimized;${cv_core_lib_rel}")
		
		#directories
		GET_FILENAME_COMPONENT(cv_core_lib_dir_rel_ ${cv_core_lib_rel} DIRECTORY)
		GET_FILENAME_COMPONENT(cv_core_lib_dir_deb_ ${cv_core_lib_deb} DIRECTORY)
		
		LIST(APPEND PERSISTENCE_LIB_DIR_REL_ ${cv_core_lib_dir_rel_})
		LIST(APPEND PERSISTENCE_LIB_DIR_DEB_ ${cv_core_lib_dir_deb_})
		
	ENDIF()
ENDIF()


message(STATUS
"============== libParameter =============
	Boost libs: ${Boost_LIBRARIES}
	UI: ${USING_UI_}
	qt_gui: ${qt_gui_}

	qt_widgets: ${qt_widgets_}
 " )

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include")
 

LIST(APPEND Parameters_LIBRARIES "${Boost_LIBRARIES};${PERSITENCE_LIBS};${UI_LIBS};${CMAKE_CURRENT_BINARY_DIR}")

SET(Parameters_LIB_DIRS_RELEASE "${Boost_LIBRARY_DIR};${UI_LIB_DIR_REL_};${PERSISTENCE_LIB_DIR_REL_}")
SET(Parameters_LIB_DIRS_DEBUG "${Boost_LIBRARY_DIR};${UI_LIB_DIR_DEB_};${PERSISTENCE_LIB_DIR_DEB_}")


add_library(libParameter SHARED ${hdr} ${src} ${MOC})
TARGET_LINK_LIBRARIES(libParameter ${Parameters_LIBRARIES_RELEASE} ${Parameters_LIBRARIES_DEBUG})
LIST(APPEND Parameters_LIBRARIES_DEBUG libParameter)
LIST(APPEND Parameters_LIBRARIES_RELEASE libParameter)

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
LIST(REMOVE_DUPLICATES Parameters_LIB_DIRS_RELEASE)
LIST(REMOVE_DUPLICATES Parameters_LIB_DIRS_DEBUG)
LIST(REMOVE_DUPLICATES Parameters_LIB_DIRS_RELEASE)
LIST(REMOVE_DUPLICATES Parameters_LIB_DIRS_DEBUG)


SET(Parameters_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include;${UI_INCLUDES_}" CACHE STRING "" FORCE)
SET(Parameters_LIBRARIES ${Parameters_LIBRARIES} CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS_RELEASE "${Parameters_LIB_DIRS_RELEASE}" CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS_DEBUG "${Parameters_LIB_DIRS_DEBUG}" CACHE STRING "" FORCE)
  
