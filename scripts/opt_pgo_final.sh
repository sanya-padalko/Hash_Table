#!/bin/bash

echo -e "\n---------------- Optimization with pgo (final) started ---------------\n"

rm -f results/table_results.csv
rm -f bin/*.gcda

g++ main.cpp src/my_strcmp.s src/opt3.cpp -o bin/opt_pgo_final -fprofile-generate -O3 -msse4.2

echo -e "\n-------------------------- Compilation ended -------------------------\n"

./bin/opt_pgo_final
rm -f results/table_results.csv

echo -e "\n--------------------------- Profiling ended --------------------------\n"

g++ main.cpp src/my_strcmp.s src/opt3.cpp -o bin/opt_pgo_final -fprofile-use -O3 -msse4.2

hyperfine --export-markdown results/opt_pgo_final.md --warmup $2 --runs $1 \
    './bin/opt_pgo_final'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 results_gen.py 1 5 $1 $2

echo -e "----------------- Optimization with pgo (final) ended ----------------\n"
