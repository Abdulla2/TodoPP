#include <iostream>
#include <ostream>
#include <string_view>
#include <vector>
#include "task.h"
#include "todo.h"


template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
{
	for(T str: vec)
	{
		out<<str<<' ';
	}

	return out<<'\n';
}



std::ostream& operator<<(std::ostream& out, const std::vector<TodoPP::Task>& vec)
{

	for(TodoPP::Task str: vec)
	{
		out<<str;
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

	try {
	
	auto todo {TodoPP::Todo{file_name, 200}};

	todo.add("(B) Do this Most important thing @test +project");
	
		todo.flushToFile();


	// auto todo {Todo(std::move(file), 0)};

	std::cout << todo.getAllTasks();
	} catch (const char*) {
		std::cout << "Exception" ;
		
	
	}
	return 0;
}
