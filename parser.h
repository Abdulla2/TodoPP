#pragma once

#include "task.h"
#include <assert.h>

namespace TodoPP
{
// This class is intended to be used only from the Task class so the implementation is all here
class Parser
{
private:
	// To make sure the size of the enum won't be less than the m_counter
	enum class ParseStep : int
	{
		completion_mark,
		priority,
		completion_date,
		creation_date,
		desc,
	};
public:
	Parser(Task& task, std::size_t words_count = 0, std::size_t task_size = 0): m_task{task}, m_counter{0}, m_prependedStringSize{0}, m_wordsCount{words_count}, m_taskSize{task_size}, m_editingDesc{false} {}

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

	void desc()
	{
		m_task.m_contextTags.clear();
		m_task.m_projectTags.clear();
		m_task.m_specialTags.clear();

		m_counter = static_cast<int>(ParseStep::desc) + 1;
		m_editingDesc = true;

		for(auto word: Task::split_by(m_task.m_desc, ' '))
		{
			if(word.front() == '+')
			{
				word.remove_prefix(1);
				m_task.m_projectTags.push_back(word);
			}
			else if( word.front() == '@')
			{
				word.remove_prefix(1);
				m_task.m_contextTags.push_back(word);
			}
			else if( word.find_first_of(':') != std::string_view::npos)
			{
				auto tag{Task::split_by(word, ':')};
				m_task.m_specialTags.push_back(std::make_pair(tag[0], tag[1]));
			}
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
	bool m_editingDesc{};

};
}

