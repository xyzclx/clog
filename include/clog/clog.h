
#pragma once 

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#include <ostream>
#include <exception>
#include <iostream>

#define CLOG(severity)      clog::init_clog(); CLOG##_##severity
#define CLOG_INFO           BOOST_LOG_TRIVIAL(info)
#define CLOG_WARNING        BOOST_LOG_TRIVIAL(warning)
#define CLOG_ERROR          BOOST_LOG_TRIVIAL(error)
#define CLOG_FATAL          BOOST_LOG_TRIVIAL(fatal) \
                                << "Fatal Error: " << __FILE__ \
                                << " (" << __LINE__ << "): "

namespace clog {
    void init_clog();
    void add_file_log(const std::string &filename, bool add_datetime_suffix = true);

    
    class fatal_error : public std::runtime_error {
    public:
        fatal_error();
        fatal_error(const std::string &msg); 

        friend std::ostream& operator << (std::ostream& stream, const fatal_error &err);
    };

    class Test {
    public:
        class Data {
        public:
            ~Data() {
                std::cout << "End" << std::endl;
            }
        };
        
        template <class T>
        Test& operator << (const T &t) {
            if (!this->data_) {
                this->data_.reset(new Data());
            }
            std::cout << t;
            return *this;
        }

    private:
        boost::shared_ptr<Data> data_;
    };
}


