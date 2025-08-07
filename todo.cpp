#include "todo.h"
#include <algorithm>
#include <execution>



namespace TodoPP
{
Todo::Todo(std::fstream file, int size_of_file)
{
	// Average line length of 80
	static constexpr auto average_line_size{80};
	m_tasks.reserve(size_of_file/average_line_size);
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

void Todo::add(Task& task)
{
	m_fileDirty = true;
	task.setId(m_tasks.size());
	m_tasks.push_back(task);
}

void Todo::add(std::string_view task)
{
	m_fileDirty = true;
	m_tasks.push_back({task, static_cast<int>(m_tasks.size())});

}

void remove(int id);

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

const std::vector<Task> Todo::getTasksByCreationDate(std::string_view date) const;
const std::vector<Task> Todo::getTasksByCreationDate(const Date& date) const;

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
}
