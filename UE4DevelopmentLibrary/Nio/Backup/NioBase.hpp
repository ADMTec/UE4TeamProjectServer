#pragma once
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <WinSock2.h>
#include <Windows.h>
#include <string>
#include <memory>
#include <mutex>
#include <deque>
#include <vector>
#include <cstdint>
#include <cassert>
#include <functional>
#include <shared_mutex>

#include <stdexcept>
#include <concurrent_queue.h>


#define USE_TBB
#ifdef USE_TBB
#include "tbb/scalable_allocator.h"
#define BUFFER_MALLOC(size) (char*)scalable_malloc(size)
#define BUFFER_REALLOC(size) (char*)scalable_realloc(size)
#define BUFFER_FREE(ptr) scalable_free(ptr)
#else
#include <stdlib.h>
#define BUFFER_MALLOC(size) (char*)malloc(size)
#define BUFFER_REALLOC(size) (char*)realloc(size)
#define BUFFER_FREE(ptr) free(ptr)
#endif

using task_t = std::function<void(void)>;
using acceptor_ptr_t = void*;

class NioContext;
class NioSocket;

namespace boost {
    namespace system {
        class error_code;
    }
}