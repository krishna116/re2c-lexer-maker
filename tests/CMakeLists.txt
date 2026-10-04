add_executable(${PROJECT_NAME}
re2c_lexer_maker.cpp;
)

target_link_libraries(${PROJECT_NAME} PUBLIC
LexerMakerLib
Re2cLib
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# if(MSVC)
# install(FILES ${PROJECT_SOURCE_DIR}/bin/uv.dll DESTINATION bin)
# endif()

install(FILES ${PROJECT_SOURCE_DIR}/doc/usage.md DESTINATION bin)

add_executable(test01
test01.cpp
)

target_link_libraries(test01 PUBLIC
LexerMakerLib
)

add_executable(test02
test02.cpp
)

target_link_libraries(test02 PUBLIC
LexerMakerLib
)

# add_subdirectory(gui)


add_executable(test03
test03.cpp
)


add_executable(test04
test04.cpp
)

target_link_libraries(test04 PRIVATE
Re2cLib
)
