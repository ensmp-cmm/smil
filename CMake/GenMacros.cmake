# Copyright (c) 2011-2016, Matthieu FAESSEL and ARMINES
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Matthieu FAESSEL, or ARMINES nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


MACRO(INSTALL_LIB _LIB_NAME)
	INSTALL(TARGETS ${_LIB_NAME} 
		LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT library
		ARCHIVE DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT library
		RUNTIME DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT library
	)
ENDMACRO(INSTALL_LIB _LIB_NAME)


# ADD_SMIL_LIBRARY(libName libDeps [ADDITIONAL_SOURCES add_src1 ...] [EXCLUDED_SOURCES excl_src1 ...])

MACRO(ADD_SMIL_LIBRARY _LIB_NAME)

	PARSE_ARGUMENTS(_TARGET
		"ADDITIONAL_SOURCES;EXCLUDED_SOURCES;COMPILE_FLAGS;LINK_FLAGS"
		""
		${ARGN}
	)

	STRING(TOUPPER ${_LIB_NAME} UPPER_LIB_NAME)
	
	INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include/private)
	STRING(REPLACE "${CMAKE_SOURCE_DIR}/" "" CURRENT_RELATIVE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	STRING(REGEX MATCH "NSTypes/.*" IS_NSTYPE ${CURRENT_RELATIVE_SOURCE_DIR})
	
	LIST_CONTAINS(IS_BASE_MODULE ${_LIB_NAME} ${MODULES})
	IF(IS_BASE_MODULE OR IS_NSTYPE)
		SET(COMPONENT_PREFIX )
		SET(COMPONENT_PREFIX_ )
	ELSE()
		SET(COMPONENT_PREFIX ${_LIB_NAME})
		SET(COMPONENT_PREFIX_ ${_LIB_NAME}-)
	ENDIF()
	SET(COMPONENT_LIST)
	
	SET(LIB_NAME ${SMIL_LIB_PREFIX}${_LIB_NAME})
	SET(LIB_DEPS ${_TARGET_DEFAULT_ARGS} ${SMIL_EXT_DEPS})
	# Add non-smil libs to ext-deps
	IF(_TARGET_DEFAULT_ARGS)
	    STRING(REGEX REPLACE "smil[^;]*" "" LIB_EXT_DEPS ${_TARGET_DEFAULT_ARGS})
	ENDIF()
	LIST(APPEND SMIL_EXT_DEPS ${LIB_EXT_DEPS})
	SET(SMIL_EXT_DEPS ${SMIL_EXT_DEPS} PARENT_SCOPE)
	
	FILE(GLOB_RECURSE  LIB_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
		*.h 
		*.hpp
		*.hxx
		)
	FOREACH(FCH ${LIB_HEADERS})
		INSTALL(FILES ${FCH} DESTINATION include/Smil/${CURRENT_RELATIVE_SOURCE_DIR}/ RENAME ${FCH} 
		      COMPONENT ${COMPONENT_PREFIX_}dev)
	ENDFOREACH()
	LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}dev)
	
	IF(IS_BASE_MODULE AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/D${MODULE_NAME}.h)
		SET(SMIL_GLOBAL_HEADERS ${SMIL_GLOBAL_HEADERS} ${CURRENT_RELATIVE_SOURCE_DIR}/include/D${MODULE_NAME}.h PARENT_SCOPE)
	ENDIF()
	
	FILE(GLOB LIB_SRCS src/*.cpp ${_TARGET_ADDITIONAL_SOURCES})
	IF(_TARGET_EXCLUDED_SOURCES AND LIB_SRCS)
		FILE(GLOB_RECURSE EXCL_SRCS ${_TARGET_EXCLUDED_SOURCES})
		IF(EXCL_SRCS)
			LIST(REMOVE_ITEM LIB_SRCS ${EXCL_SRCS})
		ENDIF(EXCL_SRCS)
	ENDIF(_TARGET_EXCLUDED_SOURCES AND LIB_SRCS)

	IF(LIB_SRCS)
		ADD_LIBRARY(${LIB_NAME} ${LIB_SRCS} ${_TARGET_ADDITIONAL_SOURCES})
		IF(_TARGET_COMPILE_FLAGS)
			SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES COMPILE_FLAGS ${_TARGET_COMPILE_FLAGS})
		ENDIF(_TARGET_COMPILE_FLAGS)
		IF(_TARGET_LINK_FLAGS)
			SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES LINK_FLAGS ${_TARGET_LINK_FLAGS})
		ENDIF(_TARGET_LINK_FLAGS)
		IF(MINGW)
			SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES PREFIX "")
			IF(NOT BUILD_SHARED_LIBS)
				SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES SUFFIX ".dll.a")
			ENDIF(NOT BUILD_SHARED_LIBS)
		ENDIF(MINGW)
		IF(LIB_DEPS)
			TARGET_LINK_LIBRARIES(${LIB_NAME} ${LIB_DEPS}) #gomp pthread)
		ENDIF(LIB_DEPS)
		SET(SMIL_LIBS ${SMIL_LIBS} ${LIB_NAME} PARENT_SCOPE)

		SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES VERSION ${SMIL_VERSION})

		IF(BUILD_SHARED_LIBS)
			INSTALL(TARGETS ${LIB_NAME}
					LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}base
					ARCHIVE DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}base
					RUNTIME DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}base
				)
			LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}base)
		ENDIF(BUILD_SHARED_LIBS)

		INSTALL(TARGETS ${LIB_NAME}
				LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}dev
				ARCHIVE DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}dev
				RUNTIME DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}dev
			)
	ENDIF(LIB_SRCS)


	IF(USE_WRAPPER AND SWIG_FOUND AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}.i)
	
		  SET(SWIG_INTERFACE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}.i)
		  SET(SWIG_INTERFACE_FILES ${SWIG_INTERFACE_FILES} ${SWIG_INTERFACE_FILE} PARENT_SCOPE)

		  SET(CPP_LIB_NAME ${LIB_NAME}Cpp)
		  SET(PYTHON_LIB_NAME ${LIB_NAME}Python)
		  SET(JAVA_LIB_NAME ${LIB_NAME}Java)
		  SET(OCTAVE_LIB_NAME ${LIB_NAME}Octave)
		  SET(RUBY_LIB_NAME ${LIB_NAME}Ruby)
		  SET(PHP_LIB_NAME ${LIB_NAME}Php)

  		  SET_SOURCE_FILES_PROPERTIES(${LIB_NAME}.i PROPERTIES CPLUSPLUS ON)
  		  
  		  ADD_DEFINITIONS(-DSWIG_WRAP_${UPPER_LIB_NAME} -Wno-maybe-uninitialized -Wno-strict-aliasing -Wno-unused-function)
		  
		  IF(WRAP_CPP)
			SMIL_WRAP_CPP(${LIB_NAME} ${CPP_LIB_NAME})
			TARGET_LINK_LIBRARIES(${CPP_LIB_NAME} ${LIB_DEPS})
			INSTALL(TARGETS ${CPP_LIB_NAME} 
				LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}cpp
				ARCHIVE DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}cpp
				)
				LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}cpp)
		  ENDIF(WRAP_CPP)
# 		  
		  IF(WRAP_PYTHON)
			# Trick to avoid to rebuild every time even without the dependencies having changed
			SET_SOURCE_FILES_PROPERTIES(${LIB_NAME}.i PROPERTIES SWIG_MODULE_NAME ${LIB_NAME}Python)
			
			SET(CMAKE_SWIG_OUTDIR "${LIBRARY_OUTPUT_PATH}/smilPython")
			SWIG_ADD_MODULE(${PYTHON_LIB_NAME} python ${LIB_NAME}.i)
			SWIG_LINK_LIBRARIES(${PYTHON_LIB_NAME} ${LIB_DEPS} ${PYTHON_LIBRARIES} ${SWIG_DEPS})
# 			SET_TARGET_PROPERTIES(_${PYTHON_LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH}/smilPython)
			IF(LIB_SRCS)
			      SWIG_LINK_LIBRARIES(${PYTHON_LIB_NAME} ${LIB_NAME} smilCore)
			ENDIF(LIB_SRCS)
			INSTALL(TARGETS _${PYTHON_LIB_NAME} LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}python)
			INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/smilPython/${LIB_NAME}Python.py DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH}/smilPython COMPONENT ${COMPONENT_PREFIX_}python)
			LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}python)
			ADD_DEPENDENCIES(python _${PYTHON_LIB_NAME})
		  ENDIF(WRAP_PYTHON)
# 		  
		  IF(WRAP_OCTAVE)
			SET(CMAKE_SWIG_OUTDIR "${LIBRARY_OUTPUT_PATH}/smilOctave")
			SWIG_ADD_MODULE(${OCTAVE_LIB_NAME} octave ${LIB_NAME}.i)
			SET_TARGET_PROPERTIES(${OCTAVE_LIB_NAME} PROPERTIES PREFIX "" SUFFIX ".oct")
			SWIG_LINK_LIBRARIES(${OCTAVE_LIB_NAME} ${LIB_DEPS} smilCore)
			IF(LIB_SRCS)
			      SWIG_LINK_LIBRARIES(${OCTAVE_LIB_NAME} ${LIB_NAME})
			ENDIF(LIB_SRCS)
			INSTALL(TARGETS ${OCTAVE_LIB_NAME} LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}octave)
			LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}octave)
			ADD_DEPENDENCIES(octave ${OCTAVE_LIB_NAME})
		  ENDIF(WRAP_OCTAVE)
# 		  
		  IF(WRAP_RUBY)
			SET(CMAKE_SWIG_OUTDIR "${LIBRARY_OUTPUT_PATH}/smilRuby")
			SWIG_ADD_MODULE(${RUBY_LIB_NAME} ruby ${LIB_NAME}.i)
			SET_TARGET_PROPERTIES(${RUBY_LIB_NAME} PROPERTIES PREFIX "")
			SWIG_LINK_LIBRARIES(${RUBY_LIB_NAME} ${LIB_DEPS} smilCore)
			IF(LIB_SRCS)
			      SWIG_LINK_LIBRARIES(${RUBY_LIB_NAME} ${LIB_NAME})
			ENDIF(LIB_SRCS)
			INSTALL(TARGETS ${RUBY_LIB_NAME} LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}ruby)
			LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}ruby)
			ADD_DEPENDENCIES(ruby ${RUBY_LIB_NAME})
		  ENDIF(WRAP_RUBY)
# 		  
		  IF(WRAP_PHP)
			SET(CMAKE_SWIG_OUTDIR "${LIBRARY_OUTPUT_PATH}/smilPhp")
			INCLUDE_DIRECTORIES(${LIBRARY_OUTPUT_PATH}/smilPhp)
			SWIG_ADD_MODULE(${PHP_LIB_NAME} php ${LIB_NAME}.i)
			SET_TARGET_PROPERTIES(${PHP_LIB_NAME} PROPERTIES PREFIX "")
			SWIG_LINK_LIBRARIES(${PHP_LIB_NAME} ${LIB_DEPS} smilCore)
			IF(LIB_SRCS)
			      SWIG_LINK_LIBRARIES(${PHP_LIB_NAME} ${LIB_NAME})
			ENDIF(LIB_SRCS)
			INSTALL(TARGETS ${PHP_LIB_NAME} LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}php)
			LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}php)
			ADD_DEPENDENCIES(php ${PHP_LIB_NAME})
		  ENDIF(WRAP_PHP)

		  # Keep Java in last position because of the "package" flags (which should not have a general impact)
		  IF(WRAP_JAVA)
			SET(CMAKE_SWIG_OUTDIR "${LIBRARY_OUTPUT_PATH}/smilJava")
                        SET_PROPERTY(SOURCE ${LIB_NAME}.i PROPERTY SWIG_FLAGS -package smil${COMPONENT_PREFIX}Java)
			SWIG_ADD_MODULE(${JAVA_LIB_NAME} java ${LIB_NAME}.i)
			SWIG_LINK_LIBRARIES(${JAVA_LIB_NAME} ${LIB_DEPS})
			IF(LIB_SRCS)
			      SWIG_LINK_LIBRARIES(${JAVA_LIB_NAME} ${LIB_NAME} smilCore)
			ENDIF(LIB_SRCS)
			INSTALL(TARGETS ${JAVA_LIB_NAME} LIBRARY DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH} COMPONENT ${COMPONENT_PREFIX_}java)
			INSTALL(DIRECTORY ${LIBRARY_OUTPUT_PATH}/smil${COMPONENT_PREFIX}Java
				DESTINATION ${SMIL_LIBRARIES_INSTALL_PATH}
				COMPONENT ${COMPONENT_PREFIX_}java)
			LIST(APPEND COMPONENT_LIST ${COMPONENT_PREFIX_}java)
			ADD_DEPENDENCIES(java ${JAVA_LIB_NAME})
		  ENDIF(WRAP_JAVA)


	ENDIF(USE_WRAPPER AND SWIG_FOUND AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LIB_NAME}.i)
	
	LIST_CONTAINS(IS_IN_LIST ${COMPONENT_LIST} ${SMIL_INSTALL_COMPONENT_LIST};"")
	IF(NOT IS_IN_LIST)
		SET(SMIL_INSTALL_COMPONENT_LIST ${SMIL_INSTALL_COMPONENT_LIST} ${COMPONENT_LIST} PARENT_SCOPE)
	ENDIF(NOT IS_IN_LIST)
	
ENDMACRO(ADD_SMIL_LIBRARY _LIB_NAME _LIB_DEPS)




MACRO(ADD_SMIL_TESTS _LIB_NAME)
	
	PARSE_ARGUMENTS(_TARGET
		"ADDITIONAL_SOURCES;EXCLUDED_SOURCES"
		""
		${ARGN}
	)
	SET(LIB_NAME ${SMIL_LIB_PREFIX}${_LIB_NAME})
	SET(LIB_DEPS ${_TARGET_DEFAULT_ARGS} ${SMIL_EXT_DEPS})

	IF(BUILD_TEST)
		SET(FILE_PATTERNS test/test*.cpp test/bench*.cpp)
		IF(WRAP_PYTHON)
			LIST(APPEND FILE_PATTERNS test/python*.cpp)
		ENDIF(WRAP_PYTHON)
		FILE(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}  ${FILE_PATTERNS})
		
		IF(_TARGET_EXCLUDED_SOURCES AND TEST_SRCS)
			FILE(GLOB_RECURSE EXCL_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
			      test/${_TARGET_EXCLUDED_SOURCES})
			IF(EXCL_SRCS)
				LIST(REMOVE_ITEM TEST_SRCS ${EXCL_SRCS})
			ENDIF(EXCL_SRCS)
		ENDIF(_TARGET_EXCLUDED_SOURCES AND TEST_SRCS)
		
		STRING(REPLACE "${PROJECT_SOURCE_DIR}" "" MOD_NAME ${CMAKE_CURRENT_SOURCE_DIR})
		IF(MOD_NAME)
		  STRING(REPLACE "/" "" MOD_NAME ${MOD_NAME})
		  SET(MOD_NAME ${MOD_NAME}_)
		ENDIF(MOD_NAME)

		IF(TEST_SRCS)
			FOREACH(_SRC ${TEST_SRCS})
				STRING(REPLACE ".cpp" "" TEST_NAME ${_SRC})
				STRING(REPLACE "test/" "" TEST_NAME ${TEST_NAME})
				STRING(REGEX REPLACE "(^[^_]+)_.+" "\\1" _EXE_PREFIX ${TEST_NAME})
				
				ADD_EXECUTABLE(${TEST_NAME} ${_SRC})
				IF(TARGET ${LIB_NAME})
					TARGET_LINK_LIBRARIES(${TEST_NAME} ${LIB_NAME} smilIO)
				ENDIF(TARGET ${LIB_NAME})
				TARGET_LINK_LIBRARIES(${TEST_NAME} ${LIB_DEPS})
				
				IF(${_EXE_PREFIX} STREQUAL "test")
				    ADD_TEST("${MOD_NAME}${TEST_NAME}" ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME})
				    ADD_DEPENDENCIES(tests ${TEST_NAME})
				ELSEIF(${_EXE_PREFIX} STREQUAL "python")
				    ADD_TEST("${MOD_NAME}${TEST_NAME}" ${EXECUTABLE_OUTPUT_PATH}/${TEST_NAME})
				    TARGET_LINK_LIBRARIES(${TEST_NAME} ${PYTHON_LIBRARIES})
				    ADD_DEPENDENCIES(tests ${TEST_NAME})
				ELSEIF(${_EXE_PREFIX} STREQUAL "bench")
				    ADD_DEPENDENCIES(benchs ${TEST_NAME})
				ENDIF(${_EXE_PREFIX} STREQUAL "test")
			ENDFOREACH(_SRC ${TEST_SOURCE_FILES})
# 			INSTALL(TARGETS ${RUBY_LIB_NAME} RUNTIME DESTINATION ${SMIL_BINARIES_INSTALL_PATH} COMPONENT tests)
		ENDIF(TEST_SRCS)
	ENDIF(BUILD_TEST)
	
ENDMACRO(ADD_SMIL_TESTS _LIB_NAME)




MACRO(FIND_SMIL_MODULE_SUBDIRECTORIES)
    FILE(GLOB _MOD_CMLISTS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "${CMAKE_CURRENT_SOURCE_DIR}/*/CMakeLists.txt")
    FOREACH(_FCH ${_MOD_CMLISTS})
            GET_FILENAME_COMPONENT(_DIR ${_FCH} PATH)
            STRING(TOUPPER ${_DIR} _USER_MOD)
            LIST(APPEND AVAILABLE_USER_MODS ${_DIR})
            OPTION(ADD_USER_MOD_${_USER_MOD} "Build additional user module ${_DIR}" OFF)
            MARK_AS_ADVANCED(ADDON_${_USER_MOD})
            IF(ADD_USER_MOD_${_USER_MOD})
                    INCLUDE_DIRECTORIES(${_DIR} ${_DIR}/include ${_DIR}/include/private)
                    ADD_SUBDIRECTORY(${_DIR})
            ENDIF(ADD_USER_MOD_${_USER_MOD})
    ENDFOREACH(_FCH ${_MOD_CMLISTS})
ENDMACRO(FIND_SMIL_MODULE_SUBDIRECTORIES _DIR_NAME)


##### LIST_CONTAINS #####

#  http://www.cmake.org/Wiki/CMakeMacroListOperations#LIST_CONTAINS

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)



