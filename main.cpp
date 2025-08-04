#include <cctype>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>
#include <sstream>
#include <map>
#include <charconv>


bool inputMatches(std::string_view input, std::string_view pattern, bool isMaskOk)
{
	if (input.length() != pattern.length())
	{
		return false;
	}

	// This table defines all special symbols that can match a range of user input
	// Each symbol is mapped to a function that determines whether the input is valid for that symbol
	static const std::map<char, int (*)(int)> validators{
		{ '#', &std::isdigit },
		{ '_', &std::isspace },
		{ '@', &std::isalpha },
		{ '*' , &std::isupper},
		{ '?', [](int) { return 1; } }
	};


	return std::equal(input.begin(), input.end(), pattern.begin(), [isMaskOk](char ch, char mask) -> bool {
		auto found{ validators.find(mask) };

		if (found != validators.end())
		{
			// The pattern's current element was found in the validators. Call the
			// corresponding function.
			return (*found->second)(ch) || (isMaskOk && (mask == ch));
		}

		// The pattern's current element was not found in the validators. The
		// characters have to be an exact match.
		return ch == mask;
	}); // end of lambda
}

class Date 
{
private:
	int m_day {};
	int m_month {};
	int m_year {};
public:
	static bool isDateFormat(const std::string_view date)
	{
		return inputMatches(date, "####-##-##", false);
	}
	Date() : m_day{}, m_month{}, m_year{}
	{
	}

	Date(const Date&) = default;

	Date(std::string_view date) 
	{
		if(isDateFormat(date))
		{
			std::from_chars(date.data(), date.data() + 4, m_year, 10);
			date.remove_prefix(5);
			std::from_chars(date.data(), date.data() + 2, m_month);
			date.remove_prefix(3);
			std::from_chars(date.data(), date.data() + 2, m_day);
		}
	}

	Date& operator=(std::string_view date)
	{
		auto ddate {Date{date}};
		*this = ddate;
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& out, const Date& date)
	{
		out << date.m_year << '-' << date.m_month << '-' << date.m_day;
		return out;
	}

};

// struct Completion
// {
// 	bool m_completion {};
//
// 	friend std::ostream& operator<<(std::ostream& out, const Completion& c)
// 	{
// 		if (c.m_completion)
// 		{
// 			return out << 'x';
// 		}
// 		return out;
// 	}
//
// 	Completion& 
// };
//

class Task
{
	// private:
public:
	// For now we'll define date as strings later we'll see what the STL offers for them
	// using Date = std::string;
	// Same goes here until finding about sets and maps in the STL
	using Set = std::string;

	using Priority = char;

	bool m_isCompleted{};
	Priority m_priority{};
	Date m_completionDate {};
	Date m_creationDate {};
	std::string m_desc {};
	std::vector<std::string> m_projectTags{};
	std::vector<std::string> m_contextTags{};
	std::vector<Set> m_specialTags{};


public:


	std::vector<std::string_view> split_by(std::string_view str, char delim) {
		std::vector<std::string_view> vec;
		size_t pos;

		while ((pos = str.find(delim)) != std::string_view::npos) {
			vec.push_back(str.substr(0, pos));    // part before delimiter
			str.remove_prefix(pos + 1);           // move past delimiter
		}

		if (!str.empty()) {
			vec.push_back(str);                   // last part
		}

		return vec;
	}


	Task() = default;
	Task(std::string_view task)
	{
		// if(std::islower(task.front()))
		if(task.front() == 'x')
		{
			m_isCompleted = true;
			task.remove_prefix(2)	;
		}
		else
		{
			m_isCompleted = false;
		}

		if( inputMatches(task.substr(0, 3), "(*) ", false) )
		{
			m_priority = task.at(1);
			task.remove_prefix(4);
		}



		if (Date::isDateFormat(task.substr(0,10))) 
		{
			if (m_isCompleted)
			{
				m_completionDate = {task.substr(0,10)} ;
			
				task.remove_prefix(11);

				if (Date::isDateFormat(task.substr(0,10))) 
				{
					m_creationDate = {task.substr(0,10)};
					task.remove_prefix(11);
				}
			}
			else
			{
				m_creationDate = {task.substr(0,10)} ;
				task.remove_prefix(11);
			}
		}

		m_desc = std::string{task};

		std::vector<std::string_view> words = {split_by(task, ' ')};

		for (auto word : words) // Will change word to auto but clangd gives warning
		{
			if(word.front() == '+')
			{
				m_projectTags.push_back(std::string{word.substr(1)});
			}
			else if( word.front() == '@')
			{
				m_contextTags.push_back(std::string{word.substr(1)});
			}
			else if( word.find_first_of(':') != std::string_view::npos)
			{
				m_specialTags.push_back(std::string{word});
			}

		}

	}



	friend std::ostream& operator<<(std::ostream& out, const Task& task)
	{
		out << task.m_isCompleted << " (" << task.m_priority << ") " << task.m_completionDate << " " << task.m_creationDate << " " << task.m_desc << '\n';
		return out;
	}
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	for(T str: vec)
	{
		out<<str<<' ';
	}

	return out<<'\n';
}

#include <array>
int main()
{
	// For Now will use a string
	// std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	// std::string task2 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	// std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};

	// Task taskO{task1};

	std::array tasks {
		Task{"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{" (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30 "} ,
		Task{"x (A) 2016-05-20 2016-04-30 measure   space for +chapelShelving @chapel due:2016-05-30 @python"} ,
		Task{"x 2016-05-20 2016-04-30    measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{" 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{"x (A)  measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{"measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{" measure space for +chapelShelving @chapel due:2016-05-30"} ,
		Task{"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"} ,


		//New New

		Task{"x (D) 2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},

		Task{" (B) 2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"(B) 2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},

		Task{"x  2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"x 2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{" 2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"2026-01-04 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		
		Task{"x (D) 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"x  2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"x 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"(D) 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{" (D) 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{" (D) 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{" 2025-08-05 Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		Task{"Make the best @todo program in the world in +C++ for +learning the language for @quitting from +TWI due:2026-01-01 Plan:A Type:learning"},
		// Task{task1}
		// Task{task1}


	};


	// std::cout << taskO;
	// std::cout << taskO.m_projectTags;
	// std::cout << taskO.m_specialTags;
	// std::cout << taskO.m_contextTags;


	return 0;

}
