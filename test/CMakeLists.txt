
file(GLOB SRCS *.cpp)

add_executable(testapp ${SRCS})

target_link_libraries(
	testapp
	PRIVATE
	jni.hpp
	Catch2
)