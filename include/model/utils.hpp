
#ifndef __UMO_UTILS_HPP__
#define __UMO_UTILS_HPP__

#include <stdexcept>
#include <sstream>

#define THROW_ERROR(x) do {\
    std::stringstream ss;\
    ss << x;\
    throw std::runtime_error(ss.str());\
    } while(0); \

#endif

