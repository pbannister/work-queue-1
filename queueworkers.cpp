#include "wq/queueworkers.h"

namespace wq {

work_queue_s::workers_s::workers_s(work_queue_p _p) : p_queue(_p) {
	::pthread_attr_init(&about_worker);
}

work_queue_s::workers_s::~workers_s() {
	::pthread_attr_destroy(&about_worker);
}

void* work_queue_s::workers_s::worker_run(void* _q) {
	static unsigned n_workers = 0;
	unsigned i_worker = ++n_workers;
	unsigned n_idle = 0;
	unsigned n_work = 0;
	work_queue_p p_queue = (work_queue_p) _q;
	while (p_queue->q_live) {
		function_t work;
		if (p_queue->dequeue_work(work)) {
			work();
			++n_work;
		} else {
			++n_idle;
		}
	}
	printf("[Worker %3u of %3u] idle: %u work: %u\n", i_worker, n_workers, n_idle, n_work);
	return 0;
}

bool work_queue_s::workers_s::worker_start() {
	pthread_t id_thread;
	int v = ::pthread_create(&id_thread, &about_worker, worker_run, p_queue);
	if (v) {
		return false;
	}
	::pthread_detach(id_thread);
	return true;
}

bool work_queue_s::workers_s::worker_start(unsigned _n) {
	for (auto i = 0; i < _n; ++i) {
		if (!worker_start()) {
			return false;
		}
	}
	return true;
}

}
