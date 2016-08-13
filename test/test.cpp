
#include "clog/clog.h"
#include <iostream>

#define TEST_CHECK_OP(op1, op2, op) \
    try { \
        CHECK_##op(op1, op2); \
    } \
    catch (clog::fatal_error) { \
    }

int main() {
    clog::add_file_log("bin/abc", false);
    
    LOG(INFO) << "info" << 123;
    LOG(ERROR) << "error" << 33;
    LOG(WARNING) << "warning" << 223;
    try {
        LOG(FATAL) << "fatal" << " dfd" << 233 << clog::fatal_error() << "ddddddddddddddddddd";
    }
    catch (clog::fatal_error &err) {
        std::cout << "enter except" << std::endl;
        std::cout << err.what() << std::endl;
        std::cout << err.line() << std::endl;
        std::cout << err.source_file() << std::endl;
    }
    catch (std::exception &err) {
        std::cout << "General exception" << std::endl;
        std::cout << err.what() << std::endl;
    } 

    TEST_CHECK_OP(2, 4 - 2, EQ);
    TEST_CHECK_OP(2, 2 + 1, EQ);
    int a = 1;
    TEST_CHECK_OP(2, a - 1, EQ);
    TEST_CHECK_OP(a, 2, NE);
    TEST_CHECK_OP(a, 1, NE);
    TEST_CHECK_OP(a, 1, LT);
    TEST_CHECK_OP('c', 's', LT);
    TEST_CHECK_OP('c', 'c', LE);
    TEST_CHECK_OP('c', 'a', LE);
    TEST_CHECK_OP(std::string("abc"), std::string("ab"), GT);
    TEST_CHECK_OP(std::string("bbc"), std::string("bbc") + "d", GT);
    TEST_CHECK_OP(std::string("bbc"), std::string("bb") + "c", GT);
    TEST_CHECK_OP(std::string("bbd"), std::string("bb") + "d", GE);
    TEST_CHECK_OP(std::string("bbd"), std::string("bb") + "dd", GE);

    try {
        CHECK(a >= 3) << "asfdaf";
    }
    catch (clog::fatal_error) {
    }

    return 0;
}
