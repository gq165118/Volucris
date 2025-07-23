# 定义自动项目函数
function(auto_add_subdirectories)
    # 获取当前目录下所有子目录
    file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*)
    
    foreach(child ${children})
        # 只处理目录
        if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
            # 检查子目录是否包含CMakeLists.txt
            if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
                message(STATUS "Including subproject: ${child}")
                add_subdirectory(${child})
            else()
                # 自动创建可执行项目
                message(STATUS "Creating auto project: ${child}")
                
                # 收集源文件
                file(GLOB_RECURSE sources 
                    ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.cpp
                    ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.h
                    ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.c
                    ${CMAKE_CURRENT_SOURCE_DIR}/${child}/*.hpp
                )
                
                if(sources)
                    # 创建可执行文件
                    add_executable(${child} ${sources})
                    
                    # 添加公共包含路径
                    target_include_directories(${child} PRIVATE
                        ${CMAKE_CURRENT_SOURCE_DIR}/${child}
                        ${VOLUCRIS_ENGINE_PRIVATE_INCLUDE}
                    )
                    target_link_libraries(${child} PRIVATE Engine glfw stb_image)
                    set_target_properties(${child} PROPERTIES FOLDER "Test")
                    
                    message(STATUS "Added executable: ${child}")
                else()
                    message(WARNING "No source files found in directory: ${child}")
                endif()
            endif()
        endif()
    endforeach()
endfunction()

# 执行自动处理
auto_add_subdirectories()