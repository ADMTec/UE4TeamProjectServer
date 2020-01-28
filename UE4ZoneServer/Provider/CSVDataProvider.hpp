#pragma once
#include "UE4DevelopmentLibrary/Utility/TSingleton.hpp"
#include <concurrent_unordered_map.h>
#include <cstdint>
#include "CSV/EquipTableData.hpp"
#include "CSV/MonsterTableData.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include "UE4DevelopmentLibrary/Exception.hpp"
#include <boost/algorithm/string.hpp>

namespace fs = std::filesystem;

using concurrency::concurrent_unordered_map;

template<typename T>
class CSVDataProvider : public TSingleton<CSVDataProvider<T>>
{
    friend class TSingleton<CSVDataProvider<T>>;
    CSVDataProvider() {}
public:
    void Initialize() {
        auto data_path = fs::current_path();
        data_path /= "Data";
        data_path /= T::file_name_;
        std::ifstream file(data_path);
        if (file.is_open() == false) {
            std::stringstream ss;
            ss << data_path;
            throw StackTraceException(ExceptionType::kFileOpenFail, ss.str().c_str());
        }
        std::vector<std::string> variable;
        std::string input_line;
        std::getline(file, input_line); // 이름 버리기용
        while (true)
        {
            if (input_line.empty() || file.eof()) {
                break;
            }
            std::getline(file, input_line);
            boost::split(variable, input_line, boost::is_any_of(","));

            T data;
            data.Read(variable);
            table_.insert(std::make_pair(data.GetID(), data));
        }
    }

    T* GetData(int32_t id) {
        auto iter = table_.find(id);
        if (iter != table_.end()) {
            return  &iter->second;
        }
        return nullptr;
    }
    std::vector<T> GetAll() const {
        std::vector<T> vec;
        vec.reserve(table_.size());
        for (const auto& element : table_) {
            vec.emplace_back(element.second);
        }
        return vec;
    }
   
private:
    concurrent_unordered_map<int32_t, T> table_;
};