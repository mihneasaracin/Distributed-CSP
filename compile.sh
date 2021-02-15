#!/bin/bash


pipenv run python parser_select.py "SelectTest.cpp"
OMPI_CXX=g++-8 ~/Downloads/clion-2018.2.4/bin/cmake/linux/bin/cmake  --build ~/Documents/distributed_csp/Distributed-CSP --target  main -- -j 4


# to run use : mpirun  --oversubscribe -np 2  main  1
