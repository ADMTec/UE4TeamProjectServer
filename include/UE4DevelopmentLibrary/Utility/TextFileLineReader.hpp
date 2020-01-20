#pragma once
#include <cstdint>
#include <string>
#include <fstream>
#include <optional>
#include <unordered_map>

class TextFileLineReader
{
public:
	TextFileLineReader() = default;
	~TextFileLineReader();

	void Initialize(const char* filename);

	std::optional<int32_t> GetInt32(const std::string& key) const;
	std::optional<int64_t> GetInt64(const std::string& key) const;
	std::optional<std::string> GetString(const std::string& key) const;
private:
	void Parse(std::ifstream& file) noexcept;
	std::string Trim(const std::string& str, const std::string& whitespaces = " \t\r\n");
private:
	std::unordered_map<std::string, std::string> parse_result_;
};