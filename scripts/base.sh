#!/bin/bash

echo -e "\n------------------------ Base version started ------------------------\n"

rm -f results/table_results.csv

g++ -g -O3 -msse4.2 main.cpp src/my_strcmp.s src/base.cpp -o bin/base

echo -e "\n-------------------------- Compilation ended -------------------------\n"

hyperfine --export-markdown results/base.md --warmup $2 --runs $1 \
    './bin/base'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 results_gen.py 1 0 $1 $2

echo -e "------------------------- Base version ended -------------------------\n"
