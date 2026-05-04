#!/bin/bash

chmod +x scripts/base.sh
chmod +x scripts/opt1.sh
chmod +x scripts/opt2.sh
chmod +x scripts/opt3.sh

./scripts/base.sh $1 $2
./scripts/opt1.sh $1 $2
./scripts/opt2.sh $1 $2
./scripts/opt3.sh $1 $2