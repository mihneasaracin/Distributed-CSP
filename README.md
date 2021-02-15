# Distributed-CSP


## Prerequisites

- Make sure you have installed [https://github.com/jl2922/hps](https://github.com/jl2922/hps) in ./include folder (this is the serialization library that I've used)

- c++11

- tested with gcc 5 or gcc8 

- mpirun (Open MPI) 3.1.3

- python3.6 with pip

- Install python dependencies with pipenv in root folder
    ```bash
    pipenv install --dev
    ```

## How to run an example

```bash
# This will generate SelectTest_generated.cpp
$ pipenv run python3.6 parser_select.py "SelectTest.cpp"


# This will compile SelectTest_generated.cpp
$ cmake  --build $PROJECT_PATH --target  main -- -j $NR_JOBS


# Run it with mpi with 2 processes example 1
$ mpirun  --oversubscribe -np 2  main 1

```

## Status of the project

- Support for channels of every STL type, custom types not supported atm, maybe change the serialization library [https://github.com/jl2922/hps](hps) to support this. If you want to still use this you must implement some functions for custom types to make the serialization work(it is documented in the library's github)

- Need support for executing remote functions, to get rid of the if clauses for the MPI. This is pretty hard since we don't have reflection in C++. One idea is to create a map with all the functions and call the functions by their string name. I have an example in ./test_decls.py. In that script I use [https://pygccxml.readthedocs.io/en/develop/install.html](https://pygccxml.readthedocs.io/en/develop/install.html) to get information about declarations of functions, classes etc.


- All the CSP functions should work (read, write, close)

 

