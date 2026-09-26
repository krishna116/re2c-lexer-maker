add_executable(${PROJECT_NAME}
re2c_lexer_maker.cpp;
)

target_link_libraries(${PROJECT_NAME} PUBLIC
LexerMakerLib
)

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
