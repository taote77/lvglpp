# Asset pack / directory staging for application targets.
#
# Capture the top-level paths now: subdirectories call project(), which resets
# PROJECT_SOURCE_DIR and PROJECT_BINARY_DIR.
set(LVGLPP_ASSET_ROOT   "${CMAKE_SOURCE_DIR}/assets")
set(LVGLPP_ASSET_OUTPUT "${EXECUTABLE_OUTPUT_PATH}")

set(LVGLPP_ASSET_MODE "pack" CACHE STRING "Asset delivery mode: pack, directory or both")
set_property(CACHE LVGLPP_ASSET_MODE PROPERTY STRINGS pack directory both)

if(NOT LVGLPP_ASSET_MODE MATCHES "^(pack|directory|both)$")
    message(FATAL_ERROR "LVGLPP_ASSET_MODE must be pack, directory or both (got '${LVGLPP_ASSET_MODE}')")
endif()

set(_lvglpp_common_files "")
file(GLOB_RECURSE _lvglpp_common_files CONFIGURE_DEPENDS "${LVGLPP_ASSET_ROOT}/common/*")

# TTF assets are only needed by the optional FreeType backend. Skip them both
# in the pack contents and in the dependency/stamp list so changing a TTF does
# not trigger a repack when FreeType is disabled.
set(_lvglpp_common_pack_excludes "")
if(NOT LVGLPP_ENABLE_FREETYPE)
    list(FILTER _lvglpp_common_files EXCLUDE REGEX "/fonts/[^/]*[.]ttf$")
    set(_lvglpp_common_pack_excludes --exclude "fonts/*.ttf")
endif()

# A configure-time list stamp makes deleted/renamed assets invalidate the pack.
set(_lvglpp_common_stamp "${CMAKE_BINARY_DIR}/lvglpp_common_assets.stamp")
file(WRITE "${_lvglpp_common_stamp}" "${_lvglpp_common_files}")

if(LVGLPP_ASSET_MODE MATCHES "pack|both")
    add_custom_command(
        OUTPUT  "${LVGLPP_ASSET_OUTPUT}/common.pak"
        COMMAND $<TARGET_FILE:respack> pack "${LVGLPP_ASSET_ROOT}/common" "${LVGLPP_ASSET_OUTPUT}/common.pak" ${_lvglpp_common_pack_excludes}
        DEPENDS respack "${_lvglpp_common_stamp}" ${_lvglpp_common_files}
        COMMENT "Packing common assets"
        VERBATIM)
    add_custom_target(lvglpp_common_pak DEPENDS "${LVGLPP_ASSET_OUTPUT}/common.pak")
    add_dependencies(lvglpp_common_pak respack)
endif()

if(LVGLPP_ASSET_MODE STREQUAL "directory")
    # Avoid a stale pack from a previous pack-mode build shadowing the
    # directory source at runtime.
    file(REMOVE "${LVGLPP_ASSET_OUTPUT}/common.pak")
endif()

if(LVGLPP_ASSET_MODE MATCHES "directory|both")
    set(_lvglpp_common_dir_stamp "${CMAKE_BINARY_DIR}/lvglpp_common_asset.stamp")
    add_custom_command(
        OUTPUT  "${_lvglpp_common_dir_stamp}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${LVGLPP_ASSET_OUTPUT}/common.asset"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${LVGLPP_ASSET_ROOT}/common" "${LVGLPP_ASSET_OUTPUT}/common.asset"
        COMMAND ${CMAKE_COMMAND} -E touch "${_lvglpp_common_dir_stamp}"
        DEPENDS "${_lvglpp_common_stamp}" ${_lvglpp_common_files}
        COMMENT "Staging common assets"
        VERBATIM)
    add_custom_target(lvglpp_common_asset DEPENDS "${_lvglpp_common_dir_stamp}")
endif()

# Attach the common sources plus an optional assets/apps/<target>/ directory.
function(lvglpp_attach_assets target)
    if(TARGET lvglpp_common_pak)
        add_dependencies(${target} lvglpp_common_pak)
    endif()
    if(TARGET lvglpp_common_asset)
        add_dependencies(${target} lvglpp_common_asset)
    endif()

    set(_app_dir "${LVGLPP_ASSET_ROOT}/apps/${target}")
    if(NOT EXISTS "${_app_dir}")
        return()
    endif()

    if(LVGLPP_ASSET_MODE STREQUAL "directory")
        file(REMOVE "${LVGLPP_ASSET_OUTPUT}/${target}.pak")
    endif()

    file(GLOB_RECURSE _app_files CONFIGURE_DEPENDS "${_app_dir}/*")
    set(_app_pack_excludes "")
    if(NOT LVGLPP_ENABLE_FREETYPE)
        list(FILTER _app_files EXCLUDE REGEX "/fonts/[^/]*[.]ttf$")
        set(_app_pack_excludes --exclude "fonts/*.ttf")
    endif()
    set(_app_stamp "${CMAKE_BINARY_DIR}/${target}_assets.stamp")
    file(WRITE "${_app_stamp}" "${_app_files}")

    if(LVGLPP_ASSET_MODE MATCHES "pack|both")
        add_custom_command(
            OUTPUT  "${LVGLPP_ASSET_OUTPUT}/${target}.pak"
            COMMAND $<TARGET_FILE:respack> pack "${_app_dir}" "${LVGLPP_ASSET_OUTPUT}/${target}.pak" ${_app_pack_excludes}
            DEPENDS respack "${_app_stamp}" ${_app_files}
            COMMENT "Packing ${target} assets"
            VERBATIM)
        add_custom_target(${target}_pak DEPENDS "${LVGLPP_ASSET_OUTPUT}/${target}.pak")
        add_dependencies(${target}_pak respack)
        add_dependencies(${target} ${target}_pak)
    endif()

    if(LVGLPP_ASSET_MODE MATCHES "directory|both")
        set(_app_dir_stamp "${CMAKE_BINARY_DIR}/${target}_asset.stamp")
        add_custom_command(
            OUTPUT  "${_app_dir_stamp}"
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${LVGLPP_ASSET_OUTPUT}/${target}.asset"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${_app_dir}" "${LVGLPP_ASSET_OUTPUT}/${target}.asset"
            COMMAND ${CMAKE_COMMAND} -E touch "${_app_dir_stamp}"
            DEPENDS "${_app_stamp}" ${_app_files}
            COMMENT "Staging ${target} assets"
            VERBATIM)
        add_custom_target(${target}_asset DEPENDS "${_app_dir_stamp}")
        add_dependencies(${target} ${target}_asset)
    endif()
endfunction()
