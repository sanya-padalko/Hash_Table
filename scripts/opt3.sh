#!/bin/bash

echo -e "\n----------------------- Optimization #3 started ----------------------\n"

rm -f results/table_results.csv

g++ -g -O3 -msse4.2 main.cpp src/my_strcmp.s src/opt3.cpp -o bin/opt_assem

echo -e "\n-------------------------- Compilation ended -------------------------\n"

hyperfine --export-markdown results/opt_assem.md --warmup 3 --runs 7 \
    './bin/opt_assem'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 results_gen.py 1 3

echo -e "------------------------ Optimization #3 ended -----------------------\n"
