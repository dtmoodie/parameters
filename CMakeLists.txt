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
	#ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB) 
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
		FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets Core)
		IF(${Qt5_FOUND})
			SET(USING_UI_ "QT")
			ADD_DEFINITIONS(-DQt5_FOUND)
			ADD_DEFINITIONS(-DPARAMTERS_USE_UI)
			QT5_WRAP_CPP(MOC ${hdr} ${src})
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
			
			
			#GET_FILENAME_COMPONENT(qt_gui_rel ${qt_gui_rel} NAME)
			#GET_FILENAME_COMPONENT(qt_gui_deb ${qt_gui_deb} NAME)
			
			#GET_FILENAME_COMPONENT(qt_widgets_rel ${qt_widgets_rel} NAME)
			#GET_FILENAME_COMPONENT(qt_widgets_deb ${qt_widgets_deb} NAME)
			
			#GET_FILENAME_COMPONENT(qt_core_rel ${qt_core_rel} NAME)
			#GET_FILENAME_COMPONENT(qt_core_deb ${qt_core_deb} NAME)
			
			
			LIST(APPEND UI_LIBS "optimized;${qt_gui_rel}")
			LIST(APPEND UI_LIBS "optimized;${qt_widgets_rel}")
			LIST(APPEND UI_LIBS "optimized;${qt_core_rel}")
			
			LIST(APPEND UI_LIBS "debug;${qt_gui_deb}")
			LIST(APPEND UI_LIBS "debug;${qt_widgets_deb}")
			LIST(APPEND UI_LIBS "debug;${qt_core_deb}")
			
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
		IF(${cv_core_lib_deb} MATCHES "NOTFOUND")
		GET_TARGET_PROPERTY(cv_core_lib_deb opencv_core LOCATION)
		ENDIF()
		GET_TARGET_PROPERTY(cv_core_lib_rel opencv_core IMPORTED_IMPLIB_RELEASE)
		IF(${cv_core_lib_rel} MATCHES "NOTFOUND")
		GET_TARGET_PROPERTY(cv_core_lib_rel opencv_core LOCATION)
		ENDIF()
		
		#directories
		GET_FILENAME_COMPONENT(cv_core_lib_dir_rel_ ${cv_core_lib_rel} DIRECTORY)
		GET_FILENAME_COMPONENT(cv_core_lib_dir_deb_ ${cv_core_lib_deb} DIRECTORY)
		
		LIST(APPEND PERSISTENCE_LIB_DIR_REL_ ${cv_core_lib_dir_rel_})
		LIST(APPEND PERSISTENCE_LIB_DIR_DEB_ ${cv_core_lib_dir_deb_})
		
		#GET_FILENAME_COMPONENT(cv_core_lib_deb ${cv_core_lib_deb} NAME)
		#GET_FILENAME_COMPONENT(cv_core_lib_rel ${cv_core_lib_rel} NAME)
		LIST(APPEND PERSITENCE_LIBS "debug;${cv_core_lib_deb}")
		LIST(APPEND PERSITENCE_LIBS "optimized;${cv_core_lib_rel}")
	ENDIF()
ENDIF()




INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/include")
#GET_FILENAME_COMPONENT(boost_libs_ "${Boost_LIBRARIES}" NAME)
SET(boost_libs_)
FOREACH(lib ${Boost_LIBRARIES})
MESSAGE(STATUS "${lib}
")
IF(${lib} MATCHES "debug")
ELSE()
IF(${lib} MATCHES "optimized")
ELSE()
GET_FILENAME_COMPONENT(lib "${lib}" NAME)
LIST(APPEND boost_libs_ ${lib})
ENDIF()

ENDIF()
ENDFOREACH()





#SET(Parameters_LIBRARIES "${boost_libs_};${PERSITENCE_LIBS};${UI_LIBS}")
SET(Parameters_LIBRARIES "${PERSITENCE_LIBS};${UI_LIBS}")

SET(Parameters_LIB_DIRS_RELEASE "${Boost_LIBRARY_DIR};${UI_LIB_DIR_REL_};${PERSISTENCE_LIB_DIR_REL_}")
SET(Parameters_LIB_DIRS_DEBUG "${Boost_LIBRARY_DIR};${UI_LIB_DIR_DEB_};${PERSISTENCE_LIB_DIR_DEB_}")
message(STATUS
"============== libParameter =============
	Boost libs: ${Boost_LIBRARIES}
	UI: ${USING_UI_}
	qt_gui: ${qt_gui_}

	qt_widgets: ${qt_widgets_}

	Release lib dirs: ${Parameters_LIB_DIRS_RELEASE}

	Debug lib dirs: ${Parameters_LIB_DIRS_DEBUG}
")
LINK_DIRECTORIES(${Parameters_LIB_DIRS_RELEASE})
LINK_DIRECTORIES(${Parameters_LIB_DIRS_DEBUG})

add_library(libParameter SHARED ${hdr} ${src} ${MOC})
TARGET_LINK_LIBRARIES(libParameter 
	${Parameters_LIBRARIES}
)
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
		#${Boost_LIBRARIES}
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


SET(Parameters_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include;${UI_INCLUDES_}" CACHE STRING "" FORCE)
SET(Parameters_LIBRARIES ${Parameters_LIBRARIES}  CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS_RELEASE "${Parameters_LIB_DIRS_RELEASE};${CMAKE_CURRENT_BINARY_DIR}"  CACHE STRING "" FORCE)
SET(Parameters_LIB_DIRS_DEBUG "${Parameters_LIB_DIRS_DEBUG};${CMAKE_CURRENT_BINARY_DIR}"  CACHE STRING "" FORCE)
  
