#!/bin/bash

echo -e "\n-------------------- Optimization with pgo started -------------------\n"

rm -f results/table_results.csv
rm -f bin/*.gcda

g++ main.cpp src/base.cpp -o bin/opt_pgo -fprofile-generate -O3

echo -e "\n-------------------------- Compilation ended -------------------------\n"

./bin/opt_pgo
rm -f results/table_results.csv

echo -e "\n--------------------------- Profiling ended --------------------------\n"

g++ main.cpp src/base.cpp -o bin/opt_pgo -fprofile-use -O3

hyperfine --export-markdown results/opt_pgo.md --warmup $2 --runs $1 \
    './bin/opt_pgo'

echo -e "\n--------------------------- Tests counted ---------------------------\n"

python3 results_gen.py 1 4 $1 $2

echo -e "--------------------- Optimization with pgo ended --------------------\n"
