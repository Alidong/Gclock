#ifndef _NODE_H
#define _NODE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "stdint.h"
#include "stdbool.h"
typedef struct _node_item
{
    struct _node_item *prev;                              
    struct _node_item *next;                                                                        
} node_item_t;
typedef struct _node_list
{
	uint16_t len;
    node_item_t root;                                                                      
} node_list_t;
#define offsetof(type, member)  (size_t)(&((type*)0)->member)

    
#define node_entry(ptr, type, member) \
	(type*)((size_t)ptr-offsetof(type,member))

#define list_first_entry(ptr, type, member) \
	node_entry((ptr)->next, type, member)

#define __list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define __list_for_each_reverse(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

inline void node_list_init(node_list_t* pList)
{
	pList->len=0;
    pList->root.next = &(pList->root);
    pList->root.prev = &(pList->root);
};
inline void node_list_add_head(node_list_t* pList,node_item_t* node)
{
	pList->len++;
	pList->root.next->prev=node;
	node->next=pList->root.next;
	node->prev=&(pList->root);
	pList->root.next=node;
}
inline void node_list_add_tail(node_list_t* pList,node_item_t* node)
{
 	pList->len++;
	pList->root.prev->next=node;
	node->prev=pList->root.prev;
	node->next=&(pList->root);
	pList->root.prev=node;
}
inline node_item_t* node_list_get_head(node_list_t* pList)
{
	node_item_t* head=NULL;
	if (pList->len)
	{
		head=pList->root.next;
	}
	return head;
}
inline  node_item_t* node_list_take_head(node_list_t* pList)
{
	node_item_t* head=NULL;
	if (pList->len)
	{
		pList->len--;
		head=pList->root.next;
		pList->root.next=head->next;
		head->next->prev=&(pList->root);
		// head->next=NULL;
		// head->prev=NULL;
	}
	return head;
}
inline  node_item_t* node_list_get_tail(node_list_t* pList)
{
	node_item_t* tail=NULL;
	if (pList->len)
	{
		tail=pList->root.prev;
	}
	return tail;
}
inline  node_item_t* node_list_take_tail(node_list_t* pList)
{
	node_item_t* tail=NULL;
	if (pList->len)
	{
		pList->len--;
		tail=pList->root.prev;
		pList->root.prev=tail->prev;
		tail->prev->next=&(pList->root);
		// tail->next=NULL;
		// tail->prev=NULL;
	}
	return tail;
}
inline bool node_list_is_empty(node_list_t* pList)
{
	return pList->len!=0? false:true;
}
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
