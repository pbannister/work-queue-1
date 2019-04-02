# work-queue-1
Minimalist code for a work-queue with thread pool.

With C++11 comes support for lambdas and closures. 
While it has always been possible to emulate closures in C++ (using a class to capture the closure),
this is not a fun exercise and in fact rather tedious.

Along the like, it has always been possible to do have work-queues and thread-pools, 
but the extra work needed makes this more expensive (and oft got wrong).

Take this as a challenge. Write the simplest code for a work-queue, backed by queue-workers (a thread pool),
and with the absolute minimum of built-in policy. A minimal base suitable to be subclessed for specialized behavior.

Will add example subclasses to illustrate specialized usage.
