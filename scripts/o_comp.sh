#!/bin/bash

echo -e "\n------------------------ O-comparison started ------------------------\n"

rm -f results/table_results.csv

g++ -g -O0 -msse4.2 main.cpp src/base.cpp -o bin/baseO0
g++ -g -O1 -msse4.2 main.cpp src/base.cpp -o bin/baseO1
g++ -g -O2 -msse4.2 main.cpp src/base.cpp -o bin/baseO2
g++ -g -O3 -msse4.2 main.cpp src/base.cpp -o bin/baseO3

echo -e "\n-------------------------- Compilation ended -------------------------\n"

hyperfine --export-markdown results/o_comp.md --warmup 3 --runs 7 \
    './bin/baseO0' \
    './bin/baseO1' \
    './bin/baseO2' \
    './bin/baseO3'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 proc_res.py 4 4

echo -e "------------------------- O-comparison ended -------------------------\n"
