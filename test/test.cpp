
#include "clog/clog.h"
#include <iostream>

int main() {
    //clog::add_file_log("abc");
    
    CLOG(INFO) << "info" << 123;
    CLOG(ERROR) << "error" << 33;
    CLOG(WARNING) << "warning" << 223;
    try {
        CLOG(FATAL) << "fatal" << " dfd" << 233 << clog::fatal_error();
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

    return 0;
}
