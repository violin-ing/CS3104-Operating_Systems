/* SPDX-License-Identifier: MIT */

/* StACSOS - Kernel
 *
 * Copyright (c) University of St Andrews 2024
 * Tom Spink <tcs6@st-andrews.ac.uk>
 */
#include <stacsos/kernel/sched/alg/rr.h>

// *** COURSEWORK NOTE *** //
// This will be where you are implementing your round-robin scheduling algorithm.
// Please edit this file in any way you see fit.  You may also edit the rr.h
// header file.

using namespace stacsos::kernel::sched;
using namespace stacsos::kernel::sched::alg;

// void round_robin::add_to_runqueue(tcb &tcb) { panic("TODO"); }
void round_robin::add_to_runqueue(tcb &task) {
	rr_node *node = new rr_node(&task);
	if (!tail) { // Empty list
		head = tail = node;
	} else {
		tail->next = node;
		tail = node;
	}
}


// void round_robin::remove_from_runqueue(tcb &tcb) { panic("TODO"); }
void round_robin::remove_from_runqueue(tcb &task) {
	rr_node *prev = nullptr;
	rr_node *curr = head;

	// Iterate through the linked list to find the task
	while (curr) {
		if (curr->task == &task) {
			if (prev) {
				prev->next = curr->next;
			} else { // If the task to be removed is the first in the queue
				head = curr->next;
			}

			if (curr == tail) { // If the task to be removed is the last in the queue
				tail = prev;
			}

			delete curr;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}


// tcb *round_robin::select_next_task(tcb *current) { panic("TODO"); }
tcb *round_robin::select_next_task(tcb *current) {
	if (!head) { // Empty queue
		return nullptr;
	}

	// Pop the first task in the queue
	rr_node *node = head;
	head = head->next;
	if (!head) {
		tail = nullptr;
	}

	tcb *task = node->task;

	// Push to the end of the queue
	node->next = nullptr;
	if (!tail) {
		head = tail = node;
	} else {
		tail->next = node;
		tail = node;
	}

	return task;
}

