#include "wq/workqueues.h"

namespace wq {

unsigned work_queue_s::free_items(item_p p_list) {
	unsigned n = 0;
	while (p_list) {
		item_p p = p_list;
		p_list = p_list->p_next;
		delete p;
		++n;
	}
	return n;
}

work_queue_s::item_p work_queue_s::item_get() {
	std::lock_guard<std::mutex> q_lock(m_free);
	item_p p = p_free;
	if (p) {
		p_free = p->p_next;
		p->p_next = 0;
	} else {
		p = new item_s();
	}
	return p;
}

void work_queue_s::item_put(item_p p) {
	std::lock_guard<std::mutex> q_lock(m_free);
	p->p_next = p_free;
	p_free = p;
}

void work_queue_s::enqueue_work(function_t&& work) {
	std::lock_guard<std::mutex> q_lock(m_used);
	item_p p = item_get();
	p->work = work;
	if (p_tail) {
		p_tail->p_next = p;
		p_tail = p;
	} else {
		p_head = p_tail = p;
	}
	q_more.notify_one();
}

static const auto work_wait = std::chrono::milliseconds(100);

bool work_queue_s::dequeue_work(function_t& work) {
	std::unique_lock<std::mutex> q_lock(m_used);
	if (p_head) {
		item_p p = p_head;
		if (p->p_next) {
			p_head = p->p_next;
		} else {
			p_head = p_tail = 0;
		}
		p->p_next = 0;
		work = p->work;
		return true;
	}
	q_more.wait_for(q_lock, work_wait);
	return 0;
}

work_queue_s::work_queue_s() : q_live(true), p_free(0), p_head(0), p_tail(0) {
}

work_queue_s::~work_queue_s() {
	std::unique_lock<std::mutex> l_used(m_used);
	std::unique_lock<std::mutex> l_free(m_free);
	unsigned n_used = free_items(p_head);
	unsigned n_free = free_items(p_free);
	p_head = p_tail = 0;
	printf("Queue done - used: %u free: %u\n", n_used, n_free);
}

} /* namespace wq */
