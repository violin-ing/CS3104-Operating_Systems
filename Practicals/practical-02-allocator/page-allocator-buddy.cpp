/* SPDX-License-Identifier: MIT */

/* StACSOS - Kernel
 *
 * Copyright (c) University of St Andrews 2024, 2025
 * Tom Spink <tcs6@st-andrews.ac.uk>
 */
#include <stacsos/kernel/debug.h>
#include <stacsos/kernel/mem/page-allocator-buddy.h>
#include <stacsos/kernel/mem/page.h>
#include <stacsos/memops.h>

// ADDITIONAL HEADER FILE FOR LOCK (THREAD-SAFETY)
#include <stacsos/kernel/lock.h>

using namespace stacsos;
using namespace stacsos::kernel;
using namespace stacsos::kernel::mem;

// Represents the contents of a free page, that can hold useful metadata.
struct page_metadata {
	page *next_free;
};

/**
 * @brief Returns a pointer to the metadata structure that is held within a free page.  This CANNOT be used on
 * pages that have been allocated, as they are owned by the requesting code.  Once pages have been freed, or
 * are being returned to the allocator, this metadata can be used.
 *
 * @param page The page on which to retrieve the metadata struct.
 * @return page_metadata* The metadata structure.
 */
static inline page_metadata *metadata(page *page) { return (page_metadata *)page->base_address_ptr(); }

/**
 * @brief Dumps out (via the debugging routines) the current state of the buddy page allocator's free lists
 */
void page_allocator_buddy::dump() const
{
	// Lock resource for thread-safety
	stacsos::kernel::unique_irq_lock lock(allocator_lock_);

	// Print out a header, so we can quickly identify this output in the debug stream.
	dprintf("*** buddy page allocator - free list ***\n");

	// Loop over each order that our allocator is responsible for, from zero up to *and
	// including* LastOrder.
	for (int order = 0; order <= LastOrder; order++) {
		// Print out the order number (with a leading zero, so that it's nicely aligned)
		dprintf("[%02u] ", order);

		// Get the pointer to the first free page in the free list.
		page *current_free_page = free_list_[order];

		// While there /is/ currently a free page in the list...
		while (current_free_page) {
			// Print out the extents of this page, i.e. its base address (at byte granularity), up to and including the last
			// valid address.  Remember: these are PHYSICAL addresses.
			dprintf("%lx--%lx ", current_free_page->base_address(), (current_free_page->base_address() + ((1 << order) << PAGE_BITS)) - 1);

			// Advance to the next page, by interpreting the free page as holding metadata, and reading
			// the appropriate field.
			current_free_page = metadata(current_free_page)->next_free;
		}

		// New line for the next order.
		dprintf("\n");
	}
}

/**
 * @brief Helper function that finds a specific block in a free list.
 * 
 * @param order The order of the list to search.
 * @param block_start The block to find.
 * @return page* A pointer to the page if found, otherwise nullptr.
 */
page* page_allocator_buddy::find_free_block(int order, page &block_start) 
{
	assert(order >= 0 && order <= LastOrder);

	page* current = free_list_[order];
	while (current) {
		if (current == &block_start) {
			return current;
		}
		current = metadata(current)->next_free;
	}
	return nullptr;
}

/**
 * @brief Inserts pages that are known to be free into the buddy allocator.
 *
 * ** You are required to implement this function **
 *
 * @param range_start The first page in the range.
 * @param page_count The number of pages in the range.
 */
