#pragma once 

#include "workqueues.h"

namespace wq {

//
//	Minimalist implementation of queue workers.
//

class work_queue_s::workers_s {

protected:
	work_queue_p p_queue;
	pthread_attr_t about_worker;

public:
	virtual bool worker_start();
	bool worker_start(unsigned);

public:
	workers_s(work_queue_p);
	virtual ~workers_s();

private:
	static void* worker_run(void*);

};

} /* namespace wq */
