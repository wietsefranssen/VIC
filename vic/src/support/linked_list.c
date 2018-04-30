/******************************************************************************
 * @section DESCRIPTION
 *
 * This file includes routines that calculate and raise alarms for writing
 * history and state files.
 *
 * @section LICENSE
 *
 * The Variable Infiltration Capacity (VIC) macroscale hydrological model
 * Copyright (C) 2016 The Computational Hydrology Group, Department of Civil
 * and Environmental Engineering, University of Washington.
 *
 * The VIC model is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <vic.h>

/*
    create a new node
    initialize the data and next field

    return the newly created node
 */
node*
list_create(node *next,
            char *name)
{
    node *new_node = (node*) malloc(sizeof(node));
    if (new_node == NULL) {
        printf("Error creating a new node.\n");
        exit(0);
    }
    strcpy(new_node->name, name);
    new_node->next = next;

    return new_node;
}

/*
    add a new node at the beginning of the list
 */
node*
list_prepend(node *head,
             char *name)
{
    node *new_node = list_create(head, name);
    head = new_node;
    return head;
}

/*
    Search for a specific node with input data

    return the first matched node that stores the input data,
    otherwise return NULL
 */
node*
list_search(node*head,
            int  id)
{
    node *cursor = head;
    while (cursor != NULL) {
        if (cursor->id == id) {
            return cursor;
        }
        cursor = cursor->next;
    }
    return NULL;
}

int
list_search_id(node *head,
               char *name)
{
    node *cursor = head;
    while (cursor != NULL) {
        if (!strcmp(cursor->name, name)) {
            return cursor->id;
        }
        cursor = cursor->next;
    }

    log_err("Name %s is unknown in output or state list. "
            "Have you correctly defined all the outputs and states "
            "for VIC and its plugins?", name);
}

/*
    remove all element of the list
 */
void
list_dispose(node *head)
{
    node *cursor, *tmp;

    if (head != NULL) {
        cursor = head->next;
        head->next = NULL;
        while (cursor != NULL) {
            tmp = cursor->next;
            free(cursor);
            cursor = tmp;
        }
        free(head);
    }
}

/*
    completely free a list
 */
void
list_free(struct node *head)
{
    node *tmp;
    while (head != NULL) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

/*
    return the number of elements in the list
 */
int
list_count(node *head)
{
    node *cursor = head;
    int   c = 0;
    while (cursor != NULL) {
        c++;
        cursor = cursor->next;
    }
    return c;
}

/*
    sort the linked list using insertion sort
 */
node*
list_insertion_sort(node*head)
{
    node *x, *y, *e;

    x = head;
    head = NULL;

    while (x != NULL) {
        e = x;
        x = x->next;
        if (head != NULL) {
            if (e->data > head->data) {
                y = head;
                while ((y->next != NULL) && (e->data > y->next->data)) {
                    y = y->next;
                }
                e->next = y->next;
                y->next = e;
            }
            else {
                e->next = head;
                head = e;
            }
        }
        else {
            e->next = NULL;
            head = e;
        }
    }
    return head;
}

/*
    reverse the linked list
 */
node*
list_reverse(node*head)
{
    node*prev = NULL;
    node*current = head;
    node*next;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    head = prev;
    return head;
}

void
list_print(node*head)
{
    node*cursor = head;
    printf("-------Printing list Start------- \n");

    while (cursor != NULL) {
        printf("%d, %s\n",
               cursor->id,
               cursor->name);

        cursor = cursor->next;
    }
}

node*
list_add_ids(node*head,
             int  start)
{
    node*cursor = head;
    int  counter = start;

    while (cursor != NULL) {
        cursor->id = counter;

        cursor = cursor->next;
        counter++;
    }
    return head;
}
