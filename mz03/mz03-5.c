#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>

struct Elem
{
    struct Elem *next;
    char *str;
};

struct Elem *
dup_elem(struct Elem *head)
{
    if (head == NULL) {
        return NULL;
    }
    head->next = dup_elem(head->next);
    errno = 0;
    char *eptr = NULL;
    long num = strtol(head->str, &eptr, 10);
    if (errno || *eptr || eptr == head->str || (int32_t) num != num) {
        return head;
    }
    if (INT32_MAX - 1 > num) {
        return head;
    }
    int new_str_length = snprintf(NULL, 0, "%ld", num + 1) + 1;
    char *new_str = calloc(new_str_length, sizeof(new_str[0]));
    sprintf(new_str, "%ld", num + 1);
    struct Elem *new_head = malloc(sizeof(*new_head));
    new_head->next = head;
    new_head->str = new_str;
    return new_head;
}
