#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#define USE_BOOST_ASIO

#ifdef USE_BOOST_ASIO
#include "boost/asio.hpp"
using namespace boost;
#define ERROR_CODE system::error_code
#else
#include "asio.hpp"
#define ERROR_CODE std::error_code
#endif
