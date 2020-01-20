#include "StackTraceException.hpp"
#include "../Exception.hpp"
#include <boost/stacktrace.hpp>
#include <filesystem>
#include <sstream>

namespace fs = std::filesystem;
#define USE_CUSTOM_STRING_PARSE_BOOST_STACKTRACE 1

#ifdef USE_BOOST_PROVIDE_OUTPUTSTRING
#ifndef USER_CONFIG_HPP
#define USER_CONFIG_HPP

#include <boost/stacktrace/stacktrace_fwd.hpp>

#include <iosfwd>

namespace boost {
    namespace stacktrace {

        template <class CharT, class TraitsT, class Allocator>
        std::basic_ostream<CharT, TraitsT>& do_stream_st(std::basic_ostream<CharT, TraitsT>& os, const basic_stacktrace<Allocator>& bt);

        template <class CharT, class TraitsT>
        std::basic_ostream<CharT, TraitsT>& operator<<(std::basic_ostream<CharT, TraitsT>& os, const stacktrace& bt) {
            return do_stream_st(os, bt);
        }

    }
}  // namespace boost::stacktrace
#endif // USER_CONFIG_HPP

namespace boost {
    namespace stacktrace {

        template <class CharT, class TraitsT, class Allocator>
        std::basic_ostream<CharT, TraitsT>& do_stream_st(std::basic_ostream<CharT, TraitsT>& os, const basic_stacktrace<Allocator>& bt) {
            const std::streamsize w = os.width();
            const std::size_t frames = bt.size();
            for (std::size_t i = 3; i < frames; ++i) {
                os.width(2);
                os << i - 3;
                os.width(w);
                os << "# ";
                os << bt[i].address() << " - ";
                os << bt[i].name();
                const auto& str = bt[i].source_file();
                if (!str.empty()) {
                    auto path = fs::path(str);
                    os << "(" << path.filename() << ":" << bt[i].source_line() << ")";
                };
                os << '\n';
            }
            return os;
        }

    }
}  // namespace boost::stacktrace
#elif USE_CUSTOM_STRING_PARSE_BOOST_STACKTRACE
#include <boost/algorithm/string.hpp>
#include <vector>

namespace {
    std::string GetParseString(const std::string& stack_trace, std::stringstream& ss)
    {
        std::vector<std::string> strs;
        boost::split(strs, stack_trace, boost::is_any_of("\n"));

        for (size_t i = 2; i < strs.size(); ++i)
        {
            size_t path_offset = strs[i].find("at ");
            if (path_offset == std::string::npos) {
                path_offset = strs[i].find("in ");
            }
            if (path_offset != std::string::npos)
            {
                ss << strs[i].substr(0, path_offset);
                path_offset += 3;
                size_t line_offset = strs[i].find_last_of(':');
                if (line_offset != std::string::npos) {
                    fs::path path(strs[i].substr(path_offset, line_offset - path_offset));
                    std::string fname = path.filename().string();
                    fname.erase(std::remove(fname.begin(), fname.end(), '\"'), fname.end());
                    ss << '(' << fname << &strs[i][line_offset] << ')';;
                }
                else {
                    ss << '(' << "in " << &strs[i][path_offset] << ')';
                }
            }
            ss << '\n';
        }
        return ss.str();
    }
}
#endif

const char* StackTraceException::TypeToString(ExceptionType type) const
{
    switch (type)
    {
        case ExceptionType::kNullPointer:
            return "nullpointer exception";
        case ExceptionType::kOutOfRange:
            return "out of range exception";
        case ExceptionType::kLogicError:
            return "logic error";
        case ExceptionType::kConcurrency:
            return "concurrency exception";
        case ExceptionType::kFileOpenFail:
            return "file open fail";
        case ExceptionType::kInvalidArgument:
            return "invalid argument exception";
        case ExceptionType::kOverflow:
            return "overflow exception";
        case ExceptionType::kUnderflow:
            return "underflow exception";
        case ExceptionType::kBadAlloc:
            return "bad alloc";
        case ExceptionType::kSQLError:
            return "sql exception";
        case ExceptionType::kRunTimeError:
            return "runtime error";
        default:
            return "exception";
    }
}

StackTraceException::StackTraceException(const StackTraceException& rhs)
{
    StackTraceException::operator=(rhs);
}

void StackTraceException::operator=(const StackTraceException& rhs)
{
    message_ = rhs.message_;
}

StackTraceException::StackTraceException(const char* except_type, const char* message)
{
#ifdef USE_CUSTOM_STRING_PARSE_BOOST_STACKTRACE
    std::stringstream stack_ss; stack_ss << boost::stacktrace::stacktrace();
    std::stringstream ss;
    ss << except_type << ') ' << message << '\n';
    message_ = GetParseString(stack_ss.str(), ss);
#else
    ss << boost::stacktrace::stacktrace();
    message_ = ss.str();
#endif
}

StackTraceException::StackTraceException(ExceptionType except_type, const char* message)
    : StackTraceException(TypeToString(except_type), message)
{
}


StackTraceException::~StackTraceException()
{
}

char const* StackTraceException::what() const
{
    return message_.c_str();
}