#ifndef _PAGE_H
#define _PAGE_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "pagemanager/pagebase.h"
extern page_node_t page_main;
extern page_node_t page_bar;
void pages_init(void);
#ifdef __cplusplus
} /* extern "C" */
#endif

#endif