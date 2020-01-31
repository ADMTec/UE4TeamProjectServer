#pragma 
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include <boost/python.hpp>
#include <unordered_map>
#include <shared_mutex>
#include <string>
#include <iostream>



class PythonScript
{
public:
    enum class Path {
        kDefault,
        kZone,
        kQuest,
        kNpc,
        kCharacter,
        kMonster,
        kItem,
    };
    class Engine : public TSingleton<Engine>
    {
        friend class TSingleton<Engine>;
        Engine() {
            Py_Initialize();
        }
    public:
        ~Engine() {
            Py_Exit(0);
        }
        template<typename... Args>
        void Execute(Path path, const std::string& file, const std::string& py_func, Args&&... args) {
            try {
                std::string full_path = GetScriptPath(path);
                full_path.append(file);

                PythonScript* script = nullptr;
                {
                    std::lock_guard lock(script_guard_);
                    script = &script_[full_path];
                }
                {
                    std::unique_lock lock(script->py_obj_guard_);
                    if (script->is_initialized_ == false) {
                        script->py_obj_ = boost::python::import(full_path.c_str());
                        script->is_initialized_ = true;
                    }
                    lock.unlock();
                }
                std::shared_lock lock(script->py_obj_guard_);
                script->py_obj_.attr(py_func.c_str())(std::forward<Args>(args)...);
            } catch (boost::python::error_already_set&) {
                PyErr_Print();
            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }
        template<typename... Args>
        void ReloadExecute(Path path, const std::string& file, const std::string& py_func, Args&&... args) {
            try {
                std::string full_path = GetScriptPath(path);
                full_path.append(file);
                std::cout << "fullpath: " << full_path << std::endl;

                PythonScript* script = nullptr;
                {
                    std::lock_guard lock(script_guard_);
                    script = &script_[full_path];
                }
                {
                    std::unique_lock lock(script->py_obj_guard_);
                    if (script->is_initialized_ == false) {
                        script->py_obj_ = boost::python::import(full_path.c_str());
                        script->is_initialized_ = true;
                    } else {
                        boost::python::import("imp").attr("reload")(script->py_obj_);
                    }
                    lock.unlock();
                }
                std::shared_lock lock(script->py_obj_guard_);
                script->py_obj_.attr(py_func.c_str())(std::forward<Args>(args)...);
            } catch (boost::python::error_already_set&) {
                PyErr_Print();

            } catch (const std::exception & e) {
                std::cout << e.what() << std::endl;
            }
        }
    private:
        static constexpr const char* GetScriptPath(Path path) {
            switch (path)
            {
                case PythonScript::Path::kZone:
                    return "script.zone.";
                case PythonScript::Path::kQuest:
                    return "script.quest.";
                case PythonScript::Path::kNpc:
                    return "script.npc.";
                case PythonScript::Path::kCharacter:
                    return "script.chr.";
                case PythonScript::Path::kMonster:
                    return "script.mob.";
                case PythonScript::Path::kItem:
                    return "script.item.";
                default:
                    return "script.";
            }
        }
    private:
        std::mutex script_guard_;
        std::unordered_map<std::string, PythonScript> script_;
    };
private:
    mutable std::shared_mutex py_obj_guard_;
    bool is_initialized_;
    boost::python::object py_obj_;
};

#define PYTHON_PASSING_BY_REFERENCE(passed) boost::ref(passed)