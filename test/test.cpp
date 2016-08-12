
#include "clog/clog.h"

int main() {
    //clog::add_file_log("abc");
    
    clog::Test() << "abc" << 234;

    CLOG(INFO) << "info";
    CLOG(ERROR) << "error";
    CLOG(WARNING) << "warning";
    try {
        CLOG(FATAL) << "fatal" << clog::fatal_error();
    }
    catch (std::runtime_error &err) {
    }

    return 0;
}
