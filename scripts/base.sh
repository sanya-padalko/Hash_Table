#!/bin/bash

echo -e "\n------------------------ Base version started ------------------------\n"

rm -f results/table_results.csv

g++ -g -O3 -msse4.2 main.cpp src/my_strcmp.s src/base.cpp -o bin/base

echo -e "\n-------------------------- Compilation ended -------------------------\n"

hyperfine --export-markdown results/base.md --warmup 3 --runs 7 \
    './bin/base'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 results_gen.py 1 0

echo -e "------------------------- Base version ended -------------------------\n"
