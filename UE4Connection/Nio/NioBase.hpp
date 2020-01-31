#pragma once

#define USE_BOOST_ASIO

#ifdef USE_BOOST_ASIO
#include "boost/asio.hpp"
using namespace boost;
#define error_code system::error_code
#else
#include "asio.hpp"
#define error_code std::error_code
#endif
