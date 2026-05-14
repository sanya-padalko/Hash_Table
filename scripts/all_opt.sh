#!/bin/bash

mkdir -p bin

chmod +x scripts/base.sh
chmod +x scripts/opt1.sh
chmod +x scripts/opt2.sh
chmod +x scripts/opt3.sh
chmod +x scripts/opt_pgo.sh
chmod +x scripts/opt_pgo_final.sh

./scripts/base.sh           $1 $2
./scripts/opt1.sh           $1 $2
./scripts/opt2.sh           $1 $2
./scripts/opt3.sh           $1 $2
./scripts/opt_pgo_final.sh  $1 $2
./scripts/opt_pgo.sh        $1 $2