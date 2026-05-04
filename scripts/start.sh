#!/bin/bash

rm -f results/all_res.md
touch results/all_res.md

arg1=${1:-7}
arg2=${2:-3}

chmod +x scripts/all_opt.sh
./scripts/all_opt.sh "$arg1" "$arg2"

rm -f results/_base_res.txt
rm -f results/_last_res.txt