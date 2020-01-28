#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <variant>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace fs = std::filesystem;
using primitive_t = std::variant<int32_t, int64_t, float, double, std::string>;


const char* GetTypeString(const std::string& str) 
{
    if (str == "") {
        return "int32_t";
    }
    try {
        int32_t b = boost::lexical_cast<int32_t>(str);
        return "int32_t";
    } catch (const std::exception & e) {

    }
    try {
        float b = boost::lexical_cast<float>(str);
        return "float";
    } catch (const std::exception & e) {

    }
    return "std::string";
}

std::string MakeHeaderFile(const std::string& file_name, const std::vector<std::string>& variable_name, const std::vector<std::string>& variable)
{
    std::stringstream ss;
    ss << "#pragma once" << '\n';
    ss << "#include <cstdint>" << '\n';
    ss << "#include <string>" << '\n';
    ss << "#include <vector>" << '\n';
    ss << '\n';
    ss << '\n';
    ss << "class " << file_name << "Data" << '\n';
    ss << "{" << '\n';
    ss << "public:" << '\n';
    ss << "    " << file_name << "Data() = default;" << '\n';
    ss << "    " << "~" << file_name << "Data() = default;" << '\n';
    ss << "public:" << '\n';
    ss << "    static constexpr const char* file_name_ = \"" << file_name << ".csv\";" << '\n';
    //ss << "    virtual void Write(OutputStream& out) const override;" << '\n';
    ss << "    void Read(const std::vector<std::string>& input);" << '\n';
    ss << "public:" << '\n';
    for (size_t i = 0; i < variable_name.size(); ++i) 
    {
        auto type_str = GetTypeString(variable[i]);
        ss << "    ";
        if (type_str == "std::string") {
            ss << "const " << type_str << "&";
        } else {
            ss << type_str;
        }
        ss << " Get" << variable_name[i] << "() const;" << "\n";
        ss << "    void Set" << variable_name[i] << "("; 
        if (type_str == "std::string") {
            ss << "const " << type_str << "&";
        } else {
            ss << type_str;
        }
        ss << " value);" << "\n";
    }
    ss << "private:" << '\n';
    for (size_t i = 0; i < variable_name.size(); ++i) {
        ss << "    " << GetTypeString(variable[i]) << " " << variable_name[i] << ';' << "\n";
    }
    ss << "};" << '\n';
    return ss.str();
}

std::string MakeCppFile(const std::string& file_name, const std::vector<std::string>& variable_name, const std::vector<std::string>& variable)
{
    std::stringstream ss;

    ss << "#include \"" << file_name << "Data.hpp\"" << '\n';
    ss << "#include <boost/lexical_cast.hpp>" << '\n';
    ss << '\n';
    ss << '\n';
   /* ss << "void " << file_name << "Data::" << "Write(OutputStream& out) const" << '\n';
    ss << "{" << '\n';
    for (const auto& variable : variable_name) {
        ss << "    " << "out << " << variable << ";" << '\n';
    }
    ss << "}" << '\n';
    ss << '\n';*/

    ss << "void " << file_name << "Data::" << "Read(const std::vector<std::string>& input)" << '\n';
    ss << "{" << '\n';
    for (size_t i = 0; i < variable_name.size(); ++i) {
        auto type_str = GetTypeString(variable[i]);
        ss << "    " << variable_name[i] << " = ";
            if (type_str == "int32_t") {
                ss << "boost::lexical_cast<int32_t>(";
            } else if (type_str == "float") {
                ss << "boost::lexical_cast<float>(";
            }
            ss << "input[" << i << "]";
            if (type_str == "int32_t" || type_str == "float") {
                ss << ")";
            }
            ss << ";" << '\n';
    }
    ss << "}" << '\n';
    ss << '\n';

    for (size_t i = 0; i < variable_name.size(); ++i) {
        auto type_str = GetTypeString(variable[i]);
        ss << '\n';
        if (type_str == "std::string") {
            ss << "const " << type_str << "&";
        } else {
            ss << type_str;
        }
        ss <<  " " << file_name << "Data::Get" << variable_name[i] << "() const {" << '\n';
        ss << "    return " << variable_name[i] << ";" << '\n';
        ss << "}\n";
        ss << '\n';
        ss << "void " << file_name << "Data::Set" << variable_name[i] << "(";
        if (type_str == "std::string") {
            ss << "const " << type_str << "&";
        } else {
            ss << type_str;
        }
        ss << " value) {" << '\n';
        ss << "    " << variable_name[i] << " = value;" << '\n';
        ss << "}\n";
    }
    return ss.str();
}

void HandleCSV(fs::path& path, std::ifstream& input, fs::path& result_path)
{
    std::vector<std::string> variable_name;
    std::vector<std::string> variable;
    std::string input_line; 
    std::getline(input, input_line);
    boost::split(variable_name, input_line, boost::is_any_of(","));
    std::getline(input, input_line);
    boost::split(variable, input_line, boost::is_any_of(","));
    input.close();

    std::string file_name = path.filename().string();
    file_name.erase(file_name.end() - 4, file_name.end());

    auto header_text = MakeHeaderFile(file_name, variable_name, variable);
    std::string header_file_name = file_name + "Data.hpp";

    auto cpp_text = MakeCppFile(file_name, variable_name, variable);
    std::string cpp_file_name = file_name + "Data.cpp";

    std::ofstream of_h(result_path / header_file_name);
    if (of_h.is_open()) {
        of_h.write(header_text.c_str(), strlen(header_text.c_str()));
        of_h.close();
    }
    std::ofstream of_cpp(result_path / cpp_file_name);
    if (of_cpp.is_open()) {
        of_cpp.write(cpp_text.c_str(), strlen(cpp_text.c_str()));
        of_cpp.close();
    }
}

int main()
{
    auto current_path = fs::current_path();
    auto result_path = current_path / "CSVCodeResult";
    auto err = std::error_code();
    fs::create_directory(result_path, err);

    for (const auto& entry : fs::directory_iterator(current_path))
    {
        auto path = entry.path();
        if (path.has_extension() && (path.extension() == ".csv")) {
            std::ifstream input(path);
            if (input.is_open() == false) {
                continue;
            }
            HandleCSV(path, input, result_path);
        }
    }
    return 0;
}