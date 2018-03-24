#include "linked_list.h"

linked_list_t *ll_new() {
    linked_list_t *ll = malloc(sizeof(linked_list_t));
    ll->node = 0;
    ll->next = NULL;
    return ll;
}

linked_list_t *ll_set(linked_list_t *ll, double val) {
    ll->node = val;
    return ll;
}

double ll_get(const linked_list_t *ll) {
    return ll->node;
}

double ll_get_and_next(linked_list_t **ll) {
    linked_list_t *old = *ll;
    *ll = (*ll)->next;
    return old->node;
}

linked_list_t *ll_next(const linked_list_t *ll) {
    return ll->next;
}

linked_list_t **ll_push(linked_list_t **ll, double val) {
    (*ll)->node = val;
    (*ll)->next = ll_new();
    *ll = (*ll)->next;
    return ll;
}

bool ll_end(const linked_list_t *ll) {
    return ll->next == NULL;
}

void ll_destroy(linked_list_t *ll) {
    while (ll->next != NULL) {
        linked_list_t *current = ll;
        ll = ll->next;
        free(current);
    }
    free(ll);
}
