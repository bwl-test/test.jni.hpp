#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <iostream>

int main( int argc, char* argv[] ) {
    // 设置cout浮点精度及显示格式
    std::cout.precision(17);

    int result = Catch::Session().run( argc, argv );

    // global clean-up...

    return result;
}

