
set(SRCS main.cpp native_method.cpp get_field_builder.cpp get_field_functions.cpp)

add_executable(testapp ${SRCS})

target_link_libraries(
	testapp
	PRIVATE
	jni.hpp
	Catch2
)