#pragma once

#include <Windows.h>
#include <filesystem>

template<typename T>
class Option
{
private:
	T m_value;

	static T GetOptionValue(const char* name, T defaultValue)
	{
		auto path = std::filesystem::current_path() / "BigfileLogger.ini";

		// read the value from the ini file
		char value[32];
		GetPrivateProfileStringA("Options", name, NULL, value, sizeof(value), path.string().c_str());

		return ParseOption(value, defaultValue);
	}

	static T ParseOption(const std::string& value, T defaultValue)
	{
		if (std::is_same<T, bool>())
		{
			// scuffed boolean parse
			if (value == "true" || value == "false") return value == "true";
			if (value == "1" || value == "0") return value == "1";
			return defaultValue;
		}

		throw std::invalid_argument("not implemented");
	}

public:
	Option(const char* name, T defaultValue)
	{
		m_value = GetOptionValue(name, defaultValue);
	}

	T GetValue() const noexcept
	{
		return m_value;
	}

	operator bool() const noexcept
	{
		return m_value;
	}
};