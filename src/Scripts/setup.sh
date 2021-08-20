#!/bin/bash
set -e


make
cd Test
make
cd ..
cp sat_solver ./bin
cp pycsolver.py ./bin
cp original.jar ./bin
cp ./Scripts/runinterpreter.sh ./bin
cp z3 ./bin
cp mathsat ./bin
cp smtrat ./bin
