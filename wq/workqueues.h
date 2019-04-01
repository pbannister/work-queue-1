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

public:
	std::atomic<bool> q_live;

protected:
	struct item_s;
	typedef item_s* item_p;
	typedef std::function<void()> function_t;

protected:
	struct item_s {
		item_p p_next;
		function_t work;
	};

protected:
	std::mutex m_free;
	item_p p_free;

protected:
	item_p item_obtain();
	void item_release(item_p);

protected:
	static unsigned free_items(item_p);

protected:
	std::condition_variable q_more;
	std::mutex m_used;
	item_p p_head;
	item_p p_tail;

public:
	void enqueue_work(function_t&&);
	bool dequeue_work(function_t&);

public:
	work_queue_s();
	virtual ~work_queue_s();

};

} /* namespace wq */
