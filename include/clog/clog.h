
#pragma once 

#include <boost/shared_ptr.hpp>
#include <string>
#include <ostream>
#include <exception>
#include <sstream>

#define CLOG(severity)      CLOG##_##severity

#define CLOG_CREATE_LOGGER(severity) \
    clog::clog_logger_helper<clog::clog_logger_##severity>(__FILE__, __LINE__)

#define CLOG_INFO           CLOG_CREATE_LOGGER(info)
#define CLOG_WARNING        CLOG_CREATE_LOGGER(warning)
#define CLOG_ERROR          CLOG_CREATE_LOGGER(error)         
#define CLOG_FATAL          CLOG_CREATE_LOGGER(fatal) 

#define CLOG_DECL_LOGGER(severity) \
    class clog_logger_##severity : public clog_logger_base { \
    public: \
        clog_logger_##severity(const std::string &source_file, int64_t line, const std::string &msg_cap = ""); \
        \
        ~clog_logger_##severity(); \
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
        ~clog_logger_base();

        void raise_logger_exception() const;

    public:
        std::stringstream& message() { return msg_; }
        void set_auto_throw_exception(bool enabled) { auto_throw_exception_ = enabled; }

    protected:
        clog_logger_base(const std::string &source_file, int64_t line, const std::string &msg_cap = "");
        
    protected:
        std::stringstream msg_;
        std::string source_file_;
        int64_t line_;
        bool auto_throw_exception_;
    };

    CLOG_DECL_LOGGER(info);
    CLOG_DECL_LOGGER(warning);
    CLOG_DECL_LOGGER(error);
    CLOG_DECL_LOGGER(fatal);

    template <class LOGGER>
    class clog_logger_helper {
    public:
        clog_logger_helper(const std::string &source_file, int64_t line, const std::string &msg_cap = "") {
            init_clog();
            logger_.reset(new LOGGER(source_file, line, msg_cap));
        }

        template <typename T>
        clog_logger_helper<LOGGER>& operator << (const T &t) {
            logger_->message() << t; 
            return *this;
        }

        clog_logger_helper<LOGGER>& operator << (const fatal_error &err) {
            logger_->set_auto_throw_exception(false);
            
            if (err.default_error()) {
                try {
                    logger_->raise_logger_exception();
                }
                catch (const fatal_error &except) {
                    logger_.reset();
                    throw except;
                }
            }
            else {
                logger_.reset();
                throw err;
            }

            return *this;
        }

        clog_logger_helper<LOGGER>& operator << (const std::runtime_error &err) {
            logger_->set_auto_throw_exception(false);
            logger_.reset();

            throw err;
            return *this;
        }

    private:
        boost::shared_ptr<LOGGER> logger_;
    };

}


