/*
 * std_list.h: Doubly- and singly-linked lists implementations.
 *
 * Copyright (C) 1999-2014 Parallels IP Holdings GmbH
 *
 * This file is part of Parallels SDK. Parallels SDK is free
 * software; you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland; http://www.parallels.com/.
 */


#ifndef __STD_LIST_H__
#define __STD_LIST_H__

#define __prl_offsetof(TYPE, MEMBER) (((ULONG_PTR) &((TYPE*)16)->MEMBER)-16)

#ifdef __GNUC__
/* version of container_of with type checking inside */
#define __prl_container_of(ptr, type, member) ({                    \
		const typeof( ((type *)0)->member ) *__xptr = (ptr);  \
		(type *)( (char *)__xptr - __prl_offsetof(type,member) );})
#else
#define __prl_container_of(ptr, type, member) \
		((type *)(((char *)(ptr)) - __prl_offsetof(type,member)))
#endif


#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

struct cd_list
{
	struct cd_list *next, *prev;
};

#define CD_LIST_INIT(name) { &(name), &(name) }
#define CD_LIST_HEAD(name) \
	    struct cd_list name = CD_LIST_INIT(name)

#define cd_list_entry(ptr, type, member) \
	__prl_container_of(ptr, type, member)

#define cd_list_first_entry(ptr, type, member) \
	cd_list_entry((ptr)->next, type, member)

#define cd_list_last_entry(ptr, type, member) \
	cd_list_entry((ptr)->prev, type, member)

static __inline void cd_list_init(struct cd_list *ptr)
{
	ptr->next = ptr;
	ptr->prev = ptr;
}

static __inline void __cd_list_add(struct cd_list *newl, struct cd_list *prev, struct cd_list *next)
{
	next->prev = newl;
	newl->next = next;
	newl->prev = prev;
	prev->next = newl;
}

/* add element to list head */
static __inline void cd_list_add(struct cd_list *newl, struct cd_list *head)
{
	__cd_list_add(newl, head, head->next);
}

/* add element to list tail */
static __inline void cd_list_add_tail(struct cd_list *newl, struct cd_list *head)
{
	__cd_list_add(newl, head->prev, head);
}

#define CD_LIST_POISON_NEXT  ((void *)0x00112233)
#define CD_LIST_POISON_PREV  ((void *)0x00445566)

static __inline void __cd_list_del(struct cd_list *prev, struct cd_list *next)
{
	next->prev = prev;
	prev->next = next;
}

static __inline void cd_list_del(struct cd_list *elem)
{
	__cd_list_del(elem->prev, elem->next);

	elem->next = (struct cd_list *)CD_LIST_POISON_NEXT;
	elem->prev = (struct cd_list *)CD_LIST_POISON_PREV;
}

static __inline void cd_list_del_init(struct cd_list *elem)
{
	__cd_list_del(elem->prev, elem->next);
	cd_list_init(elem);
}

static __inline int cd_list_empty(const struct cd_list* head)
{
	return (head->next == head);
}

static __inline void cd_list_move(struct cd_list *elem, struct cd_list *head)
{
	__cd_list_del(elem->prev, elem->next);
	cd_list_add(elem, head);
}

static __inline void cd_list_move_tail(struct cd_list *elem, struct cd_list *head)
{
	__cd_list_del(elem->prev, elem->next);
	cd_list_add_tail(elem, head);
}

static __inline void cd_list_splice(struct cd_list *list, struct cd_list *head)
{
	if (!cd_list_empty(list)) {
		struct cd_list *first = list->next;
		struct cd_list *last = list->prev;
		struct cd_list *at = head->next;

		first->prev = head;
		head->next = first;

		last->next = at;
		at->prev = last;
	}
}

#define cd_list_for_each_entry(typeof_pos, pos, head, member) \
	for (pos = cd_list_entry((head)->next, typeof_pos, member); \
			&pos->member != (head); \
			pos = cd_list_entry(pos->member.next, typeof_pos, member))

#define cd_list_for_each_entry_reverse(typeof_pos, pos, head, member) \
	for (pos = cd_list_entry((head)->prev, typeof_pos, member); \
			&pos->member != (head); \
			pos = cd_list_entry(pos->member.prev, typeof_pos, member))

#define cd_list_for_each_entry_safe(typeof_pos, pos, n, head, member) \
	for (pos = cd_list_entry((head)->next, typeof_pos, member), \
			n = cd_list_entry(pos->member.next, typeof_pos, member); \
			&pos->member != (head); \
			pos = n, n = cd_list_entry(n->member.next, typeof_pos, member))

#define cd_list_for_each_entry_from_safe(typeof_pos, pos, n, head, member) \
	for (n = cd_list_entry(pos->member.next, typeof_pos, member); \
			&pos->member != (head); \
			pos = n, n = cd_list_entry(n->member.next, typeof_pos, member))

#define cd_list_for_each_entry_reverse_safe(typeof_pos, pos, p, head, member) \
	for (pos = cd_list_entry((head)->prev, typeof_pos, member),	\
			p = cd_list_entry(pos->member.prev, typeof_pos, member); \
			&pos->member != (head); \
			pos = p, p = cd_list_entry(pos->member.prev, typeof_pos, member))


struct s_list
{
	struct s_list *snext;
};

#define s_list_entry(ptr, type, member) \
	__prl_container_of(ptr, type, member)

#define s_list_first_entry(ptr, type, member) \
	s_list_entry((ptr)->snext, type, member)

static __inline void s_list_init(struct s_list *ptr)
{
	ptr->snext = NULL;
}

/* add element to list head */
static __inline void s_list_add(struct s_list *newl, struct s_list *head)
{
	newl->snext = head->snext;
	head->snext = newl;
}

#define S_LIST_POISON_NEXT  ((void *)0x00778899)

static __inline void s_list_del(struct s_list *elem, struct s_list* head)
{
	struct s_list *pos;

	for (pos = head; pos; pos = pos->snext)
		if (pos->snext == elem) {
			pos->snext = elem->snext;
			elem->snext = (struct s_list *)S_LIST_POISON_NEXT;
			break;
		}
}

#define s_list_cut_head(head, elem, type, member) \
	do {\
		elem = s_list_entry((head)->snext, type, member);\
		s_list_del_first(head);\
	} while (0)

static __inline void s_list_del_first(struct s_list* head)
{
	head->snext = head->snext->snext;
}

static __inline int s_list_empty(const struct s_list* head)
{
	return (head->snext == NULL);
}

#define s_list_null(typeof_pos, member) \
	s_list_entry(NULL, typeof_pos, member)

#define s_list_for_each_entry(typeof_pos, pos, head, member) \
	for (pos = s_list_entry((head)->snext, typeof_pos, member); \
			pos != s_list_null(typeof_pos, member); \
			pos = s_list_entry(pos->member.snext, typeof_pos, member))

#define s_list_next_safe(ptr, type, member) \
	s_list_entry((ptr) == s_list_null(type, member) ? NULL : \
		(ptr)->member.snext, type, member)

#define s_list_for_each_entry_safe(typeof_pos, pos, n, head, member) \
	for (pos = s_list_entry((head)->snext, typeof_pos, member), \
			n = s_list_next_safe(pos, typeof_pos, member); \
			pos != s_list_null(typeof_pos, member); \
			pos = n, n = s_list_next_safe(n, typeof_pos, member))

#endif /* __STD_LIST_H__ */
