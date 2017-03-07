/*
 * Copyright 2016, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * fifo.c - example of tail queue usage
 */

#include <sys/time.h>
#include <execinfo.h>
#include <stdint.h>
#include <inttypes.h>
#include <ex_common.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pmemobj_list.h"

POBJ_LAYOUT_BEGIN(list);
POBJ_LAYOUT_ROOT(list, struct fifo_root);
POBJ_LAYOUT_TOID(list, struct tqnode);
POBJ_LAYOUT_END(list);

POBJ_TAILQ_HEAD(tqueuehead, struct tqnode);

struct fifo_root {
	struct tqueuehead head;
};

struct tqnode {
	char data;
	char size[1024]; //added by Weiwei Jia
	POBJ_TAILQ_ENTRY(struct tqnode) tnd;
};

uint64_t debug_time_usec(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000lu + tv.tv_usec;
}

uint64_t debug_diff_usec(const uint64_t last) {
  return debug_time_usec() - last;
}

static void
print_help(void)
{
	printf("usage: fifo <pool> <node_number> <node_size>\n");
	printf("\tAvailable options:\n");
	printf("\tpool: pool file path\n");
	printf("\tnode_number: how many node in linked list\n");
	printf("\tnode_size: the size of each node\n");
}

int
main(int argc, const char *argv[])
{
	PMEMobjpool *pop;
	const char *path;
	uint64_t start = 0ULL;
	int i = 0;
	
	if (argc < 3) {
		print_help();
		return 0;
	}
	path = argv[1];
	int node_num = strtol(argv[2], NULL, 0);
	int node_size = strtol(argv[3], NULL, 0);
	printf("node number is %d, node size is %d\n", node_num, node_size);

	start = debug_time_usec();
	if (file_exists(path) != 0) {
		if ((pop = pmemobj_create(path, POBJ_LAYOUT_NAME(list),
			1.5*1024*1024*1024, 0666)) == NULL) {
			perror("failed to create pool\n");
			return -1;
		}
	} else {
		if ((pop = pmemobj_open(path,
				POBJ_LAYOUT_NAME(list))) == NULL) {
			perror("failed to open pool\n");
			return -1;
		}
	}

	TOID(struct fifo_root) root = POBJ_ROOT(pop, struct fifo_root);
	struct tqueuehead *tqhead = &D_RW(root)->head;
	TOID(struct tqnode) node;
	printf("Open file and mmap cost %lu microseconds\n", debug_diff_usec(start));
	start = debug_time_usec();

#if 0
	TX_BEGIN(pop) {
		for (i = 0; i < node_num; i++) {
			node = TX_NEW(struct tqnode);
			D_RW(node)->data = 0;
			POBJ_TAILQ_INSERT_HEAD(tqhead, node, tnd);
			printf("i is %d\n", i);
		}
	} TX_ONABORT {
		abort();
	} TX_END
#else
	POBJ_TAILQ_FOREACH(node, tqhead, tnd) {
		//printf("i is %d\n", i);
		//i++;
	}
	printf("Scan linkedlist cost %lu microseconds\n", debug_diff_usec(start));
#endif
	pmemobj_close(pop);
	return 0;
}
