/*
	list.c
	------

	Linked List Module

	Programming by Chojoongpil
	All copyright (c) 1997, MAIET entertainment software
*/
#include <stdio.h>
#include <stdlib.h>
#include "cml.h"

typedef struct ListNode {
    struct ListNode *prev;	/* Link to previous node in list. */
    struct ListNode *next;	/* Link to next node in list. */
    void *data;				/* Data value for item I. */
} ListNode;

/*
	ListAdd

	����Ʈ���� �ش� �������� ���Ѵ�.
	position - �����Ͱ� ������ ��ġ. ��, position�� ���� -1�̶�� ����Ʈ�� �� �ڿ�
	�������� ��������.
*/
int ListAdd( LIST *list, int position, void *item )
{
	ListNode *node, *prev;

/* Create a list node. */
    node = (ListNode *) malloc(sizeof (ListNode));
    if (node == NULL){
#if (_DEBUG) & (_CONSOLE)
		printf("(ListAdd) Cannot allocate memory for linked list node.\n");
#endif
		return 0;
    }
    node->prev = node->next = NULL;
    node->data = item;

	/* Add the item to the list. */
    prev = *list;
    if(prev == NULL){
        node->prev = node;
        *list = node;
    } else {					/* Existing list? */
        if(position < 0){		/* End of list? */
            node->next = prev;  prev = prev->prev;
            node->prev = prev;  prev->next = node;
            prev = node->next;  prev->prev = node;
            node->next = NULL;
        } else if(position == 0){
								/* Beginning of list? */
            node->next = prev;  node->prev = prev->prev;
            prev->prev = node;  *list = node;
        } else{					/* Position I in list? */
            while ((--position > 0) && (prev->next != NULL)) prev = prev->next;
            node->prev = prev;  node->next = prev->next;  prev->next = node;
            if (node->next == NULL) (*list)->prev = node;
            else (node->next)->prev = node;
        }
    }
    return 1;
}

/*
	ListRemove

	List������ �ش� ��ġ�� �������� �����Ѵ�.
*/

void *ListRemove( LIST *list, int position )
{
	ListNode *node, *prev;
	void *data;

    node = *list;
    if((node == NULL) || (position < 1)){
        return (NULL);
    }else if (position == 1){			/* Item 1 in list? */
        *list = node->next;
        if(node->next != NULL) (node->next)->prev = node->prev;
    } else {							/* Item 2..N in list? */
		while ((--position > 0) && (node != NULL))
			node = node->next;
		if (node == NULL)  return (NULL);
		prev = node->prev;  prev->next = node->next;
		if (node->next == NULL)			/* Very last item in list? */
			(*list)->prev = prev;
		else							/* Middle of list? */
			(node->next)->prev = prev;
    }
    data = node->data;
    free ((char *) node);
    return (data);
}

/*
	ListFind

	List���� �ش� �������� �ִ��� �˻��Ѵ�. (��ȯ������ �������� ��ġ���� �ε����� ��ȯ�Ѵ�.)
*/
int ListFind( LIST list, void *item )
{
	int i;
	
    for (i = 1 ;  list != NULL ;  list = list->next, i++)
		if (list->data == item)  break ;

    return ((list == NULL) ? 0 : i);
}

/*
	ListGet

	LIST���� �ش� �����ǿ� ��ġ�� �����͸� �����´�.
	�����Ͱ� void *������ ��ȯ�Ǿ� ����.
*/
void *ListGet( LIST list, int position )
{
	if( list == NULL ){
		return (NULL);
	} else if( position < 0 ){
        return ((list->prev)->data);
	} else if(position == 0){
        return (NULL);
	}

    while((--position > 0) && (list != NULL)) list = list->next;
    if (list == NULL)	/* I > N */
        return (NULL);
    else				/* 1 <= I <= N */
        return (list->data);
}

/*
	ListGetCount()

	List�� ������ ������ ���Ѵ�.
*/
int ListGetCount( LIST list )
{
	int  count ;

    for(count = 0; list != NULL;  count++) list = list->next;

    return (count);
}
