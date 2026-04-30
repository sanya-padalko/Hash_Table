#!/bin/bash

echo -e "\n----------------------- Optimization #1 started ----------------------\n"

rm -f results/table_results.csv

g++ -g -O2 -msse4.2 main.cpp my_strcmp.s src/opt1.cpp -o bin/opt_strcmp

echo -e "\n-------------------------- Compilation ended -------------------------\n"

hyperfine --export-markdown results/opt_strcmp.md --warmup 3 --runs 7 \
    './bin/opt_strcmp'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 proc_res.py 1

echo -e "------------------------ Optimization #1 ended -----------------------\n"
