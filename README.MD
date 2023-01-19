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
./exec tests/hold_wait.txt 100
```

## Solution description

Our project tries to find possible deadlocks and starvations in a program that uses shared resources such as mutexes and semaphores. The programs that we analyze are written in a simplified syntax (there are a few examples in the ```tests/``` folder).

The problem of statically deciding whether or not a program can suffer a deadlock is still open. There is a probabilistic solution for Java programs (https://www.cis.upenn.edu/~mhnaik/papers/icse09.pdf), however there is no deterministic solution known for general programs.

There are multiple algorithms for detecting or preventing deadlocks at runtime. Our project uses a slightly modified deadlock avoidance algorithm (Banker's algorithm) and yields probabilistic responses with no false positives (i.e. if our algorithm detects a deadlock, then a deadlock can definitely occur; however, if our algorithm does not detect a deadlock, there may or may not be one).

Firstly, there are 4 necessary conditions for a deadlock to occur. By ensuring that at least one of these 4 conditions does not hold, a deadlock cannot occur in a program.
The first two conditions can be checked statically, namely ```Mutual exclusion``` and ```Hold and wait```. 

If there are no shared resources between threads, then there is no Mutual exclusion and the program cannot deadlock. If a thread that requests a resource does not hold any other shareable resource, then the Hold and wait condition is not met, therefore no deadlock can occur.
We check these two conditions at the start, and if one of them is not met, we can report with no false negatives that a deadlock cannot occur.

The third condition assumes that the operating system can preempt allocated resources - this is not the case for mutexes and semaphores (the OS can, however, preempt resources such as CPU registers, because it can save their state before a context switch). The fourth condition imposes a total order on shareable resources, which not a lot of practical programs can conform to.

Banker's algorithm is a deadlock avoidance algorithm that has the following requirements: 
* the number of threads/processes is kept constant
* each thread/process must declare before the start of the execution what is the maximum number of resources of each kind that it will need
* the number of available resources of each kind is known

The first requirement is trivially met as the file containing the program has a fixed number of threads.
The second requirement is constructed as a 2D array during parsing, with an entry for each pair of (thread, resource type).
The third requirement is also an array constructed during parsing, with an entry for each resource type.

Banker's algorithm keeps track of the number of allocated resources to each thread and the number of available resources. It defines the concept of ```safe states```, i.e. a point of time in the runtime of a program in which the algorithm can guarantee that there is at least one possible sequential execution of all threads such that the demands of each thread are met. If a state is not safe, then there is no way that the threads can finish their work, so a deadlock is bound to occur.

When a thread requests a new resource of some kind, it asks the algorithm if the request can be granted.
If there are not enough available resources of that kind, the algorithm denies the request.
If there are enough available resources of that kind, the algorithm simulates granting the resource to the new thread, and checks whether or not the state that it transitioned to is safe or not. If it is safe, the request is indeed granted; if it is not safe, then the algorithm reports that a deadlock is unavoidable.

We check whether or not a program can suffer a deadlock by simulating the program a number of times against Banker's algorithm. When one thread requests a resource, we run the algorithm to see if the resource can be granted. If there is at least one occurrence of a deadlock, then the parsed program is sure to contain a deadlock. However, if there are no occurrences of a deadlock, we cannot guarantee that a deadlock will never occur.

One trick that we used in order to increase the likelihood of a faulty program being detected is to add a random sleep time between different requests for each thread. This way, we simulate possible work done between consecutive resource requests and we also account for cases in which a program pause can occur (i.e. the thread was preempted or there was a stop-the-world garbage collection).

Starvation detection is also an open problem. There are some techniques for avoiding starvation (priority ageing, for example), but there are few solutions to identifying starvation and they are almost always program-specific.

The simplest way of probabilistically detecting starvation is to identify a period of time T that is considered unreasonable and see if the program has elapsed more than T while in execution.

Since we only allow 100 instructions per program, and Banker's algorithm has a complexity of ```O(num_threads^2*num_resources)```, we think that 10s is a reasonable threshold for starvation.
