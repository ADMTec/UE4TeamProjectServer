#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <variant>
#include <cstdint>
#include <sstream>
#include <iostream>


namespace fs = std::filesystem;
using primitive_t = std::variant<int32_t, int64_t, float, double, std::string>;
/* 이름규칙
* 변수 앞에
* n : int
* l : int64
* f : float
* d : double
* s : std::string
*/

const char* GetPrimitiveTypeNameFromPrefix(const std::string& str) 
{
    switch (str[0])
    {
        case 'n':
            return "int32_t";
        case 'l':
            return "int64_t";
        case 'f':
            return "float";
        case 'd':
            return "double";
        case 's':
            return "std::string";
        default:
            return "unknown_t";
    }
}

std::string MakeHeaderFile(const std::string& file_name, const std::vector<std::string>& variables)
{
    std::stringstream ss;
    ss << "#include <cstdint>" << '\n';
    ss << "#include <string>" << '\n';
    ss << '\n';
    ss << '\n';
    ss << "class " << file_name << " : public ISerialize" << '\n';
    ss << "{" << '\n';
    ss << "public:" << '\n';
    ss << "    " << file_name << "() = default;" << '\n';
    ss << "    " << "~" << file_name << "() = default;" << '\n';
    ss << "public:" << '\n';
    ss << "    virtual void Write(OutputStream& out) const override;" << '\n';
    ss << "    virtual void Read(InputStream& input) override;" << '\n';
    //ss << "    std::string ToString() const;" << '\n';
    ss << "private:" << '\n';
    for (const auto& variable : variables) {
        ss << "    " << GetPrimitiveTypeNameFromPrefix(variable) << " " << variable << ';' << "\n";
    }
    ss << "};" << '\n';
    return ss.str();
}

std::string MakeCppFile(const std::string& file_name, const std::vector<std::string>& variables)
{
    std::stringstream ss;

    ss << "#include \"" << file_name << ".h\"" << '\n';
    ss << '\n';
    ss << '\n';
    ss << "void " << file_name << "::" << "Write(OutputStream& out) const" << '\n';
    ss << "{" << '\n';
    for (const auto& variable : variables) {
        ss << "    " << "out << " << variable << ";" << '\n';
    }
    ss << "}" << '\n';
    ss << '\n';

    ss << "void " << file_name << "::" << "void Read(InputStream& input)" << '\n';
    ss << "{" << '\n';
    for (const auto& variable : variables) {
        ss << "    " << "input >> " << variable << ";" << '\n';
    }
    ss << "}" << '\n';
    return ss.str();
}

void HandleCSV(fs::path& path, std::ifstream& input, fs::path& result_path)
{
    std::vector<std::string> variables;
    std::string input_line; 
    std::getline(input, input_line);
    boost::split(variables, input_line, boost::is_any_of(","));
    input.close();

    std::string file_name = path.filename().string();
    file_name.erase(file_name.end() - 4, file_name.end());

    auto header_text = MakeHeaderFile(file_name, variables);
    std::string header_file_name = file_name + ".h";

    auto cpp_text = MakeCppFile(file_name, variables);
    std::string cpp_file_name = file_name + ".cpp";

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