
#pragma once 

#include <string>
#include <ostream>
#include <exception>
#include <sstream>

#define LOG(severity)                   CLOG##_##severity

#define CLOG_CREATE_LOGGER(severity)    CLOG_CREATE_LOGGER_MSG(severity, "")
#define CLOG_CREATE_LOGGER_MSG(severity, msg) \
    for (clog::clog_logger_##severity __logger(__FILE__, __LINE__, msg); \
        __logger.is_alive(); __logger.close()) \
        __logger

#define CLOG_INFO                       CLOG_CREATE_LOGGER(info)
#define CLOG_WARNING                    CLOG_CREATE_LOGGER(warning)
#define CLOG_ERROR                      CLOG_CREATE_LOGGER(error)         
#define CLOG_FATAL                      CLOG_CREATE_LOGGER(fatal) 

#define CLOG_DECL_LOGGER(severity) \
    class clog_logger_##severity : public clog_logger_base { \
    public: \
        clog_logger_##severity(const std::string &source_file, int64_t line, const std::string &msg_cap = ""); \
        \
        virtual void flush(); \
    }

#define CHECK(condition) \
    if (!(condition)) \
        CLOG_CREATE_LOGGER_MSG(fatal, "Check failed: " #condition " ")

#define CHECK_EQ(op1, op2)             CHECK_OP(op1, op2, ==)
#define CHECK_NE(op1, op2)             CHECK_OP(op1, op2, !=)
#define CHECK_LT(op1, op2)             CHECK_OP(op1, op2, <)
#define CHECK_LE(op1, op2)             CHECK_OP(op1, op2, <=)
#define CHECK_GT(op1, op2)             CHECK_OP(op1, op2, >)
#define CHECK_GE(op1, op2)             CHECK_OP(op1, op2, >=)

#define CHECK_OP(op1, op2, op) \
    if (!((op1) op (op2))) \
        CLOG_CREATE_LOGGER_MSG(fatal, "Check failed: " \
            #op1 " " #op " " #op2 " (" \
            + clog::to_string(op1) + " " #op " " + clog::to_string(op2) + ") ")

#ifdef _DEBUG

#define DLOG(severity)                  LOG(severity)
#define DCHECK(condition)               CHECK(condition)
#define DCHECK_EQ(op1, op2)             CHECK_EQ(op1, op2)
#define DCHECK_NE(op1, op2)             CHECK_NE(op1, op2)
#define DCHECK_LT(op1, op2)             CHECK_LT(op1, op2)
#define DCHECK_LE(op1, op2)             CHECK_LE(op1, op2)
#define DCHECK_GT(op1, op2)             CHECK_GT(op1, op2)
#define DCHECK_GE(op1, op2)             CHECK_GE(op1, op2)

#else

#define DLOG(severity)                  clog::clog_logger_dummy()
#define DCHECK(condition)               clog::clog_logger_dummy()
#define DCHECK_EQ(op1, op2)             clog::clog_logger_dummy()
#define DCHECK_NE(op1, op2)             clog::clog_logger_dummy()
#define DCHECK_LT(op1, op2)             clog::clog_logger_dummy()
#define DCHECK_LE(op1, op2)             clog::clog_logger_dummy()
#define DCHECK_GT(op1, op2)             clog::clog_logger_dummy()
#define DCHECK_GE(op1, op2)             clog::clog_logger_dummy()

#endif


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


    class clog_logger_dummy {
    public:
        template <typename T>
        clog_logger_dummy& operator << (const T &) {
            return *this;
        }
    };

    CLOG_DECL_LOGGER(info);
    CLOG_DECL_LOGGER(warning);
    CLOG_DECL_LOGGER(error);
    CLOG_DECL_LOGGER(fatal);

    template <class T>
    std::string to_string(const T &t) {
        std::stringstream stream;
        stream << t;
        return stream.str();
    }
}


