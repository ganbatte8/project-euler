
My Project Euler solutions implemented in a single C file.
Project Euler has recently allowed posting solutions up to problem 100. 
I think this is a decent way to showcase some of my reasoning and programming skills, and share to the curious what a C approach may look like.

This works on Windows and Linux. 
Differences between platforms lie in the timer implementation and the initial memory allocation (mmap/VirtualAlloc). 
printf() formatting rules differ slightly and sometimes I don't double check that they work on both Windows and Linux for a while.

I only use stdio.h libcalls (for fopen and printf). Nothing else, not even math.h is used as far as libraries are concerned.

I have solved over 90 problems, but I have started over from the beginning to write everything in a single program that can execute them all, 
suit my current coding style better and improve some solutions.
Progress is slow as I get distracted by other projects, but the first 60 problems have been fully reimplemented so far, 
and MSVC -O2 build can run them in about 13 seconds on a 2.7GHz processor.
This is all single-threaded scalar code, so nothing hardcore as far as optimization goes, and no work is explicitly overlapped between problems: 
each problem is solved by a separate function call, lookup tables and various initialization setups 
which may be similar between some problems are rebuilt from zero at the start of each solution.
Project Euler has a 1 minute rule when designing their problems, but they don't specify on which language or hardware afaik.
This kind of straightforward C code can easily solve most problems encountered so far in less than a second.

