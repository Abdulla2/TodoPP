#include "todo.h"
#include <algorithm>
#include <execution>

namespace TodoPP
{
Todo::Todo(std::fstream file, int size_of_file)
{
	if(!file) throw "File Error";
	// Average line length of 80
	static constexpr auto average_line_size{80};
	m_tasks.reserve(size_of_file/average_line_size);
	int curr_line {0};
	// int curr_pos {0};

	for(Task curr_task{}; file>>curr_task;curr_line++)
	{
		m_tasks.push_back(curr_task);
		curr_task.setId(curr_line);
		curr_task.setTaskListener([&](Task& task)
			    {
			    tasksListener(task);
			    });
	}

	if(!file.eof()) throw "Error Parsing the file";

	m_currentLastId = curr_line;

	bool file_dirty {sortAndMarkChange()};
	
	file.close();
	if(file_dirty)
	{
		fileChanged();
	}
}

bool Todo::sortAndMarkChange()
{
	bool file_dirty {false};
	std::sort(std::execution::seq, m_tasks.begin(), m_tasks.end(), [&file_dirty](const Task& task1, const Task& task2)
	   {
	   // return task1.m_isCompleted > task2.m_isCompleted;
	   if ( (task1 > task2) != (task1.getId() < task2.getId())) file_dirty = true;
	   return task1 > task2;
	   });
	return file_dirty;
}

void Todo::add(Task& task)
{
	task.setId(m_currentLastId++);
	m_tasks.push_back(task);
	sortAndMarkChange();
	fileChanged();
}

void Todo::add(std::string_view task)
{
	m_tasks.push_back({task, m_currentLastId++});
	sortAndMarkChange();
	fileChanged();
}

void Todo::remove(int id)
{
	
	if((id < m_tasks.size()) && (m_tasks[id].getId() == id))
	{
		m_tasks.erase(m_tasks.begin() + id);
		return;
	}

	m_tasks.erase(getTaskPosById(id));
	sortAndMarkChange();
	fileChanged();
}

const std::vector<Task>& Todo::getAllTasks() const { return m_tasks;}

const Task* Todo::getTaskById(const int id) const 
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

const std::vector<Task>::const_iterator Todo::getTaskPosById(const int id) const 
{
	auto found = std::find_if(m_tasks.begin(), m_tasks.end(), [id](const Task& task)
			   {
			   return (task.getId() == id);
			   });

	return found;
}

const std::vector<Task> Todo::getTasksByProject(std::string_view project) const
{
	auto searcher = [project](const Task& task)
		{
			auto task_projects {task.getProjects()};
			auto found = std::find(task_projects.begin(), task_projects.end(), project);
			return (found != task_projects.end());
		};
	return getTasksBy(searcher);
}

const std::vector<Task> Todo::getTasksByTag(std::string_view tag) const
{
	auto searcher = [tag](const Task& task)
		{
			auto task_tags {task.getContexts()};
			auto found = std::find(task_tags.begin(), task_tags.end(), tag);
			return (found != task_tags.end());
		};
	return getTasksBy(searcher);
}

const std::vector<Task> Todo::getTasksByCreationDate(std::string_view date) const
{
	return getTasksByCreationDate(Date{date});
}

const std::vector<Task> Todo::getTasksByCreationDate(const Date& date) const
{
	auto searcher = [date](const Task& task)
		{
			const std::optional<Date> curr_task_date = task.getCreationDate();
			if (curr_task_date)
			{
				return date == *(curr_task_date);
			}
			return false;
		};
	return getTasksBy(searcher);
}

const std::vector<Task> Todo::getCompletedTasks() const
{
	auto searcher = [](const Task& task)
		{
			return task.isCompleted();
		};
	return getTasksBy(searcher);
}
const std::vector<Task> Todo::getUnfinishedTasks() const
{
	auto searcher = [](const Task& task)
		{
			return !task.isCompleted();
		};
	return getTasksBy(searcher);
}

const std::vector<Task> Todo::getTasksBy(const std::function<bool(const Task&)> searcher) const
{
	std::vector<Task> vec{};
	vec.reserve(3);


	for(auto found {std::find_if(m_tasks.begin(), m_tasks.end(), searcher)}; found != m_tasks.end(); found = std::find_if(++found, m_tasks.end(), searcher))
	{
		vec.push_back(*found);
	}

	return vec;
}

void Todo::flushToFile()
{
	if(m_fileDirty)
	{
		std::fstream file{m_fileName, std::ios::out | std::ios::trunc};

		if (!file)
		{
			throw "File Error";
		}
		for (auto task : m_tasks)
		{
			file << task;
		}
		file.close();

	}
	m_fileDirty = {false};
}


void Todo::fileChanged()
{
	m_fileDirty = {true};
	if(m_immediateFlush)
	{
		flushToFile();
	}
}

void Todo::tasksListener(Task& task)
{
	bool file_dirty {sortAndMarkChange()};
	if(file_dirty)
	{
		fileChanged();
	}
}

void Todo::setFileImmediateFlush()
{
	m_immediateFlush = {true};

	flushToFile();
}

void Todo::clearFileImmediateFlush()
{
	m_immediateFlush = {false};
}

}
