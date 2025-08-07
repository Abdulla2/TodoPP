#include "task.h"
#include <assert.h>
#include <cctype>
#include <cstddef>
#include <iostream>
#include <utility>
#include <array>


namespace TodoPP
{
class Parser
{
private:
	enum class ParseStep : int
	{
		completion_mark,
		priority,
		completion_date,
		creation_date,
		desc,
	};
public:
	Parser(Task& task, std::size_t words_count, std::size_t task_size): m_task{task}, m_counter{0}, m_prependedStringSize{0}, m_wordsCount{words_count}, m_taskSize{task_size} {}
	void word(std::string_view str)
	{
		if(str.size() < 1) return;
		if(m_wordsCount >= m_counter) return;

		m_counter++;
		switch(static_cast<ParseStep>(m_counter))
		{
			case ParseStep::completion_mark:
				break;
			case ParseStep::priority:
				break;
			case ParseStep::completion_date:
				break;
			case ParseStep::creation_date:
				break;
			case ParseStep::desc:
				descPrepare(str);
				[[fallthrough]];
			default:
				descWord(str);
				break;
		}
	}
private:
	void completionMark(std::string_view str)
	{
		if(str.front() == 'x')
		{
			m_prependedStringSize += 2;
			m_task.m_isCompleted = true;
		}
		else
		{
			m_task.m_isCompleted = false;
		}
	}

	// @desc: Checks if string is in priority format: (A)
	constexpr inline static bool isPriority(const std::string_view str)
	{
		assert(str.size() >= 3);

		if (str[0] == '(' && str[2] == ')' && std::isupper(str[1])) return true;
		else return false;
	}

	void priority(std::string_view str)
	{
		if( isPriority(str) )
		{
			m_task.m_priority = str[1];
			m_prependedStringSize += 4;
		}
		else
		{
			m_task.m_priority = std::nullopt;
		}
	}

	std::optional<Date> date(std::string_view str)
	{
		if (Date::isDateFormat(str.substr(0,10))) 
		{
			return {str};

		}
		return std::nullopt; 
	}

	void creationDate(std::string_view str)
	{
		m_task.m_creationDate = date(str);
		m_prependedStringSize += 12;
	}

	void completionDate(std::string_view str)
	{
		if(m_task.m_isCompleted)
		{
			m_task.m_completionDate = date(str);
			m_prependedStringSize += 12;
		}
		else
		{
			m_counter++;
			creationDate(str);
		}
	}

	void descWord(std::string_view word)
	{
		auto word_size {word.size()};
		m_task.m_desc += word;
		m_task.m_desc += ' ';
		std::string_view curr_word{m_task.m_desc};

		curr_word.remove_suffix(1);
		if(word.front() == '+')
		{
			curr_word.remove_prefix(m_task.m_desc.size() - 1 - word_size);
			m_task.m_projectTags.push_back(curr_word);
		}
		else if( word.front() == '@')
		{
			curr_word.remove_prefix(m_task.m_desc.size() - 1 - word_size);
			m_task.m_contextTags.push_back(curr_word);
		}
		else if( word.find_first_of(':') != std::string_view::npos)
		{
			auto tag{Task::split_by(curr_word, ':')};
			m_task.m_specialTags.push_back(std::make_pair(tag[0], tag[1]));
		}
	}

