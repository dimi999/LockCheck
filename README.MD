# Lock check

Compile: ```make```

Clean: ```make clean```

Tests that contain deadlocks:
```
./exec tests/stupid_deadlock.txt 100
./exec tests/simple_deadlock.txt 100
./exec tests/circular_deadlock.txt 100
./exec tests/semaphore_deadlock.txt 100
```

Tests that do not contain deadlocks:
```
./exec tests/semaphore_ok.txt 100
./exec tests/no_deadlock.txt 100
```