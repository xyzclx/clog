
#pragma once 

#include <string>
#include <ostream>
#include <exception>
#include <sstream>

#define CLOG(severity)      CLOG##_##severity

#define CLOG_CREATE_LOGGER(severity) \
    for (clog::clog_logger_##severity __logger(__FILE__, __LINE__); \
        __logger.is_alive(); __logger.close()) \
        __logger

#define CLOG_INFO           CLOG_CREATE_LOGGER(info)
#define CLOG_WARNING        CLOG_CREATE_LOGGER(warning)
#define CLOG_ERROR          CLOG_CREATE_LOGGER(error)         
#define CLOG_FATAL          CLOG_CREATE_LOGGER(fatal) 

#define CLOG_DECL_LOGGER(severity) \
    class clog_logger_##severity : public clog_logger_base { \
    public: \
        clog_logger_##severity(const std::string &source_file, int64_t line, const std::string &msg_cap = ""); \
        \
        virtual void flush(); \
    }

namespace clog {
    void init_clog();
    void add_file_log(const std::string &filename, bool add_datetime_suffix = true);

    class fatal_error : public std::runtime_error {
    public:
        fatal_error();
        fatal_error(const std::string &msg); 
        fatal_error(const std::string &msg, const std::string &source_file, int64_t line);

    public:
        const std::string& source_file() const { return source_file_; }
        int64_t line() const { return line_; }
        bool default_error() const { return default_error_; }

    protected:
        std::string source_file_;
        int64_t line_;
        bool default_error_;
    };
    
    class clog_logger_base {
    public:
        void close();

    public:
        bool is_alive() { return is_alive_; }

    public:
        template <typename T>
        clog_logger_base& operator << (const T &t) {
            msg_ << t;
            return *this;
        }

    protected:
        clog_logger_base(const std::string &source_file, int64_t line, const std::string &msg_cap = "");
        void raise_logger_exception() const;

        virtual void flush() = 0;

    protected:
        std::stringstream msg_;
        std::string source_file_;
        int64_t line_;
        bool is_alive_;
        bool enable_auto_exception_;
    };

    template <>
    clog_logger_base& clog_logger_base::operator<< <fatal_error>(const fatal_error &except);
    
    template <>
    clog_logger_base& clog_logger_base::operator<< <std::runtime_error>(const std::runtime_error &except);

    template <>
    clog_logger_base& clog_logger_base::operator<< <std::exception>(const std::exception &except);

    CLOG_DECL_LOGGER(info);
    CLOG_DECL_LOGGER(warning);
    CLOG_DECL_LOGGER(error);
    CLOG_DECL_LOGGER(fatal);
}


