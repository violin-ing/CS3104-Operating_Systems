/* SPDX-License-Identifier: MIT */

/* StACSOS - Kernel
 *
 * Copyright (c) University of St Andrews 2024
 * Tom Spink <tcs6@st-andrews.ac.uk>
 */
#pragma once

#include <stacsos/kernel/sched/alg/scheduling-algorithm.h>

namespace stacsos::kernel::sched::alg {

// *** COURSEWORK NOTE *** //
// Please edit this file in any way you see fit.

// Use a linked list to implement our queue struture
struct rr_node {
	tcb *task;
	rr_node *next;
	rr_node(tcb *t) : task(t), next(nullptr) {}
};

class round_robin : public scheduling_algorithm {
private:
	rr_node *head; // First task in run queue
	rr_node *tail; // Last task in run queue

public:
	// Constructor
	round_robin() : head(nullptr), tail(nullptr) {}

	virtual void add_to_runqueue(tcb &tcb) override;
	virtual void remove_from_runqueue(tcb &tcb) override;
	virtual tcb *select_next_task(tcb *current) override;
	virtual const char *name() const { return "round robin"; }
};
} // namespace stacsos::kernel::sched::alg


