#pragma 
#include <boost/python.hpp>
#include <concurrent_unordered_map.h>
#include <shared_mutex>
#include <string>
#include <iostream>


using concurrency::concurrent_unordered_map;

class PythonScript
{
public:
    class System {
    public:
        static constexpr const char* python_script_path = "script.";
    public:
        static void Initialize();
        static void Exit();

        template<typename... Args>
        static void Execute(const std::string& file, const std::string& py_func, Args&&... args) {
            try {
                std::string full_path = python_script_path;
                full_path.append(file);

                auto& script = script_[full_path];
                
                std::unique_lock lock(script.py_obj_guard_);
                if (script.is_initialized_ == false) {
                    script.py_obj_ = boost::python::import(full_path.c_str());
                    script.is_initialized_ = true;
                }
                lock.unlock();

                std::shared_lock lock(script.py_obj_guard_);
                script.py_obj_.attr(py_func.c_str())(std::forward<Args>(args)...);
            } catch (boost::python::error_already_set&) {
                PyErr_Print();
            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }
        template<typename... Args>
        static void ReloadExecute(const std::string& file, const std::string& py_func, Args&&... args) {
            try {
                std::string full_path = python_script_path;
                full_path.append(file);

                auto& script = script_[full_path];

                std::unique_lock lock(script.py_obj_guard_);
                if (script.is_initialized_ == false) {
                    script.py_obj_ = boost::python::import(full_path.c_str());
                    script.is_initialized_ = true;
                } else {
                    boost::python::import("imp").attr("reload")(script.py_obj_);
                }
                lock.unlock();

                std::shared_lock lock(script.py_obj_guard_);
                script.py_obj_.attr(py_func.c_str())(std::forward<Args>(args)...);
            } catch (boost::python::error_already_set&) {
                PyErr_Print();

            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }
    private:
        static concurrent_unordered_map<std::string, PythonScript> script_;
    };
private:
    mutable std::shared_mutex py_obj_guard_;
    bool is_initialized_;
    boost::python::object py_obj_;
};

#define PYTHON_PASSING_BY_REFERENCE(passed) boost::ref(passed)