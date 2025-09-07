# TodoPP 

This is a Todo manager library based on the syntax and structure of [todo.txt](http://todotxt.org/), it stores and manages the items in a todo.txt file.

There are two main classes that you should include to interface with this library the Todo class and the Task class, they are both in TodoPP namespace, you can see a simple usage of the classes in the main.cpp in src directory.

There is a reference qt gui implementation at https://github.com/Abdulla2/TodoPP_GUI .

## Build

The build is simple just run cmake in the directory.

```
cmake .
```

## Todo(Wink Wink)

* Change the vector in Todo class of tasks to be a vector of shared ptrs and see a way to make the memory also contigous for fast accessing all the tasks.

* Support search.

* Support different ways for sorting.

* Support more special tags.

* Maybe support plugins through something like Lua, an overkill maybe but to support any special tag or search you want.

* Maybe support multiple users accessing the interface simulatanesly to enable the program in the future to transform into a project managemetn solution.

* Add unit testing.

* Add a config support.

* Add an option for being independent and not depending on the Std lib.

* Much more.

