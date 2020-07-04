cmake_minimum_required(VERSION 3.0)
project(ds_engine)

# disable ZERO_CHECK project generation
set(CMAKE_SUPPRESS_REGENERATION true)

set(physfs_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/physfs-2.0.3/include)
set(lua_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/lua-1.5/include)
set(jsonjsmn_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/JSON-jsmn/include)
set(fw1fontwrapper_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/FW1FontWrapper/include)
set(dx11sdk_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx11sdk/Include)
set(dx9sdk_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/dx9sdk/Include)
set(cegui_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/CEGUI-0.8.7/include/cegui-0)
set(bullet_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/bullet-2.87/src)
set(assimp_include_dir ${CMAKE_SOURCE_DIR}/thirdparts/Assimp-4.1.0/include)

add_subdirectory(drawspace_core)



