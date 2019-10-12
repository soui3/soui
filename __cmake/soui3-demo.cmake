#
# 1394020320@qq.com
#

macro(GIT_DEMOS ProName)
    SET(DIRECORTY_CHECK "${CMAKE_CURRENT_SOURCE_DIR}/demos/${ProName}")
    if (NOT EXISTS ${DIRECORTY_CHECK})
        message("Git clone ${ProName}, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos" ARGS "clone https://github.com/soui3-demo/${ProName}.git")
    else()
        message("Git update ${ProName}, please wait")
        EXEC_PROGRAM("git" "${CMAKE_CURRENT_SOURCE_DIR}/demos/${ProName}" ARGS "pull")
    endif()
endmacro()

#
# 下载并编译Soui-demo中的demo
#
if (DOWNLOAD_COMPILE_DEMOS)
    message("--- Download the demos in Soui3-demo")
    find_package(Git)
    if(Git_FOUND)
        message("Git found: ${GIT_EXECUTABLE}")
        GIT_DEMOS(idoudemo)
        GIT_DEMOS(SoTool)
        GIT_DEMOS(CapScreenDemo)
        GIT_DEMOS(sinstar3)
        GIT_DEMOS(SouiActivexDemo)
        message("Git clone Complete")
        FILE(COPY ${CMAKE_CURRENT_SOURCE_DIR}/__cmake/CMakeLists.txt DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/demos/)
        ADD_SUBDIRECTORY(demos)
    else()
        message("Can't found Git, please install git first")
    endif()
endif()