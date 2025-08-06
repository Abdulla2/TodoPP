#include <algorithm>
#include <cctype>
#include <iomanip>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>
#include <sstream>
#include <map>
#include <charconv>
#include <array>
#include <execution>
#include <optional>
#include <fstream>


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
		out <<std::setfill('0') << std::setw(2) << date.m_year << '-' ;
		out <<std::setfill('0') << std::setw(2) << date.m_month << '-';
		out <<std::setfill('0') << std::setw(2) << date.m_day;
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
private:
	// For now we'll define date as strings later we'll see what the STL offers for them
	// using Date = std::string;
	// Same goes here until finding about sets and maps in the STL
	using Set = std::string;

	using Priority = std::optional<char>;

	bool m_isCompleted{};
	Priority m_priority{};
	std::optional<Date> m_completionDate {};
	std::optional<Date> m_creationDate {};
	std::string m_desc {};
	std::vector<std::string> m_projectTags{};
	std::vector<std::string> m_contextTags{};
	std::vector<Set> m_specialTags{};
	int m_id;

	// std::string full_text{};

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
	Task(std::string_view task, int id = 0): m_id{id}
	{
		// full_text = std::string{task};
		// if(std::islower(task.front()))
		auto vec  {split_by(task, ' ')};
		auto words_n {vec.size()};
		auto counter {0};

		// if(task.front() == 'x')
		if (counter == words_n) return;
		if(vec[counter].front() == 'x')
		{
			counter++;
			m_isCompleted = true;
		}
		else
	{
			m_isCompleted = false;
		}

		if (counter == words_n) return;
		if( inputMatches(vec[counter].substr(0, 3), "(*)", false) )
		{
			m_priority = vec[counter][1];
			counter++;
		}
		else
	{
			m_priority = std::nullopt;
		}


		if (counter == words_n) return;
		if (Date::isDateFormat(vec[counter].substr(0,10))) 
		{
			if (m_isCompleted)
			{
				m_completionDate = {vec[counter].substr(0,10)} ;
				counter++;

				if (counter == words_n) return;
				if (Date::isDateFormat(vec[counter].substr(0,10))) 
				{
					m_creationDate = {vec[counter].substr(0,10)};
					counter++;
				}
				else
			{
					m_creationDate = std::nullopt;
				}
			}
			else
		{
				m_creationDate = {vec[counter].substr(0,10)} ;
				m_completionDate = std::nullopt;
				counter++;
			}
		}
		else 
	{
			m_completionDate = std::nullopt;
			m_creationDate = std::nullopt;
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

	void setId(int id)
	{
		m_id = id;
	}

	const int getId() const { return m_id;}
	const bool isCompleted() const { return m_isCompleted;}
	void setCompleted() { m_isCompleted = true;}
	void clearCompleted() { m_isCompleted = false;}

	const std::vector<std::string> getProjects() const { return m_projectTags;} 
	const std::vector<std::string> getContexts() const { return m_contextTags;} 
	const std::vector<std::string> getSpecialTags() const { return m_specialTags;} 
	const std::string_view getDesc() const { return m_desc;}

	void addProject(const std::string_view project);
	void addContext(const std::string_view context);
	void addTag(const std::string_view tag);

	void removeProject(const std::string_view project);
	void removeContext(const std::string_view context);
	void removeTag(const std::string_view tag);

	const std::optional<char> getPriority() const { return m_priority;}
	const std::optional<Date> getCreationDate() const { return m_creationDate;}
	const std::optional<Date> getCompletionDate() const { return m_completionDate;}

	void setPriority(char priority) { m_priority = priority;}
	void clearPriority() { m_priority = std::nullopt;}

	void setCompletionDate(Date& date);
	void setCreationDate(Date& date);
	void setCompletionDate(std::string_view);
	void setCreationDate(std::string_view );


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

	friend std::istream& operator>>(std::istream& in, Task& task)
	{
		std::string task_str{};
		std::getline(in, task_str);
		task = Task{task_str};
		return in;
	}

	friend std::ostream& operator<<(std::ostream& out, const Task& task)
	{
		// out<<task.full_text<<'\n';
		if(task.m_isCompleted) out << "x ";
		// out << (task.m_isCompleted ? "x " : "");

		if (task.m_priority)
		{
			out << "(" << task.m_priority << ") ";
		}
		if (task.m_completionDate)
		{
			out << task.m_completionDate << " "; 
		}
		if(task.m_creationDate)
		{
			out << task.m_creationDate << " "; 
		}
		out << task.m_desc << '\n';
		// out << "Context Tags: " << task.m_contextTags;
		// out << "Project Tags: " << task.m_projectTags;
		// out << "Special Tags: " << task.m_specialTags;
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

class Todo
{


	std::vector<Task> m_tasks{};
	// std::unordered_map<std::string, std::shared_ptr<Task>> projectsToTasks{};
	bool m_fileDirty{false};
	std::fstream m_file;
	
	const std::vector<Task> getTasksBy(const std::function<bool(const Task&)> searcher) const
	// template <typename T>
	// const std::vector<Task> getTasksBy(T searcher) const
	{
		std::vector<Task> vec{};
		vec.reserve(3);


		for(auto found {std::find_if(m_tasks.begin(), m_tasks.end(), searcher)}; found != m_tasks.end(); found = std::find_if(++found, m_tasks.end(), searcher))
		{
			vec.push_back(*found);
		}

		return vec;
	}
public:
	Todo(std::fstream file, int size_of_file = 800)
	{
		// Average line length of 80
		m_tasks.reserve(size_of_file/80);
		int curr_line {0};
		int curr_pos {0};
		
		for(Task curr_task{}; file>>curr_task;curr_line++)
		{

			// auto curr_task_ptr = m_tasks.emplace_back(std::make_shared<Task>(curr_task));
			m_tasks.push_back(curr_task);
			curr_task.setId(curr_line);
			curr_pos = file.tellg();

			// for(auto project: curr_task.getProjects())
			// {
			// 	projectsToTasks.insert_or_assign(project, curr_task_ptr);
			//
			// }

			// temp_file_tasks.push_back(curr_task);
			// lines_pos.push_back(std::pair<int,int>{i,file.tellg()});
		}

		bool file_dirty {false};
		std::sort(std::execution::seq, m_tasks.begin(), m_tasks.end(), [&file_dirty](const Task& task1, const Task& task2)
	    {
	    // return task1.m_isCompleted > task2.m_isCompleted;
	    if ( (task1 > task2) != (task1.getId() < task2.getId())) file_dirty = true;
	    return task1 > task2;
	    });
		m_fileDirty = file_dirty;
	}

	void add(Task& task)
	{
		m_fileDirty = true;
		m_tasks.push_back(task);
	}

	void add(std::string_view task)
	{
		m_fileDirty = true;
		m_tasks.push_back(task);
	}

	void remove(int id);

	const Task* getTaskById(const int id) const 
	{
		auto found = std::find_if(m_tasks.begin(), m_tasks.end(), [id](const Task& task)
	       {
	       return (task.getId() == id);
	       });
		if(found != m_tasks.end())
		{
			return &(*found);
		}
		return nullptr;
	}

	const std::vector<Task> getTasksByProject(std::string_view project) const
	{
		auto searcher = [project](const Task& task)
			    {
				auto task_projects {task.getProjects()};
				auto found = std::find(task_projects.begin(), task_projects.end(), project);
				return (found != task_projects.end());
			    };
		return getTasksBy(searcher);
	}

	const std::vector<Task> getTasksByTag(std::string_view tag) const
	{
		auto searcher = [tag](const Task& task)
			    {
				auto task_tags {task.getContexts()};
				auto found = std::find(task_tags.begin(), task_tags.end(), tag);
				return (found != task_tags.end());
			    };
		return getTasksBy(searcher);
	}

	const std::vector<Task> getTasksByCreationDate(std::string_view date) const;
	const std::vector<Task> getTasksByCreationDate(const Date& date) const;
	const std::vector<Task> getCompletedTasks() const
	{
		auto searcher = [](const Task& task)
			    {
				return task.isCompleted();
			    };
		return getTasksBy(searcher);
	}
	const std::vector<Task> getUnfinishedTasks() const
	{
		auto searcher = [](const Task& task)
			    {
				return !task.isCompleted();
			    };
		return getTasksBy(searcher);
	}
	// Adding by member wise
	// void add(prio, things)
	// {
	// 	m_file_dirty = true;
	// 	m_tasks.push_back(task);
	// }

};

int main()
{
	// For Now will use a string
	// std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	// std::string task2 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};
	// std::string task1 {"x (A) 2016-05-20 2016-04-30 measure space for +chapelShelving @chapel due:2016-05-30"};

	// Task taskO{task1};

	std::fstream file{"todo.txt",std::ios::in | std::ios::out};

	std::vector<std::pair<int,Task>> file_tasks{};
	std::vector<std::pair<int,int>> lines_pos{};

	std::vector<Task> temp_file_tasks{};

	int i{};
	for(Task curr_task{}; file>>curr_task;i++)
	{

		file_tasks.push_back(std::pair<int, Task>{i,curr_task});
		temp_file_tasks.push_back(curr_task);
		lines_pos.push_back(std::pair<int,int>{i,file.tellg()});
	}

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

	std::sort(std::execution::seq, temp_file_tasks.begin(), temp_file_tasks.end(), [](const Task& task1, const Task& task2)
	   {
	   // return task1.m_isCompleted > task2.m_isCompleted;
	   return task1 > task2;
	   });
	std::cout<<temp_file_tasks;

	// std::cout << taskO;
	// std::cout << taskO.m_projectTags;
	// std::cout << taskO.m_specialTags;
	// std::cout << taskO.m_contextTags;


	return 0;

}