// void page_allocator_buddy::insert_free_pages(page &range_start, u64 page_count) { panic("TODO"); }
void page_allocator_buddy::insert_free_pages(page &range_start, u64 page_count)
{
	// Lock resource for thread-safety
	stacsos::kernel::unique_irq_lock lock(allocator_lock_);

	u64 current_pfn = range_start.pfn();
	u64 remaining_pages = page_count;

	// Iterate while there are still pages left to be inserted
	while (remaining_pages > 0) {
		// Find the largest order that is <= remaining pages and for which the current block is aligned
		int order = 0;

		// Find the largest possible block that is small enough to fit the remaining pages AND has correct alignment
		while ((order + 1) <= LastOrder) {
			if (pages_per_block(order + 1) > remaining_pages) {
				break; // Order is too big for the number of remaining pages
			}
			if (!block_aligned(order + 1, current_pfn)) {
				break; // Order not aligned
			} 

			order++;
		}

		// At this point, 'order' should be the largest valid order
		u64 pages_in_block = pages_per_block(order);

		insert_free_block(order, page::get_from_pfn(current_pfn));

		current_pfn += pages_in_block;
		remaining_pages -= pages_in_block;
	}
}

/**
 * @brief Inserts a block of pages into the free list for the given order.
 *
 * @param order The order in which to insert the free blocks.
 * @param block_start The starting page of the block to be inserted.
 */
void page_allocator_buddy::insert_free_block(int order, page &block_start)
{
	// Assert that the given order is in the range of orders we support.
	assert(order >= 0 && order <= LastOrder);

	// Assert that the starting page in the block is aligned to the requested order.
	assert(block_aligned(order, block_start.pfn()));

	// Iterate through the free list, until we get to the position where the
	// block should be inserted, i.e. ordered by page base address.
	// The comparison in the while loop is valid, because page descriptors (which we
	// are dealing with) are contiguous in memory -- just like the pages they represent.
	page *target = &block_start;
	page **slot = &free_list_[order];
	while (*slot && *slot < target) {
		slot = &(metadata(*slot)->next_free);
	}

	// Make sure the block wasn't already in the free list.
	assert(*slot != target);

	// Update the target block (i.e. the block we're inserting) to point to
	// the candidate slot -- which is the next pointer.  Then, update the
	// candidate slot to point to this block; thus inserting the block into the
	// linked-list.
	metadata(target)->next_free = *slot;
	*slot = target;
}

/**
 * @brief Removes a block of pages from the free list of the specified order.
 *
 * @param order The order in which to remove a free block.
 * @param block_start The starting page of the block to be removed.
 */
void page_allocator_buddy::remove_free_block(int order, page &block_start)
{
	// Assert that the given order is in the range of orders we support.
	assert(order >= 0 && order <= LastOrder);

	// Assert that the starting page in the block is aligned to the requested order.
	assert(block_aligned(order, block_start.pfn()));

	// Loop through the free list for the given order, until we find the
	// block to remove.
	page *target = &block_start;
	page **candidate_slot = &free_list_[order];
	while (*candidate_slot && *candidate_slot != target) {
		candidate_slot = &(metadata(*candidate_slot)->next_free);
	}

	// Assert that the candidate block actually exists, i.e. the requested
	// block really was in the free list for the order.
	assert(*candidate_slot == target);

	// The candidate slot is the "next" pointer of the target's previous block.
	// So, update that to point that to the target block's next pointer, thus
	// removing the requested block from the linked-list.
	*candidate_slot = metadata(target)->next_free;

	// Clear the next_free pointer of the target block.
	metadata(target)->next_free = nullptr;
}

/**
 * @brief Splits a free block of pages from a given order, into two halves into a lower order.
 *
 * ** You are required to implement this function **
 * @param order The order in which the free block current exists.
 * @param block_start The starting page of the block to be split.
 */
// void page_allocator_buddy::split_block(int order, page &block_start) { panic("TODO"); }
void page_allocator_buddy::split_block(int order, page &block_start)
{
	assert(order > 0 && order <= LastOrder);

	// Remove block from higher order list
	remove_free_block(order, block_start);

	// Find PFN of split blocks
	u64 block1_pfn = block_start.pfn();
	u64 block2_pfn = block1_pfn + pages_per_block(order - 1);

	page &block1 = page::get_from_pfn(block1_pfn);
	page &block2 = page::get_from_pfn(block2_pfn);

	// Insert the smaller halves into the next order
	insert_free_block(order - 1, block1);
	insert_free_block(order - 1, block2);
}