##### PARSE_ARGUMENTS #####

# http://www.cmake.org/Wiki/CMakeMacroParseArguments

# There is a general convention for CMake commands that take optional flags and/or variable arguments. 
# Optional flags are all caps and are added to the arguments to turn on. 
# Variable arguments have an all caps identifier to determine where each variable argument list starts. 
# The PARSE_ARGUMENTS macro, defined below, can be used by other macros to parse arguments defined in this way. 
# Note that this macro relies on the  LIST_CONTAINS command. 

MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})
    LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
    IF (is_arg_name)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name)
      LIST_CONTAINS(is_option ${arg} ${option_names})
      IF (is_option)
	SET(${prefix}_${arg} TRUE)
      ELSE (is_option)
	SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option)
    ENDIF (is_arg_name)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)



#### PKG-CONFIG ####
# Input: LIB_NAME (which is supposed to have an entry in pkg-config) [ STATIC ]
# Outputs: 
#	* LIB_NAME_PKG_FOUND: true if the pkg has been found
#	* LIB_NAME_DEFS: compiler flags
#	* LIB_NAME_LINK_DIRS: linker directories
#	* LIB_NAME_LINK_LIBS: lib dependancies

MACRO(ADD_PKG_CONFIG_DEFS _LIB_NAME)
  FIND_PACKAGE(PkgConfig)
  IF(PKG_CONFIG_FOUND)
      PKG_CHECK_MODULES(${_LIB_NAME}_MOD ${_LIB_NAME})
      
      IF(${_LIB_NAME}_MOD_FOUND)
	  SET(${_LIB_NAME}_PKG_FOUND TRUE)
	  SET(${_LIB_NAME}_DEFS ${${_LIB_NAME}_MOD_CFLAGS})
	  SET(${_LIB_NAME}_LINK_DIRS)
	  SET(${_LIB_NAME}_LINK_LIBS)
	  IF("${ARGV1}" STREQUAL "STATIC")
	      SET(${_LIB_NAME}_LDFLAGS ${${_LIB_NAME}_MOD_STATIC_LDFLAGS})
	  ELSE("${ARGV1}" STREQUAL "STATIC")
	      SET(${_LIB_NAME}_LDFLAGS ${${_LIB_NAME}_MOD_LDFLAGS})
	  ENDIF("${ARGV1}" STREQUAL "STATIC")
	  FOREACH(_FLAG ${${_LIB_NAME}_LDFLAGS})
	      IF(_FLAG MATCHES "^-l.*")
		  STRING(REGEX REPLACE "^-l" "" _LIB ${_FLAG})
		  LIST(APPEND ${_LIB_NAME}_LINK_LIBS ${_LIB})
	      ELSEIF(_FLAG MATCHES "^-L.*")
		  STRING(REGEX REPLACE "^-L" "" _LIB_DIR ${_FLAG})
		  LIST(APPEND ${_LIB_NAME}_LINK_DIRS ${_LIB_DIR})
	      ENDIF(_FLAG MATCHES "^-l.*")
	  ENDFOREACH(_FLAG ${${_LIB_NAME}_LDFLAGS})
	  
	  ADD_DEFINITIONS(${${_LIB_NAME}_DEFS})
	  LINK_DIRECTORIES(${${_LIB_NAME}_LINK_DIRS})
	  LIST(APPEND SMIL_EXT_DEPS ${${_LIB_NAME}_LINK_LIBS})
	  
      ENDIF(${_LIB_NAME}_MOD_FOUND)
  ENDIF(PKG_CONFIG_FOUND)
ENDMACRO(ADD_PKG_CONFIG_DEFS _MODULE)