	void descPrepare(std::string_view str)
	{
		constexpr auto reserved_future_space{12 + 2 + 8}; // 12 for completion date, 2 for completion mark and, 8 for any other arbitary additions

		m_task.m_desc.reserve(m_taskSize - m_prependedStringSize + reserved_future_space);
		m_task.m_projectTags.reserve(m_wordsCount / 3);
		m_task.m_specialTags.reserve(m_wordsCount / 3);
		m_task.m_contextTags.reserve(m_wordsCount / 3);
	}
private:
	Task& m_task;
	std::size_t m_taskSize{};
	int m_counter{};
	int m_prependedStringSize{};
	std::size_t m_wordsCount{};

};

std::vector<std::string_view> Task::split_by(std::string_view str, char delim) 
{
	std::vector<std::string_view> vec;
	vec.reserve(str.size() / 5);
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


// Parses a task in the shape of todo.txt format and saves it
Task::Task(std::string_view task, int id = 0): m_id{id}
{
	auto vec  {split_by(task, ' ')};
	auto words_n {vec.size()};
	auto counter {0};
	auto prepended_str_size{0};

	Parser parser{*this, words_n, task.size()};
	for(auto word: vec)
	{
		parser.word(word);
	}

// std::vector<std::string_view> words {vec.begin() + counter, vec.end()};
	//
	// constexpr auto reserved_future_space{12 + 2 + 8}; // 12 for completion date, 2 for completion mark and, 8 for any other arbitary additions
	//
	// m_desc.reserve(task.size() - prepended_str_size + reserved_future_space);
	// m_projectTags.reserve(words.size() / 3);
	// m_specialTags.reserve(words.size() / 3);
	// m_contextTags.reserve(words.size() / 3);
	//
	// for (auto word : words) 
	// {
	// 	auto word_size {word.size()};
	// 	m_desc += word;
	// 	m_desc += ' ';
	// 	std::string_view curr_word{m_desc};
	//
	// 	curr_word.remove_suffix(1);
	// 	if(word.front() == '+')
	// 	{
	// 		curr_word.remove_prefix(m_desc.size() - 1 - word_size);
	// 		m_projectTags.push_back(curr_word);
	// 	}
	// 	else if( word.front() == '@')
	// 	{
	// 		curr_word.remove_prefix(m_desc.size() - 1 - word_size);
	// 		m_contextTags.push_back(curr_word);
	// 	}
	// 	else if( word.find_first_of(':') != std::string_view::npos)
	// 	{
	// 		auto tag{Task::split_by(curr_word, ':')};
	// 		m_specialTags.push_back(std::make_pair(tag[0], tag[1]));
	// 	}
	// }
}

void Task::setId(int id) {m_id = id;}

const int Task::getId() const { return m_id;}
const bool Task::isCompleted() const { return m_isCompleted;}
void Task::setCompleted() { m_isCompleted = true;}
void Task::clearCompleted() { m_isCompleted = false;}

const std::vector<std::string_view> Task::getProjects() const { return m_projectTags;} 
const std::vector<std::string_view> Task::getContexts() const { return m_contextTags;} 
const std::vector<Task::Set> Task::getSpecialTags() const { return m_specialTags;} 
const std::string_view Task::getDesc() const { return m_desc;}
const Task::Priority Task::getPriority() const { return m_priority;}
const std::optional<Date> Task::getCreationDate() const { return m_creationDate;}
const std::optional<Date> Task::getCompletionDate() const { return m_completionDate;}

void Task::addProject(const std::string_view project)
{
	m_desc += " +";
}

void Task::addProject(const std::string& project)
{
	Task::addProject(std::move(project));
}

void Task::addContext(const std::string_view context);
void Task::addTag(const std::string_view tag);

void Task::removeProject(const std::string_view project);
void Task::removeContext(const std::string_view context);
void Task::removeTag(const std::string_view tag);


void Task::setPriority(char priority) { m_priority = priority;}
void Task::clearPriority() { m_priority = std::nullopt;}

void Task::setCompletionDate(Date& date){m_completionDate = {date};}
void Task::setCreationDate(Date& date){m_creationDate = {date};}

void Task::setCompletionDate(std::string_view date){m_completionDate = {date};}
void Task::setCreationDate(std::string_view date){m_creationDate = {date};}

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::optional<T>& data)
{
	if (data) 
	{

		return out << data.value();
	}
	return out;
}

bool Task::operator>(const Task& task) const
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

std::istream& operator>>(std::istream& in, Task& task)
{
	std::string task_str{};
	std::getline(in, task_str);
	task = Task{task_str};
	return in;
}

std::ostream& operator<<(std::ostream& out, const Task& task)
{
	if(task.m_isCompleted) out << "x ";

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
	return out;
}
}
