
#include "clog/clog.h"
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/exceptions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <iostream>
#include <exception>

namespace clog {
    static bool __init = false;

    void init_clog() {
        if (!__init) {
            __init = true;

            boost::log::register_simple_formatter_factory<boost::log::trivial::severity_level, char>("Severity");
            boost::log::add_common_attributes();

            boost::log::add_console_log(std::cout,
                    boost::log::keywords::format = "[%TimeStamp%] (%Severity%): %Message%",
                    boost::log::keywords::auto_flush = true
                    )->set_filter(
                    boost::log::trivial::severity >= boost::log::trivial::warning
                );
        }
    }

    void add_file_log(const std::string &filename, bool add_datetime_suffix) {
        init_clog();

        std::string log_file = filename;
        if (add_datetime_suffix) {
            boost::posix_time::ptime time = boost::posix_time::microsec_clock::local_time();
            log_file += boost::posix_time::to_iso_string(time);
        }

        boost::log::add_file_log(log_file,
            boost::log::keywords::format = "[%TimeStamp%] (%Severity%): %Message%",
            boost::log::keywords::auto_flush = true
            );
    }


    fatal_error::fatal_error() 
        : std::runtime_error("") {
    }


    fatal_error::fatal_error(const std::string &msg) 
        : std::runtime_error(msg) {
    }

    std::ostream& operator << (std::ostream& stream, const fatal_error &err) {
        stream.flush();
        throw err;
        return stream;
    }
}
