#!/bin/bash

# Check if command line args were provided
if [[ $# -eq 0 ]]
    then
        echo "Please provide the number of times a test should be run"
        exit 0
fi

# Recompile solution
make

# Run tests
echo ""
echo "Running mutual_exclusion..."
./exec tests/mutual_exclusion.txt $1

echo ""
echo "Running hold_wait.txt..."
./exec tests/hold_wait.txt 100

echo ""
echo "Running semaphore_ok..."
./exec tests/semaphore_ok.txt $1

echo ""
echo "Running stupid_deadlock..."
./exec tests/stupid_deadlock.txt $1

echo ""
echo "Running simple_deadlock..."
./exec tests/simple_deadlock.txt $1

echo ""
echo "Running circular_deadlock..."
./exec tests/circular_deadlock.txt $1

echo ""
echo "Running semaphore_deadlock..."
./exec tests/semaphore_deadlock.txt $1
