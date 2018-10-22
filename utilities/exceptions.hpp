#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include<boost/assert.hpp>
#include<boost/current_function.hpp>
#include<boost/shared_ptr.hpp>
#include<exception>
#include<sstream>
#include<stdexcept>

class Error: public std::exception {
    private:
        boost::shared_ptr<std::string> _msg;
    public:
        Error(const std::string& file,
                long line,
                const std::string& function,
                const std::string& msg = "");
        ~Error() throw() {}
        const char* what() const throw();
};



#define FAIL(msg) \
    std::ostringstream ss; \
    ss << msg; \
    throw Error(__FILE__,__LINE__, \
            BOOST_CURRENT_FUNCTION,ss.str()); 

#define ASSERT(condition,msg) \
    if(!(condition)) { \
        std::ostringstream ss; \
        ss << msg; \
        throw Error(__FILE__,__LINE__, \
                BOOST_CURRENT_FUNCTION,ss.str()); \
    } 

/////////////////////////
namespace {
    std::string format(const std::string& file,
            long line,
            const std::string& function,
            const std::string& msg) {
        std::ostringstream ss;
        ss<<function<<": ";
        ss<<"\n "<<file<<"("<<line<<"): \n"<<msg;
        return ss.str();
    }

}

namespace boost { 
    void assertion_failed(char const * expr,
            char const * function,
            char const * file,
            long line){
        throw std::runtime_error(format(file,line,function,
            "Boost assertion failed: " + std::string(expr)));
    }
}

Error::Error(const std::string& file,
            long line,
            const std::string& function,
            const std::string& msg){
    _msg = boost::shared_ptr<std::string>(new std::string(
                format(file,line,function,msg)));
}

const char* Error::what() const throw () {
    return _msg->c_str();
}

#endif