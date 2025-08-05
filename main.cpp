#include <algorithm>
#include <cctype>
#include <iomanip>
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
#include <array>
#include <execution>
#include <optional>


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	for(T str: vec)
	{
		out<<str<<' ';
	}

	return out<<'\n';
}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::optional<T>& data)
{
	if (data) 
	{
	
		return out << data.value();
	}
	return out;
}

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

	using Priority = std::optional<char>;

	bool m_isCompleted{};
	Priority m_priority{};
	Date m_completionDate {};
	Date m_creationDate {};
	std::string m_desc {};
	std::vector<std::string> m_projectTags{};
	std::vector<std::string> m_contextTags{};
	std::vector<Set> m_specialTags{};

	std::string full_text{};

public:


	std::vector<std::string_view> split_by(std::string_view str, char delim) {
		std::vector<std::string_view> vec;
		size_t pos;

		while ((pos = str.find(delim)) != std::string_view::npos) {
			if(pos != 0)
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
		full_text = std::string{task};
		// if(std::islower(task.front()))
		auto vec = split_by(task, ' ');
		auto counter {0};

		// if(task.front() == 'x')
		if(vec[counter].front() == 'x')
		{
			counter++;
			m_isCompleted = true;
		}
		else
		{
			m_isCompleted = false;
		}

		if( inputMatches(vec[counter].substr(0, 3), "(*)", false) )
		{
			m_priority = vec[counter][1];
			counter++;
		}
		else
		{
			m_priority = std::nullopt;
		}


		if (Date::isDateFormat(vec[counter].substr(0,10))) 
		{
			if (m_isCompleted)
			{
				m_completionDate = {vec[counter].substr(0,10)} ;
				counter++;

				if (Date::isDateFormat(vec[counter].substr(0,10))) 
				{
					m_creationDate = {vec[counter].substr(0,10)};
					counter++;
				}
			}
			else
			{
				m_creationDate = {vec[counter].substr(0,10)} ;
				counter++;
			}
		}

		// m_desc = std::string{task};

		// std::vector<std::string_view> words = {split_by(task, ' ')};
		std::vector<std::string_view> words {vec.begin() + counter, vec.end()};

		for (auto word : words) // Will change word to auto but clangd gives warning
		{
			m_desc += word;
			m_desc += ' ';
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

	bool operator>(const Task& task) const
	{
		if (m_isCompleted != task.m_isCompleted) {
		
			return task.m_isCompleted;
		}

		if (m_priority)
		{
			if (!task.m_priority) 
			{
				return true;
			}

			return m_priority < task.m_priority; // First chars are Higher priority
		
		}
		else if (task.m_priority)
		{
			return false;
		}

		// Should have creation date comparison here but won't do

		return m_desc > task.m_desc;
	}


	friend std::ostream& operator<<(std::ostream& out, const Task& task)
	{
		out<<task.full_text<<'\n';
		out << task.m_isCompleted <<std::setfill('0') << std::setw(2) << " (" << task.m_priority << ") " << task.m_completionDate << " " << task.m_creationDate << " " << task.m_desc << '\n';
		out << "Context Tags: " << task.m_contextTags;
		out << "Project Tags: " << task.m_projectTags;
		out << "Special Tags: " << task.m_specialTags;
		return out;
	}
};

std::ostream& operator<<(std::ostream& out, const std::vector<Task>& vec)
{
	for(Task str: vec)
	{
		out<<str<<'\n';
	}

	return out<<'\n';
}


int main()
{
	// For Now will use a string
	// std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	// std::string task2 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	// std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};

	// Task taskO{task1};

	std::vector tasks {
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

	

	// std::sort()
	std::sort(std::execution::seq, tasks.begin(), tasks.end(), [](const Task& task1, const Task& task2)
	   {
	   // return task1.m_isCompleted > task2.m_isCompleted;
	   return task1 > task2;
	   });

	std::cout<<tasks;

	// std::cout << taskO;
	// std::cout << taskO.m_projectTags;
	// std::cout << taskO.m_specialTags;
	// std::cout << taskO.m_contextTags;


	return 0;

}
