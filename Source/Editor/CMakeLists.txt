cmake_minimum_required(VERSION 3.28)

set(TARGET "Editor")

add_executable(${TARGET} "")

function(volucris_engine_code FOLDER_NAME OUTPUT_LIST)
    file(GLOB PUBLIC_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Public/Engine/${FOLDER_NAME}/*.h)
    file(GLOB PRIVATE_HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER_NAME}/*.h)
    file(GLOB PRIVATE_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER_NAME}/*.cpp)
    source_group("${FOLDER_NAME}/Public" FILES ${PUBLIC_HEADER_FILES})
    source_group("${FOLDER_NAME}" FILES ${PRIVATE_HEADER_FILES} ${PRIVATE_SOURCE_FILES})

    set(ALL_FILES 
        ${PUBLIC_HEADER_FILES}
        ${PRIVATE_HEADER_FILES}
        ${PRIVATE_SOURCE_FILES}
    )

    set(${OUTPUT_LIST} ${ALL_FILES} PARENT_SCOPE)
endfunction()

volucris_engine_code(EditorCore EDITOR_CORE_CODE_FILES)
volucris_engine_code(EditorEntry EDITOR_ENTRY_CODE_FILES)
volucris_engine_code(Viewport VIEWPORT_CODE_FILES)
volucris_engine_code(MaterialEditor MATERIAL_EDITOR_CODE_FILES)
volucris_engine_code(ContentBrowser CONTENT_BROWSER_CODE_FILES)
volucris_engine_code(Common COMMON_CODE_FILES)

target_sources(${TARGET} PRIVATE 
    ${EDITOR_CORE_CODE_FILES}
    ${EDITOR_ENTRY_CODE_FILES}
    ${MATERIAL_EDITOR_CODE_FILES}
    ${VIEWPORT_CODE_FILES}
    ${CONTENT_BROWSER_CODE_FILES}
    ${COMMON_CODE_FILES}
)

target_include_directories(${TARGET} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

function (volucris_link_assimp target_name)
    target_include_directories(${target_name} PRIVATE ${VOLUCRIS_ASSIMP_DIRECTORY}/include)
    file(GLOB ASSIMP_LIBS ${VOLUCRIS_ASSIMP_DIRECTORY}/lib/*.lib)
    target_link_libraries(${target_name} PRIVATE ${ASSIMP_LIBS})
endfunction()

volucris_link_assimp(${TARGET})
target_link_libraries(${TARGET} PUBLIC Engine)
target_link_libraries(${TARGET} PRIVATE stb_image)
set_target_properties(${TARGET} PROPERTIES FOLDER "")

file(GLOB ASSIMP_DLLS ${VOLUCRIS_ASSIMP_DIRECTORY}/bin/*.dll)
install(FILES ${ASSIMP_DLLS} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
