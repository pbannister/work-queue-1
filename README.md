# work-queue-1
Minimalist code for a work-queue with thread pool.

With C++11 comes support for lambdas and closures. 
While it has always been possible to emulate closures in C++ (using a class to capture the closure),
this is not a fun exercise.

Along the like, it has always been possible to do have work-queues and thread-pools, 
but the extra work needed (and oft got wrong) makes this more expensive.

Take this as a challenge, to write the simplest code for a work-queue, backed by queue-workers (a thread pool),
and with the absolute minimum of built-in policy. The minimal base suitable to be subclessed for specialied behavior.

Will add example subclasses to illustrate specialized usage.
