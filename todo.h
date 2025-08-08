#pragma once

#include "task.h"
#include <fstream>
#include <functional>
#include <iterator>

namespace TodoPP
{

class Todo
{
public:
	Todo(std::fstream file, int size_of_file = m_defaultNumberOfbytesInFile);
	

	Todo(std::string_view file_name, int size_of_file = m_defaultNumberOfbytesInFile) :  Todo{std::fstream{std::string{file_name}}, size_of_file}
	{
	}
	void add(Task& task);
	void add(std::string_view task);
	void remove(int id);

	const std::vector<Task>& getAllTasks() const;
	const Task* getTaskById(const int id) const;

	const std::vector<Task> getTasksByProject(std::string_view project) const;

	const std::vector<Task> getTasksByTag(std::string_view tag) const;
	

	const std::vector<Task> getTasksByCreationDate(std::string_view date) const;
	const std::vector<Task> getTasksByCreationDate(const Date& date) const;
	const std::vector<Task> getCompletedTasks() const;
	
	const std::vector<Task> getUnfinishedTasks() const;
	

private:
	const std::vector<Task> getTasksBy(const std::function<bool(const Task&)> searcher) const;
	const std::vector<Task>::const_iterator getTaskPosById(int id) const;
	
	static constexpr auto m_defaultNumberOfbytesInFile{800};

	std::vector<Task> m_tasks;
	// std::unordered_map<std::string, std::shared_ptr<Task>> projectsToTasks{};
	std::fstream m_file;
	int m_currentLastId{};	
	bool m_fileDirty{false};
	
};

}
