/*
 * vilist.h
 *
 *  Created on: Aug 10, 2015
 *      Author: john
 */

#ifndef VILIST_H_
#define VILIST_H_

#include <string.h>
#include <unistd.h>


#ifndef vilist_t
typedef struct vilist vilist_t;
struct vilist{
	vilist_t *prev;
	vilist_t *next;
};
#endif

#ifndef vilnode_t
typedef struct vilnode {
	vilist_t *prev;
	vilist_t *next;
	void *p;
} vilnode_t;
#endif

/* list macros */
static inline void vilistInit(vilist_t *h ) { h->next = h->prev = NULL; }

// add a list
static inline void vilistAppend(vilist_t *head, vilist_t *tail, vilist_t *h)
{
	if (h->next) {
        	h->next->next = head;
        	head->prev = h->next;

	}
	else
	{
		 h->prev = head;
	}
	 h->next = tail;
}

static inline void vilistAdd(vilist_t *newp, vilist_t *h)
{
	newp->next = newp->prev = NULL;
	if (h->next) {
        	h->next->next = newp;
        	newp->prev = h->next;
		 h->next = newp;
	}
	else
	{
		 h->prev = h->next = newp;
	}
}

static inline void vilistPush(vilist_t * newp, vilist_t *h)
{
	newp->next = newp->prev = NULL;
	if (h->prev) {
        	h->prev->prev = newp;
        	newp->next = h->prev;
		 h->prev = newp;
	}
	else
	{
		 h->prev = h->next = newp;
	}
}

static inline void vilistAfter(vilist_t *newp, vilist_t *node, vilist_t *h)
{
	newp->prev = node;
	newp->next = node->next;
	node->next = newp;
	if (h->next == node) {
		h->next = newp;
	}
}


static inline void vilistInsert(vilist_t * newp, vilist_t *node, vilist_t *h)
{
	newp->prev = node->prev;
	node->prev = newp;
	newp->next = node;
	if (h->prev == node) {
		h->prev = newp;
	}
}

static inline void vilistDel(vilist_t *newp, vilist_t *h)
{
	vilist_t *prev, *next;
	prev= newp->prev;
	next = newp->next;
	if (prev != NULL)
	{
		if (next != NULL)
		{
			prev->next = next;
			next->prev = prev;
		}
		else
		{
			prev->next = NULL;
			h->next = prev;
        	}
	}
	else
	{
		if (next != NULL)
		{
			next->prev = NULL;
			h->prev = next;
		}
		else
		{
			h->prev = h->next = NULL;
		}
	}
	newp->prev = newp->next = NULL;
}

static inline int vilistEmpty(vilist_t *h)
{
	return (h->prev == NULL);
}

#endif /* VILIST_H_ */
