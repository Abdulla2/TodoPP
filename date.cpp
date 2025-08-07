#include <string_view>
#include <iostream>
#include <iomanip>
#include <charconv>
#include "date.h"





Date::Date() : m_day{}, m_month{}, m_year{}
{
}


Date::Date(std::string_view date) 
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

Date& Date::operator=(std::string_view date)
{
	auto ddate {Date{date}};
	*this = ddate;
	return *this;
}

std::ostream& operator<<(std::ostream& out, const Date& date)
{
	out <<std::setfill('0') << std::setw(2) << date.m_year << '-' ;
	out <<std::setfill('0') << std::setw(2) << date.m_month << '-';
	out <<std::setfill('0') << std::setw(2) << date.m_day;
	return out;
}





