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





std::ostream& operator<<(std::ostream& out, const std::vector<Task>& vec)
{

	for(Task str: vec)
	{
		out<<str<<'\n';
	}

	return out<<'\n';
}


int main(int argc, const char* argv[])
{
	
	std::string_view file_name{};
	if (argc > 1)
	{
		file_name = argv[1];
	}
	else
	{
		file_name = "todo.txt";
	}
	// std::fstream file{"todo.txt",std::ios::in | std::ios::out};

	auto todo {Todo{file_name, 200}};
	// auto todo {Todo(std::move(file), 0)};

	std::cout << todo.getAllTasks();
	return 0;
}
