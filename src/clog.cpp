
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

#define CLOG_IMPL_LOGGER_CON(severity) \
    clog_logger_##severity::clog_logger_##severity(const std::string &source_file, int64_t line, const std::string &msg_cap) \
        : clog_logger_base(source_file, line, msg_cap) { \
    }

#define CLOG_IMPL_LOGGER_DES(severity) \
    clog_logger_##severity::~clog_logger_##severity() { \
        BOOST_LOG_TRIVIAL(severity) << msg_.str(); \
    }

#define CLOG_IMPL_LOGGER(severity) \
    CLOG_IMPL_LOGGER_CON(severity) \
    CLOG_IMPL_LOGGER_DES(severity)


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
        : std::runtime_error(""), line_(-1), default_error_(true) {
    }


    fatal_error::fatal_error(const std::string &msg) 
        : std::runtime_error(msg), line_(-1), default_error_(false) {
    }

    fatal_error::fatal_error(const std::string &msg, const std::string &source_file, int64_t line) 
        : std::runtime_error(msg), source_file_(source_file), line_(line), default_error_(false) {
    }

    clog_logger_base::clog_logger_base(const std::string &source_file, int64_t line, const std::string &msg_cap)
        : source_file_(source_file), line_(line), auto_throw_exception_(false) {
        msg_ << msg_cap;
    }

    clog_logger_base::~clog_logger_base() {
        if (auto_throw_exception_) {
            raise_logger_exception();
        }
    }

    void clog_logger_base::raise_logger_exception() const {
        throw fatal_error(msg_.str(), source_file_, line_);
    }

    CLOG_IMPL_LOGGER(info)
    CLOG_IMPL_LOGGER(warning)
    CLOG_IMPL_LOGGER(error)

    clog_logger_fatal::clog_logger_fatal(const std::string &source_file, int64_t line, const std::string &msg_cap)
        : clog_logger_base(source_file, line, msg_cap) { 
        auto_throw_exception_ = true;
    }

    clog_logger_fatal::~clog_logger_fatal() {
        BOOST_LOG_TRIVIAL(fatal) << "Fatal error: " << source_file_ << " (" << line_ << "): " << msg_.str();
    }
}
