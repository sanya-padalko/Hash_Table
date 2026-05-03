#!/bin/bash

echo -e "\n----------------------- Optimization #2 started ----------------------\n"

rm -f results/table_results.csv

g++ -g -O3 -msse4.2 main.cpp src/my_strcmp.s src/opt2.cpp -o bin/opt_crc32

echo -e "\n-------------------------- Compilation ended -------------------------\n"

hyperfine --export-markdown results/opt_crc32.md --warmup 3 --runs 7 \
    './bin/opt_crc32'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 results_gen.py 1 2

echo -e "------------------------ Optimization #2 ended -----------------------\n"