/**
 * @brief Merges two buddy-adjacent free blocks in one order, into a block in the next higher order.
 *
 * ** You are required to implement this function **
 * @param order The order in which to merge buddies.
 * @param buddy Either buddy page in the free block.
 */
// void page_allocator_buddy::merge_buddies(int order, page &buddy) { panic("TODO"); }
void page_allocator_buddy::merge_buddies(int order, page &block)
{
	// BASE CASE 1: Maximum block size = no more merging
	if (order == LastOrder) {
		insert_free_block(order, block);
		return;
	}

	// Find buddy PFN
	u64 block_pfn = block.pfn();
	u64 buddy_pfn = block_pfn ^ pages_per_block(order); // Note: pages_per_block(order) = 1 << order
	page &buddy = page::get_from_pfn(buddy_pfn);

	// Check if buddy is actually free by iterating through free_list_
	page* free_buddy = find_free_block(order, buddy);

	if (free_buddy == nullptr) { // Buddy is not free
		// BASE CASE 2: Buddy is not free, so just insert the block into free_list_
		insert_free_block(order, block);
	} else { // Buddy is free
		// Remove buddy from free_list_
		remove_free_block(order, *free_buddy);

		// Find PFN of merged block
		page &merged_block_start = (block_pfn < buddy_pfn) ? block : *free_buddy;

		// Recursively merge blocks
		merge_buddies(order + 1, merged_block_start);
	}
}

/**
 * @brief Allocates pages, using the buddy algorithm.
 *
 * ** You are required to implement this function **
 * @param order The order of pages to allocate (i.e. 2^order number of pages)
 * @param flags Any allocation flags to take into account.
 * @return page* The starting page of the block that was allocated, or nullptr if the allocation cannot be satisfied.
 */
// page *page_allocator_buddy::allocate_pages(int order, page_allocation_flags flags) { panic("TODO"); }
page *page_allocator_buddy::allocate_pages(int order, page_allocation_flags flags)
{
	// Lock resource for thread-safety
	stacsos::kernel::unique_irq_lock lock(allocator_lock_);

	if (order < 0 || order > LastOrder) {
		return nullptr; // Invalid order
	}

	// Find smallest order >= order that has a free block
	int current_order = order;
	while (current_order <= LastOrder) {
		if (free_list_[current_order] != nullptr) {
			break; // Free block found
		} else {
			current_order++; // Increase order and try again
		}
	}

	// If no block found = out of memory
	if (current_order > LastOrder) {
		return nullptr;
	}

	// Get first block from free_list_
	page* block = free_list_[current_order];
	assert(block != nullptr);

	// Split the block if needed
	while (current_order > order) {
		split_block(current_order, *block); // split_block will handle inserting the split halves
		current_order--;
	}

	// Remove the final block
	remove_free_block(order, *block); // Remove appropriately-sized block

	// Zero memory if needed
	if ((flags & page_allocation_flags::zero) == page_allocation_flags::zero) {
		memops::pzero(block->base_address_ptr(), pages_per_block(order));
	}

	return block;
}

/**
 * @brief Frees previously allocated pages, using the buddy algorithm.
 *
 * ** You are required to implement this function **
 * @param block_start The starting page of the block to be freed.
 * @param order The order of the block being freed.
 */
// void page_allocator_buddy::free_pages(page &block_start, int order) { panic("TODO"); }
void page_allocator_buddy::free_pages(page &block_start, int order)
{
	// Lock resource for thread-safety
	stacsos::kernel::unique_irq_lock lock(allocator_lock_);

	assert(order >= 0 && order <= LastOrder);
	
	// Start merge_buddies -> handles all logic
	merge_buddies(order, block_start);
}