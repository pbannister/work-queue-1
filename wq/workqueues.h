#pragma once 

#include <mutex>
#include <memory>
#include <functional>

namespace wq {

class work_queue_s;
typedef work_queue_s* work_queue_p;

//
//	Minimalist work-queue implementation.
//

class work_queue_s {

public:
	class workers_s;
	typedef std::function<void()> function_t;

public:
	// Setting this to false will cause workers to terminate.
	std::atomic<bool> q_live;

protected:
	struct item_s;
	typedef item_s* item_p;
	struct item_s {
		item_p p_next;
		function_t work;
	};

protected:
	// Managed free list of work items.
	std::mutex m_free;
	item_p p_free;

protected:
	// Access to free list of work items for queue-workers.
	item_p item_obtain();
	void item_release(item_p);

protected:
	// Final cleanup for list(s) of work items.
	static unsigned free_items(item_p);

protected:
	// Managed queue of work items.
	std::condition_variable q_more;
	std::mutex m_used;
	item_p p_head;
	item_p p_tail;

public:
	// How the application adds work to the queue.
	virtual void enqueue_work(function_t);

protected:
	// How the queue-worker(s) pull work from the queue.
	virtual bool dequeue_work(function_t&);

public:
	// How to cause (near) immediate termination of workers.
	virtual void queue_stop();

public:
	work_queue_s();
	virtual ~work_queue_s();

};

} /* namespace wq */
