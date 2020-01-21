#include "TextFileLineReader.hpp"
#include "UE4DevelopmentLibrary/Exception.hpp"
#include <iostream>
#include <algorithm>
#include <sstream>

TextFileLineReader::~TextFileLineReader()
{
	
}

void TextFileLineReader::Parse(std::ifstream& file) noexcept
{
	std::string lines;
	while (file.eof() == false)
	{
		std::getline(file, lines);
		size_t find_result = lines.find_first_of('=');
		if (find_result != lines.npos && lines[0] != '/') {
			parse_result_.emplace(Trim(lines.substr(0, find_result)), Trim(lines.substr(find_result + 1)));
		}
		lines.clear();
	}
}

void TextFileLineReader::Initialize(const char* filename)
{
	std::ifstream file(filename);
	if (file.is_open() == false)  {
		throw StackTraceException(ExceptionType::kInvalidArgument, "FileOpenFail");
	}
	Parse(file);
	file.close();
}

std::string TextFileLineReader::Trim(const std::string& str, const std::string& whitespaces)
{
	std::string result;

	size_t begin = str.find_first_not_of(whitespaces);
	size_t end = str.find_last_not_of(whitespaces);
	if (begin != std::string::npos && end != std::string::npos) {
		result = str.substr(begin, end - begin + 1);
	}
	return result;
}
std::optional<int32_t> TextFileLineReader::GetInt32(const std::string& key) const
{
	auto iter = parse_result_.find(key);
	if (iter != parse_result_.end()) {
		try {
			return std::atoi(iter->second.c_str());
		} catch (...) {
		}
	}
	return std::nullopt;
}
std::optional<int64_t> TextFileLineReader::GetInt64(const std::string& key) const
{
	auto iter = parse_result_.find(key);
	if (iter != parse_result_.end()) {
		try {
			return std::atoi(iter->second.c_str());
		} catch (...) {
		}
	}
	return std::nullopt;
}

std::optional<std::string> TextFileLineReader::GetString(const std::string& key) const
{
	auto iter = parse_result_.find(key);
	if (iter != parse_result_.end()) {
		return iter->second;
	}
	return std::nullopt;
}