#pragma once

#include <cctype>
#include <string_view>

class Date 
{
private:
	int m_day {};
	int m_month {};
	int m_year {};
public:
	static bool isDateFormat(const std::string_view date)
	{
		int counter{0};
		bool is_valid =  true;
		for(auto ch : date)
		{
			// Hyphen positions in Date format of ####-##-##
			if(counter == 4 || counter == 7)
			{
				if(ch != '-')
				{
					is_valid =false;
					break;
				}
			}
			else
			{
				if(!std::isdigit(ch))
				{
					is_valid = false;
					break;
				}
			}
		}
		return is_valid;
	}

	Date();

	Date(const Date&) = default;

	Date(std::string_view date);
	

	Date& operator=(std::string_view date);

	friend std::ostream& operator<<(std::ostream& out, const Date& date);

};
