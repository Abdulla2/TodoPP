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

std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& vec);
std::ostream& operator<<(std::ostream& out, const std::vector<std::string_view>& vec);
void logger(int curr_line)
{
	std::cout << "Curr line:" << curr_line << "\n";
}

#define LOGGER logger(__LINE__)

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
public:
	int m_day {};
	int m_month {};
	int m_year {};
public:
	static bool isDateFormat(const std::string_view date)
	{
		return inputMatches(date, "####-##-##", false);
	}
	Date() 
	{
	}

	// Date(Date& date) 
	// {
	// LOGGER;
	// }
	// Date(Date&& date) 
	// {
	// LOGGER;
	// }
	Date(std::string_view date) 
	{
		
		LOGGER;
		if(isDateFormat(date))
		{
			// char hyphen {};
		LOGGER;
			// std::stringstream in {date};
			// in >> m_year;
			// in >> hyphen;
			// in >> m_month;
			// in >> hyphen;
			// in >> m_day;
			// std::stoi(date.substr())
			std::from_chars(date.data(), date.data() + 4, m_year, 10);
			date.remove_prefix(5);
		LOGGER;
			std::from_chars(date.data(), date.data() + 2, m_month);
		LOGGER;
		LOGGER;
		LOGGER;
			date.remove_prefix(3);
		LOGGER;
			std::from_chars(date.data(), date.data() + 2, m_day);
		LOGGER;
		LOGGER;
			std::cout << m_year << '-' << m_month << '-' << m_day;
		}
	}

	Date& operator=(std::string_view date)
	{
		// LOGGER;
		//
		auto ddate {Date{date}};
		// std::cout<<ddate;
		// LOGGER;
		*this = ddate;
		
		
		return *this;
	}

	// Date& operator=(Date& date)
	// {
	// 	LOGGER;
	// 	return date;
	//
	//
	// }

	friend std::ostream& operator<<(std::ostream& out, const Date& date)
	{
		std::cout << date.m_year;
		out << date.m_year << '-' << date.m_month << '-' << date.m_day;
		return out;
	}

};


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

#if 0
	Task(const std::string& task)
	{
		// char priority {};
		std::stringstream in {task};
		// in >> priority;
		// if(!std::isupper(priority))
		m_isCompleted = false;
		if (std::islower(task.front()))
		{
			if (task.at(1) == ' ')
			{
				char temp {};
				in >> temp;
				m_isCompleted = true;
			}
		}

		char brace1 {};
		char brace2 {};
		char priority {};

		in >> brace1 >> priority >> brace2;
		if(brace1 == '(' && brace2 == ')')
		{
			if (std::isupper(priority))
			{
				m_priority = priority;
			}
			else
		{
				throw "Wrong File Foramtiing";
			}
		}
		else
		{
			in << brace1 << priority << brace2;
		}

		std::string date {};
		in >> date;

		if (Date::isDateFormat(date)) 
		{
			m_completionDate = {date} ;

			in >> date;

			if (Date::isDateFormat(date)) 
			{
				m_creationDate = {date};
			}
			else
			{
				in << date;
			}
		}
		else
		{
			in << date;
		}
		
		std::string desc{};
		
		std::stringstream desc_stream{desc};
		std::string word{};
		while(!in.eof() && !in.fail())
		{
			// in >> desc >> ' ';



		}
		



		
	}
#endif
	
	// std::vector<std::string_view> split_by(std::string_view str, char delim)
	// {
	// 	std::vector<std::string_view> vec {};
	// 	auto pos{str.find_first_of(delim)};
	// 	while(std::string_view::npos != pos)
	// 	{
	// 		pos = str.find_first_of(delim);
	// 		vec.push_back(str.substr(0, pos));
	// 		str.remove_prefix(pos + 1);
	// 	}
	//
	// 	if (!str.empty()) {
	// 		vec.push_back(str);
	// 	}
	//
	// 	return vec;
	// }
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


	Task(std::string_view task)
	{
		LOGGER;
		if(std::islower(task.front()))
		{
			m_isCompleted = true;
			task.remove_prefix(2)	;
		}
		else
		{
			m_isCompleted = false;
		}

		LOGGER;
		if( inputMatches(task.substr(0, 3), "(*)", false) )
		{
			m_priority = task.at(1);
			task.remove_prefix(4);
		}

		LOGGER;

		
		if (Date::isDateFormat(task.substr(0,10))) 
		{
			LOGGER;
			Date customdate {};
			LOGGER;
			customdate = {task.substr(0,10)};
			LOGGER;
			m_completionDate = {task.substr(0,10)} ;
			LOGGER;
			std::cout << m_completionDate.m_year << std::endl;

			LOGGER;
			std::cout << customdate << std::endl;
			LOGGER;
			std::cout << m_completionDate;
			LOGGER;
			task.remove_prefix(11);

			if (Date::isDateFormat(task.substr(0,10))) 
			{
				m_creationDate = {task.substr(0,10)};
				task.remove_prefix(11);
			LOGGER;
			std::cout << m_creationDate;
			}
		}

		LOGGER;
		m_desc = std::string{task};

		std::vector<std::string_view> words = {split_by(task, ' ')};

		// std::cout<<static_cast<std::vector<std::string>()
		std::cout << words;
		LOGGER;
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

		LOGGER;
	}
	
	
#if 0	
	std::string getText()
	{
		// std::string currTask{};

		std::stringstream os {};

		// os << (m_isCompleted ? 'x' : ' ' )<< " (" << m_priority << ") " << m_completionDate << ' ' << m_creationDate << ' ' << m_desc << "\n";
		return os.str();
	}
#endif

	friend std::ostream& operator<<(std::ostream& out, const Task& task)
	{
		out << task.m_isCompleted << " (" << task.m_priority << ") " << task.m_completionDate << " " << task.m_creationDate << " " << task.m_desc << '\n';
		return out;
	}
};


std::ostream& operator<<(std::ostream& out, const std::vector<std::string_view>& vec)
{
	for(auto str: vec)
	{
		out<<str<<' ';
	}

	return out<<'\n';
}
std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& vec)
{
	for(auto str: vec)
	{
		out<<str<<' ';
	}

	return out<<'\n';
}
int main()
{
	// For Now will use a string
	std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	
	// std::stringstream os {task1};
	
	Task taskO {task1};
	std::cout << taskO;
	std::cout << taskO.m_projectTags;
	std::cout << taskO.m_contextTags;
	std::cout << taskO.m_specialTags;

	#if 0
	// std::ws(os);
	char isCompleted {};
	os >> isCompleted;

	if ('x' == isCompleted) 
	{
	
		taskO.m_isCompleted = true;
	}
	else
	{
		taskO.m_isCompleted = false;
	}

	char brace {};
	// os >> std::skipws >> brace >> taskO.m_priority >> brace  >> taskO.m_completionDate >> taskO.m_creationDate >>  taskO.m_desc;

	std::cout << os.str();
	std::cout << taskO.m_desc;
	std::cout << taskO.getText();
	#endif
	return 0;

}
