#pragma once

#include <vector>
#include <boost/filesystem/path.hpp>

namespace Parameters
{
	struct ReadFile : public boost::filesystem::path
	{
		ReadFile(const std::string& str) : boost::filesystem::path(str){}
	};
	struct WriteFile : public boost::filesystem::path
	{
		WriteFile(const std::string& file) : boost::filesystem::path(file){}
	};
	struct ReadDirectory : public boost::filesystem::path
	{
		ReadDirectory(const boost::filesystem::path& path) : boost::filesystem::path(path){}
	};
	struct WriteDirectory : public boost::filesystem::path{};

	class EnumParameter
	{
	public:
		EnumParameter()
		{
			currentSelection = 0;
		}

		void addEnum(int value, const std::string& enumeration)
		{
			enumerations.push_back(enumeration);
			values.push_back(value);
		}
		int getValue()
		{
			return values[currentSelection];
		}

		std::vector<std::string> enumerations;
		std::vector<int>         values;
		int currentSelection;
	};
}