#include "queue.h"
#include "sched.h"

typedef pcb_t item_t;

void queue_init(queue_t *queue)
{
    /*if (queue == NULL)
        (*queue) = *(queue_t *)malloc(sizeof(queue_t));
    else*/
    // if (queue->head = queue->tail = NULL) the queue is empty
    queue->head = queue->tail = NULL;
}

int queue_is_empty(queue_t *queue)
{
    if (queue->head == NULL)
    {
        return 1;
    }
    return 0;
}

void queue_push(queue_t *queue, void *item)
{
    item_t *_item = (item_t *)item;

    // queue is empty
    if (queue->head == NULL)
    {
        queue->head = item;
        queue->tail = item;
        _item->next = NULL;
        _item->prev = NULL;
    }
    // insert into the last queue
    else
    {
        ((item_t *)(queue->tail))->next = item;
        _item->next = NULL;
        _item->prev = queue->tail;
        queue->tail = item;
    }
}

void *queue_dequeue(queue_t *queue)
{
    item_t *temp = (item_t *)queue->head;

    // this queue only has one item
    if (temp->next == NULL)
    {
        queue->head = NULL;
        queue->tail = NULL;
    }
    else
    {
        queue->head = ((item_t *)(queue->head))->next;
        ((item_t *)(queue->head))->prev = NULL;
    }

    // head
    temp->prev = NULL;
    temp->next = NULL;

    return (void *)temp;
}

void *queue_max(queue_t *queue)
{
    item_t *temp = (item_t *)queue->head; /*头*/
    item_t *max;
    max = temp;
    while (temp != NULL)
    {
        if (temp->priority_count > max->priority_count)
        {
            max = temp;
        }
        temp = temp->next;
    }
    if (max->priority_count == 0)
    {
        for (temp = (item_t *)queue->head; temp != NULL; temp = temp->next)
            temp->priority_count = temp->priority;
        temp = (item_t *)queue->head;
        max = queue->head;
        for (; temp != NULL; temp = temp->next)
        {
            if (temp->priority_count > max->priority_count)
            {
                max = temp;
            }
        }
    }
    // only one item
    if (max == queue->head && max == queue->tail)
    {
        queue->head = NULL;
        queue->tail = NULL;
    }
    // remove the first item
    else if (max == queue->head)
    {
        queue->head = max->next;
        ((item_t *)(queue->head))->prev = NULL;
    }
    // remove the last item
    else if (max == queue->tail)
    {
        queue->tail = max->prev;
        ((item_t *)(queue->tail))->next = NULL;
    }
    // remove middle item
    else
    {
        ((item_t *)(max->prev))->next = max->next;
        ((item_t *)(max->next))->prev = max->prev;
    }
    return (void*)max;
}

/* remove this item and return next item */
void *queue_remove(queue_t *queue, void *item)
{
    item_t *_item = (item_t *)item; //副本
    item_t *next = (item_t *)_item->next;

    // only one item
    if (item == queue->head && item == queue->tail)
    {
        queue->head = NULL;
        queue->tail = NULL;
    }
    // remove the first item
    else if (item == queue->head)
    {
        queue->head = _item->next;
        ((item_t *)(queue->head))->prev = NULL;
    }
    // remove the last item
    else if (item == queue->tail)
    {
        queue->tail = _item->prev;
        ((item_t *)(queue->tail))->next = NULL;
    }
    // remove middle item
    else
    {
        ((item_t *)(_item->prev))->next = _item->next;
        ((item_t *)(_item->next))->prev = _item->prev;
    }

    _item->prev = NULL;
    _item->next = NULL;

    return (void *)next;
}
