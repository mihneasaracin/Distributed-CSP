#!/bin/bash


pipenv run python parser_select.py "main.cpp"
OMPI_CXX=g++-8 cmake  --build ~/Documents/distributed_csp/Distributed-CSP --target  main -- -j 4


# to run use : mpirun  --oversubscribe -np 2  main  1
