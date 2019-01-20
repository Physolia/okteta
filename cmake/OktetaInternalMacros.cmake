#
# Okteta Private Macros
#
# Copyright 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. The name of the author may not be used to endorse or promote products
#    derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# ECM
include(ECMGenerateHeaders)
# CMake
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(CMakeParseArguments)


macro(okteta_add_sublibrary _baseName)
    set(options
        NO_VERSIONED_INCLUDEDIR
        REVERSE_NAMESPACE_INCLUDEDIR
    )
    set(oneValueArgs
        SUBDIR
        SOURCE_TAG
    )
    set(multiValueArgs
        NAMESPACE
        PUBLIC
        PRIVATE
        KCFG
        UI
    )
    cmake_parse_arguments(OKTETA_ADD_SUBLIBRARY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    string(CONCAT _namespaceConcat ${OKTETA_ADD_SUBLIBRARY_NAMESPACE})
    string(TOLOWER "${_namespaceConcat}${_baseName}" _libname)
    if (OKTETA_ADD_SUBLIBRARY_SOURCE_TAG)
        set(_sourceTag "_${OKTETA_ADD_SUBLIBRARY_SOURCE_TAG}")
    else()
        set(_sourceTag)
    endif()
    if (OKTETA_ADD_SUBLIBRARY_SUBDIR)
        set(_relativePath "${OKTETA_ADD_SUBLIBRARY_SUBDIR}/")
        set(_egh_relative_param RELATIVE ${OKTETA_ADD_SUBLIBRARY_SUBDIR})
        string(REPLACE ".." "PARENTDIR" _subdir_id ${OKTETA_ADD_SUBLIBRARY_SUBDIR})
        string(REPLACE "/" "_" _subdir_id ${_subdir_id})
        set(_SRCS   ${_libname}_${_subdir_id}${_sourceTag}_SRCS)
        set(_HDRS   ${_libname}_${_subdir_id}_HDRS)
        set(_CCHDRS ${_libname}_${_subdir_id}_CCHDRS)
    else()
        set(_relativePath)
        set(_egh_relative_param)
        set(_SRCS   ${_libname}${_sourceTag}_LIB_SRCS)
        set(_HDRS   ${_libname}_LIB_HDRS)
        set(_CCHDRS ${_libname}_LIB_CCHDRS)
    endif()

    foreach(_classname ${OKTETA_ADD_SUBLIBRARY_PUBLIC} ${OKTETA_ADD_SUBLIBRARY_PRIVATE})
        string(TOLOWER "${_classname}" _lc_classname)

        set(_source "${_relativePath}${_lc_classname}.cpp")
        set(_actualsource "${CMAKE_CURRENT_SOURCE_DIR}/${_source}")
        if (EXISTS ${_actualsource})
            list(APPEND ${_SRCS} "${_source}")
        endif()

        set(_source "${_relativePath}${_lc_classname}_p.cpp")
        set(_actualsource "${CMAKE_CURRENT_SOURCE_DIR}/${_source}")
        if (EXISTS ${_actualsource})
            list(APPEND ${_SRCS} "${_source}")
        endif()
    endforeach()

    foreach(_kcfg ${OKTETA_ADD_SUBLIBRARY_KCFG})
        kconfig_add_kcfg_files(${_SRCS} "${_relativePath}${_kcfg}")
    endforeach()

    foreach(_ui ${OKTETA_ADD_SUBLIBRARY_UI})
        ki18n_wrap_ui(${_SRCS} "${_relativePath}${_ui}")
    endforeach()

    if (OKTETA_ADD_SUBLIBRARY_PUBLIC)
        set(_cc_include_dir ${OKTETA_ADD_SUBLIBRARY_NAMESPACE})
        if (OKTETA_ADD_SUBLIBRARY_REVERSE_NAMESPACE_INCLUDEDIR)
            list(REVERSE _cc_include_dir)
        endif()
        string(REPLACE ";" "/" _cc_include_dir "${_cc_include_dir}")

        ecm_generate_headers(${_CCHDRS}
            HEADER_NAMES
                ${OKTETA_ADD_SUBLIBRARY_PUBLIC}
            ${_egh_relative_param}
            PREFIX ${_cc_include_dir}
            REQUIRED_HEADERS ${_HDRS}
        )
    endif()

    if (OKTETA_ADD_SUBLIBRARY_SUBDIR)
        list(APPEND ${_libname}${_sourceTag}_LIB_SRCS   ${${_SRCS}})
        list(APPEND ${_libname}_LIB_HDRS   ${${_HDRS}})
        list(APPEND ${_libname}_LIB_CCHDRS ${${_CCHDRS}})
    endif()
endmacro()


function(okteta_add_library _baseName)
    set(options
        NO_TARGET_NAMESPACE
        NO_VERSIONED_INCLUDEDIR
        NO_PACKAGE_NAMESPACED_INCLUDEDIR
        REVERSE_NAMESPACE_LIB
        REVERSE_NAMESPACE_INCLUDEDIR
        LOWERCASE_LIB
    )
    set(oneValueArgs
        INTERNAL_BASENAME
        VERSION
        SOVERSION
    )
    set(multiValueArgs
        NAMESPACE
        ABIVERSION
        PUBLIC
        PRIVATE
        SOURCES
        HEADERS
        CCHEADERS
    )
    cmake_parse_arguments(OKTETA_ADD_LIBRARY "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT OKTETA_ADD_LIBRARY_INTERNAL_BASENAME)
        set(OKTETA_ADD_LIBRARY_INTERNAL_BASENAME ${_baseName})
    endif()
    string(CONCAT _namespacePrefix ${OKTETA_ADD_LIBRARY_NAMESPACE})
    if (OKTETA_ADD_LIBRARY_ABIVERSION)
        list(LENGTH OKTETA_ADD_LIBRARY_ABIVERSION _abiversionCount)
        list(LENGTH OKTETA_ADD_LIBRARY_NAMESPACE _namespaceCount)
        if (NOT _abiversionCount EQUAL _namespaceCount)
            message(FATAL_ERROR "ABIVERSION needs to have as many elements as NAMESPACE if used")
        endif()
        math(EXPR _namespaceLastIndex "${_namespaceCount} - 1")

        set(_versioned_namespace)
        foreach(val RANGE ${_namespaceLastIndex})
            list(GET OKTETA_ADD_LIBRARY_NAMESPACE ${val} _namespace)
            list(GET OKTETA_ADD_LIBRARY_ABIVERSION ${val} _abiversion)
            list(APPEND _versioned_namespace "${_namespace}${_abiversion}")
        endforeach()
        string(CONCAT _versionedNamespacePrefix ${_versioned_namespace})
    else()
        set(_versioned_namespace ${OKTETA_ADD_LIBRARY_NAMESPACE})
        set(_versionedNamespacePrefix "${_namespacePrefix}")
    endif()

    set(_fullName "${_namespacePrefix}${_baseName}")
    set(_fullInternalName "${_namespacePrefix}${OKTETA_ADD_LIBRARY_INTERNAL_BASENAME}")

    set(_targets_export_name "${_fullName}Targets")

    set(_libraryName ${_versioned_namespace})
    if (OKTETA_ADD_LIBRARY_REVERSE_NAMESPACE_LIB)
        list(REVERSE _libraryName)
    endif()
    string(CONCAT _libraryName ${_libraryName} ${_baseName})
    if (OKTETA_ADD_LIBRARY_LOWERCASE_LIB)
        string(TOLOWER ${_libraryName} _libraryName)
    endif()

    set(_targetName ${_fullName})
    string(TOLOWER ${_fullInternalName} _lc_fullInternalName)
    if (OKTETA_ADD_LIBRARY_NO_VERSIONED_INCLUDEDIR)
        set(_cc_include_dir ${OKTETA_ADD_LIBRARY_NAMESPACE})
    else()
        set(_cc_include_dir ${_versioned_namespace})
    endif()
    if (OKTETA_ADD_LIBRARY_REVERSE_NAMESPACE_INCLUDEDIR)
        list(REVERSE _cc_include_dir)
    endif()
    string(REPLACE ";" "/" _cc_include_dir "${_cc_include_dir}")
    string(TOLOWER ${_cc_include_dir} _include_dir)

    add_library(${_targetName} SHARED ${OKTETA_ADD_LIBRARY_SOURCES} )
    if (NOT OKTETA_ADD_LIBRARY_NO_TARGET_NAMESPACE)
        add_library("${_namespacePrefix}::${_baseName}" ALIAS ${_targetName})
        set(_export_name_args EXPORT_NAME ${_baseName})
    else()
        set(_export_name_args)
    endif()

    set(_exportHeaderFilePath ${CMAKE_CURRENT_BINARY_DIR}/${_include_dir}/${_lc_fullInternalName}_export.h)
    generate_export_header(${_targetName}
        BASE_NAME ${_fullInternalName}
        EXPORT_FILE_NAME ${_exportHeaderFilePath}
    )

    target_link_libraries(${_targetName}
        PUBLIC
            ${OKTETA_ADD_LIBRARY_PUBLIC}
        PRIVATE
            ${OKTETA_ADD_LIBRARY_PRIVATE}
    )

    target_include_directories(${_targetName}
        PUBLIC "$<BUILD_INTERFACE:${${_targetName}_BINARY_DIR};${CMAKE_CURRENT_BINARY_DIR}>"
    )

    set_target_properties(${_targetName} PROPERTIES
        ${_export_name_args}
        OUTPUT_NAME ${_libraryName}
        VERSION     ${OKTETA_ADD_LIBRARY_VERSION}
        SOVERSION   ${OKTETA_ADD_LIBRARY_SOVERSION}
    )

    install( TARGETS ${_targetName}
        EXPORT ${_targets_export_name}
        ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
    )

    if (OKTETA_ADD_LIBRARY_HEADERS OR OKTETA_ADD_LIBRARY_CCHEADERS)
        if (OKTETA_ADD_LIBRARY_NO_PACKAGE_NAMESPACED_INCLUDEDIR)
            set(_include_install_dir "${KDE_INSTALL_INCLUDEDIR}")
        else()
            set(_include_install_dir "${KDE_INSTALL_INCLUDEDIR}/${_versionedNamespacePrefix}${_baseName}")
        endif()
        target_include_directories(${_targetName}
            INTERFACE "$<INSTALL_INTERFACE:${_include_install_dir}>"
        )
        install( FILES ${OKTETA_ADD_LIBRARY_HEADERS} ${_exportHeaderFilePath}
            DESTINATION "${_include_install_dir}/${_include_dir}"
            COMPONENT Devel
        )
        install( FILES ${OKTETA_ADD_LIBRARY_CCHEADERS}
            DESTINATION "${_include_install_dir}/${_cc_include_dir}"
            COMPONENT Devel
        )
    endif()
endfunction()


function(okteta_add_cmakeconfig _baseName)
    set(options
        NO_TARGET_NAMESPACE
    )
    set(oneValueArgs
        VERSION
    )
    set(multiValueArgs
        NAMESPACE
        ABIVERSION
    )
    cmake_parse_arguments(OKTETA_ADD_CMAKECONFIG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    string(CONCAT _namespacePrefix ${OKTETA_ADD_CMAKECONFIG_NAMESPACE})
    if (OKTETA_ADD_CMAKECONFIG_ABIVERSION)
        list(LENGTH OKTETA_ADD_CMAKECONFIG_ABIVERSION _abiversionCount)
        list(LENGTH OKTETA_ADD_CMAKECONFIG_NAMESPACE _namespaceCount)
        if (NOT _abiversionCount EQUAL _namespaceCount)
            message(FATAL_ERROR "ABIVERSION needs to have as many elements as NAMESPACE if used")
        endif()
        math(EXPR _namespaceLastIndex "${_namespaceCount} - 1")

        foreach(val RANGE ${_namespaceLastIndex})
            list(GET OKTETA_ADD_CMAKECONFIG_NAMESPACE ${val} _namespace)
            list(GET OKTETA_ADD_CMAKECONFIG_ABIVERSION ${val} _abiversion)
            string(APPEND _fullVersionedName ${_namespace} ${_abiversion})
        endforeach()
        string(APPEND _fullVersionedName ${_baseName})
    else()
        set(_fullVersionedName "${_namespacePrefix}${_baseName}")
    endif()

    set(_targets_export_name "${_fullVersionedName}Targets")

    # create a Config.cmake and a ConfigVersion.cmake file and install them
    set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/${_fullVersionedName}")

    if (OKTETA_ADD_CMAKECONFIG_VERSION)
        set(_configVersionFilePath "${CMAKE_CURRENT_BINARY_DIR}/${_fullVersionedName}ConfigVersion.cmake")
        if(NOT OKTETA_ADD_CMAKECONFIG_COMPATIBILITY)
            set(OKTETA_ADD_CMAKECONFIG_COMPATIBILITY AnyNewerVersion)
        endif()
        write_basic_package_version_file("${_configVersionFilePath}"
            VERSION ${OKTETA_ADD_CMAKECONFIG_VERSION}
            COMPATIBILITY ${OKTETA_ADD_CMAKECONFIG_COMPATIBILITY}
        )
    else()
        set(_configVersionFilePath)
    endif()

    set(_configFileTemplatePath "${CMAKE_CURRENT_SOURCE_DIR}/${_fullVersionedName}Config.cmake.in")
    set(_configFilePath "${CMAKE_CURRENT_BINARY_DIR}/${_fullVersionedName}Config.cmake")
    configure_package_config_file(
        "${_configFileTemplatePath}"
        "${_configFilePath}"
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    )

    install( FILES
            "${_configFilePath}"
            "${_configVersionFilePath}"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    if(NOT OKTETA_ADD_CMAKECONFIG_NO_TARGET_NAMESPACE)
        set(_namespace_args NAMESPACE "${_namespacePrefix}::")
    endif()
    install(EXPORT ${_targets_export_name}
        FILE "${_fullVersionedName}Targets.cmake"
        ${_namespace_args}
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
endfunction()