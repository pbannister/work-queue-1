#include <cstdio>

#include "wq/workqueues.h"
#include "wq/queueworkers.h"

#include <getopt.h>

void test_work_queue() {
	wq::work_queue_s queue;



}

int main(int ac, char** av) {
	printf("Hello work-queue.\n");
	test_work_queue();
	return 0;
}
