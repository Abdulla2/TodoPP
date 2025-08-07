#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include "date.h"

namespace TodoPP
{
class Task
{
public:

	using Priority = std::optional<char>;
	using Set = std::pair<std::string_view, std::string_view>;

	Task() = default;
	Task(std::string_view task, int id );
	

	void setId(int id);
	

	const int getId() const; 
	const std::vector<std::string_view> getProjects() const ; 
	const std::vector<std::string_view> getContexts() const ; 
	const std::vector<Set> getSpecialTags() const ; 
	const std::string_view getDesc() const; 
	const std::optional<char> getPriority() const; 
	const std::optional<Date> getCreationDate() const; 
	const std::optional<Date> getCompletionDate() const; 
	const bool isCompleted() const; 

	void setCompleted(); 
	void clearCompleted(); 

	void setPriority(char priority); 
	void clearPriority(); 

	void addProject(const std::string_view project);
	void addContext(const std::string_view context);
	void addTag(const std::string_view tag);

	void removeProject(const std::string_view project);
	void removeContext(const std::string_view context);
	void removeTag(const std::string_view tag);

	void setCompletionDate(Date& date);
	void setCreationDate(Date& date);
	void setCompletionDate(std::string_view);
	void setCreationDate(std::string_view );

	
	void clearCompletionDate();
	void clearCreationDate();

	void appendToDesc(std::optional<const char> ch, std::string_view str);
	void removeWordFromDesc(std::optional<const char> ch, std::string_view word);
	void descChanged();
	void editDesc(std::string_view desc);


	bool operator>(const Task& task) const;	

	friend std::istream& operator>>(std::istream& in, Task& task);
	

	friend std::ostream& operator<<(std::ostream& out, const Task& task);	
	friend class Parser;
private:
	static std::vector<std::string_view> split_by(std::string_view str, char delim);
private:
	// For now we'll define date as strings later we'll see what the STL offers for them
	// using Date = std::string;
	// Same goes here until finding about sets and maps in the STL


	int m_id{};
	bool m_isCompleted{};
	Priority m_priority{};
	std::optional<Date> m_completionDate {};
	std::optional<Date> m_creationDate {};
	std::string m_desc {};
	std::vector<std::string_view> m_projectTags{};
	std::vector<std::string_view> m_contextTags{};
	std::vector<Set> m_specialTags{};

};
}

