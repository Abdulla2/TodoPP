#include <assert.h>
#include <cstddef>
#include <iostream>

#include "task.h"
#include "parser.h"

namespace TodoPP
{

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
Task::Task(std::string_view task, int id): m_id{id}
{
	auto vec  {split_by(task, ' ')};

	Parser parser{*this, vec.size(), task.size()};
	for(auto word: vec)
	{
		parser.word(word);
	}

}

void Task::setId(int id) {m_id = id;}

const int Task::getId() const { return m_id;}
const bool Task::isCompleted() const { return m_isCompleted;}
void Task::setCompleted()
{
    m_isCompleted = true;
    taskChanged();
}
void Task::clearCompleted()
{
    m_isCompleted = false;
    taskChanged();
}
void Task::toggleCompleted() {
    m_isCompleted = !m_isCompleted;
    taskChanged();
}

const std::vector<std::string_view> Task::getProjects() const { return m_projectTags;} 
const std::vector<std::string_view> Task::getContexts() const { return m_contextTags;} 
const std::vector<Task::Set> Task::getSpecialTags() const { return m_specialTags;} 
const std::string_view Task::getDesc() const { return m_desc;}
const Task::Priority Task::getPriority() const { return m_priority;}
const std::optional<Date> Task::getCreationDate() const { return m_creationDate;}
const std::optional<Date> Task::getCompletionDate() const { return m_completionDate;}

void Task::descChanged()
{
	Parser parser{*this};
	parser.desc();
	taskChanged();
}

void Task::appendToDesc(std::optional<const char> ch, std::string_view str)
{
	if(ch)
	{
		m_desc += ' ';
		m_desc += *ch;
	}
	m_desc += str;
	descChanged();
}

void Task::removeWordFromDesc(std::optional<const char> ch, std::string_view word)
{
    // auto start { m_desc.find(word) };

	for(std::size_t start{}; start != std::string::npos; start = {m_desc.find(word, start)})
	{
		if(ch)
		{
			if((start != 0) && (m_desc[start - 1] == *ch))
			{
				m_desc.erase(start - 1, word.size() + 1);
			}
		}
		else
		{
			m_desc.erase(start, word.size());
		}
		start += word.size();
	}
	descChanged();
}

void Task::editDesc(std::string_view desc)
{
	m_desc = {static_cast<std::string>(desc)};
	descChanged();
}

void Task::addProject(const std::string_view project) { appendToDesc('+', project); }
void Task::addContext(const std::string_view context){ appendToDesc('@', context); }
void Task::addTag(const std::string_view tag){ appendToDesc(std::nullopt, tag); }

void Task::removeProject(const std::string_view project) { removeWordFromDesc('+', project); }
void Task::removeContext(const std::string_view context){ removeWordFromDesc('@', context); }
void Task::removeTag(const std::string_view tag){ removeWordFromDesc(std::nullopt, tag); }


void Task::setPriority(char priority) 
{
	m_priority = priority;
	taskChanged();
}
void Task::clearPriority() 
{
	m_priority = std::nullopt;
	taskChanged();
}

void Task::setCompletionDate(Date& date)
{
	m_completionDate = {date};
	taskChanged();
}

void Task::setCreationDate(Date& date)
{
	m_creationDate = {date};
	taskChanged();
}

void Task::setCompletionDate(std::string_view date){
	m_completionDate = {date};
	taskChanged();

}
void Task::setCreationDate(std::string_view date)
{
	m_creationDate = {date};
	taskChanged();
}

void Task::clearCompletionDate() 
{
	m_completionDate = std::nullopt;
	taskChanged();
}
void Task::clearCreationDate()
{
	m_completionDate = std::nullopt;
	taskChanged();
}


void Task::setTaskListener(std::function<void(Task&)> listener)
{
	m_listener = {listener};
}

void Task::taskChanged()
{
    m_listener(*this);
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

