# Producer-Consumer-Problem
N mcounter threads count independent incoming messages in a system, and another thread
mmonitor gets the count of threads at time intervals of size t1, and then resets the counter to
0.
The mmonitor then places this value in a buffer of size b, and a mmcollector thread reads
the values from the buffer.
Any thread will have to wait if the counter is being locked by any other thread. Also, the
mmonitor and mcollecttor threads will not be able to access the buffer at the same time or to
add another entry if the buffer is full.
Assume that the messages come randomly to the system, this can be realized if the mcounter
threads sleep for random times, and their activation (sleep time ends) corresponds to an email
arrival. Similarly, the mmonitor and mcollector will be activated at random time intervals.
Procedures:
1. Divide up the problem into two sub-problems
(a) P1: counter threads count messages and add them to the counter shared
variable, and monitor thread reads it.
(b) P2: monitor thread places the count in the buffer and collector thread reads it.
Compile the program using: g++ pthread main.cpp -o .x
