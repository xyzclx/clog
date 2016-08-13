
#include "clog/clog.h"

#define BOOST_LOG_DYN_LINK 

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
    void clog_logger_##severity::flush() { \
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
        : source_file_(source_file), line_(line), is_alive_(true), enable_auto_exception_(true) {
        init_clog();
        msg_ << msg_cap;
    }

    void clog_logger_base::raise_logger_exception() const {
        if (enable_auto_exception_) {
            throw fatal_error(msg_.str(), source_file_, line_);
        }
    }

    void clog_logger_base::close() {
        if (is_alive_) {
            flush();
            is_alive_ = false;
        }
    }

    template <>
    clog_logger_base& clog_logger_base::operator<< <fatal_error>(const fatal_error &except) {
        if (except.default_error()) {
            close();
        }
        else {
            enable_auto_exception_ = false;
            close();
            throw except;
        }
        return *this;
    }

    template <>
    clog_logger_base& clog_logger_base::operator<< <std::runtime_error>(const std::runtime_error &except) {
        enable_auto_exception_ = false;
        close();
        throw except;
        return *this;
    }

    template <>
    clog_logger_base& clog_logger_base::operator<< <std::exception>(const std::exception &except) {
        enable_auto_exception_ = false;
        close();
        throw except;
        return *this;
    }

    CLOG_IMPL_LOGGER(info)
    CLOG_IMPL_LOGGER(warning)
    CLOG_IMPL_LOGGER(error)

    CLOG_IMPL_LOGGER_CON(fatal)
    void clog_logger_fatal::flush() {
        std::string sp = msg_.str().empty() ? "" : ": ";
        BOOST_LOG_TRIVIAL(fatal) << "Fatal error: " << source_file_ << " (Line " << line_ << ")" << sp << msg_.str();
        raise_logger_exception();
    }
}
