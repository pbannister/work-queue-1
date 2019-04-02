#include "wq/workqueues.h"
#include "wq/queueworkers.h"

#include <cstdio>
#include <thread>
#include <vector>
#include <future>

#include <getopt.h>
#include <time.h>

struct options_s {
	int n_workers;
	int n_passes;
	int n_tasks;
	int n_loops;
	int n_verbose;
} g_options = {
	0,
	10,
	1000,
	1000,
	0
};

class timer_s {

public:
	typedef long long elapsed_t;

protected:
	timespec t1;
	elapsed_t dt;

public:
	void start() {
		::clock_gettime(CLOCK_REALTIME, &t1);
	}
	elapsed_t split() {
		timespec t2;
		::clock_gettime(CLOCK_REALTIME, &t2);
		dt = t2.tv_sec - t1.tv_sec;
		dt = (1000000000 * dt) + (t2.tv_nsec - t1.tv_nsec);
		return dt;
	}
	elapsed_t elapsed_ns() {
		return dt;
	}
	unsigned elapsed_us() {
		return dt / 1000;
	}
	unsigned elapsed_ms() {
		return dt / 1000000;
	}

public:
	timer_s() :dt(0) {
		start();
	}

};

void test_work_queue() {
	wq::work_queue_s queue;
	wq::work_queue_s::workers_s workers(&queue);

	new std::thread([&queue](){
		unsigned n = 0;
		while (queue.q_live) {
			printf("tick #%u\n", ++n);
			::sleep(1);
		}
	});

	typedef std::promise<std::string> promise_t;
	promise_t promise_passes;

	queue.enqueue_work([&queue, &promise_passes](){
		for (auto i_pass = 0; i_pass < g_options.n_passes; ++i_pass) {
			if (0 < g_options.n_verbose) {
				::printf("#%03d pass is running.\n", i_pass);
			}

			// Queue much work to run.
			auto n_tasks = g_options.n_tasks;
			std::vector<promise_t> promises(n_tasks);
			for (auto i_task = 0; i_task < n_tasks; ++i_task) {
				auto& promise = promises[i_task];
				queue.enqueue_work([i_task, &promise](){
					char name[40];
					::sprintf(name, "#%03d task", i_task);
					if (2 < g_options.n_verbose) {
						printf("%s is running.\n", name);
					}

					// Chew up a lot of floating point.
					double a = 1.1;
					double b = 1.002;
					double c = .999;
					auto n_loops = g_options.n_loops;
					for (auto i = 0; i < n_loops; ++i) {
						a = (b * a / c);
						a = (0 == (0xFFF & i)) ? 1.2 : a;
					}

					// Report completion.
					char sv[80];
					::sprintf(sv, "%s value: %f", name, a);
					promise.set_value(sv);
				});
			}

			// Collect results from work performed in pass.
			for (auto i_task = 0; i_task < n_tasks; ++i_task) {
				auto future = promises[i_task].get_future();
				auto v = future.get();
				if (1 < g_options.n_verbose) {
					::printf("Collected work: %s\n", v.c_str());
				}
			}
		}
		promise_passes.set_value("Passes done.");
	});

	// Start queue workers.
	timer_s timer;
	workers.worker_start(g_options.n_workers);

	// Collect result from test.
	auto future_passes = promise_passes.get_future();
	auto v = future_passes.get();
	timer.split();
	printf("Collected work: %s\n", v.c_str());
	// All work is complete at this point, and all workers are idle.
	queue.queue_stop();
	::sleep(1);

	double ns_elapsed = timer.elapsed_ns();
	double ns_pass = ns_elapsed / g_options.n_passes;
	double ns_task = ns_pass / g_options.n_tasks;
	double ns_loop = ns_task / g_options.n_loops;

	printf("%11d workers\n", g_options.n_workers);
	printf("%11d passes\n", g_options.n_passes);
	printf("%11d tasks\n", g_options.n_tasks);
	printf("%11d loops\n", g_options.n_loops);
	printf("%11.1f elapsed (milliseconds)\n", ns_elapsed / 1000000);
	printf("%11.1f elapsed/task (nanoseconds)\n", ns_task);
	printf("%11.1f elapsed/loop (nanoseconds)\n", ns_loop);
	printf("%11.0f loops/microsecond\n", 1000 / ns_loop);
}

void usage(const char* av0) {
	printf(
		"\nUsage:\n\t%s [ options ]\n"
		"\nWhere options are:\n"
		"\t-w count		number of worker threads  : %d\n"
		"\t-p count		number of passes          : %d\n"
		"\t-t count		number of tasks per pass  : %d\n"
		"\t-n count		number of loops per task  : %d\n"
		"\t-v    		verbose (repeat for more) : %d\n"
		"\n",
		av0,
		g_options.n_workers,
		g_options.n_passes,
		g_options.n_tasks,
		g_options.n_loops,
		g_options.n_verbose
	);
	exit(1);
}

int main(int ac, char** av) {
	printf("Hello work-queue.\n");
	for (;;) {
		int c = ::getopt(ac, av, "w:p:t:n:h");
		if (c < 0) break;
		switch (c) {
		case 'w':
			g_options.n_workers = ::atoi(optarg);
			break;
		case 'p':
			g_options.n_passes = ::atoi(optarg);
			break;
		case 't':
			g_options.n_tasks = ::atoi(optarg);
			break;
		case 'n':
			g_options.n_loops = ::atoi(optarg);
			break;
		default:
			usage(av[0]);
		}
	}
	if (g_options.n_workers < 2) {
		usage(av[0]);
	}
	test_work_queue();
	return 0;
}
