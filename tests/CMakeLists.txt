
add_executable(compare compare.cpp)
INSTALL(TARGETS compare DESTINATION bin)

if(WIN32)
  set(COMPARE_EXE_NAME compare.exe)
else()
  set(COMPARE_EXE_NAME compare)
endif()

# these tests should pass

foreach(ID RANGE 6)
add_test(NAME test${ID}
COMMAND ${COMPARE_EXE_NAME} ${CMAKE_SOURCE_DIR}/tests/test${ID}-0.wav ${CMAKE_SOURCE_DIR}/tests/output${ID}.wav
)
endforeach(ID)
