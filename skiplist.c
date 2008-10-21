#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* To Go in Header */

/* 
 * SkipLists are Key-Value associtive arrays ordered by key.
 * They have a O(ln n) search, insertion, and deletion time.
 * They have O(n + ln n) storage
 */

typedef enum {
    kSkipListKeyCompareLessThan = -1,
    kSkipListKeyCompareEquals = 0,
    kSkipListKeyCompareGreaterThan = 1,
} SkipListKeyComparisonResult;

typedef SkipListKeyComparisonResult (*SkipListKeyCompare) (void *key);
typedef void *(*SkipListKeyRetain) (void *key);
typedef void (*SkipListKeyRelease) (void *key);
typedef void *(*SkipListValueRetain) (void *value);
typedef void (*SkipListValueRelease) (void *value); 

typedef struct _SkipListFunctions {
    SkipListKeyCompare keyCompare;
    // The following retain/release functions can be set to NULL if you want to do your own memory management.
    // However, if you remove a key/value from the list or destroy the list, you may lose references to your memory.
    SkipListKeyRetain keyRetain;  
    SkipListKeyRelease keyRelease;
    SkipListValueRetain valueRetain;
    SkipListValueRelease valueRelease;
} SkipListFunctions;

typedef struct _SkipList *SkipListRef;

/* End Header */

#pragma mark -
#pragma mark Private Types

typedef struct _SkipListNode *SkipListNodeRef;

// The next struct is internal and should never be exposed through the API
typedef struct _SkipListNext {
    SkipListNodeRef next; // The direct neighbor
    long numSkips;
    SkipListNodeRef *skips;
} SkipListNext, *SkipListNextRef;

// Nodes are internal and should never be exposed through the API
typedef struct _SkipListNode {
    SkipListNext slNext;
    void *key;
    void *value;
} SkipListNode;

// This is opaque to the user
typedef struct _SkipList {
    SkipListNode head;  // We're wasting 2 pointer values at the ease of not special casing bookmark == NULL
    SkipListNode *bookmark;
    SkipListNode *bookmarkPrev; // Points to the node just prior to the bookmark
    long size;
    SkipListFunctions functions;
} SkipList;

#pragma mark -
#pragma mark SkipListNode

static SkipListNode *SkipListNodeCreate(SkipList *list, void *key, void *value) {
    SkipListNode *node = malloc(sizeof(SkipListNode));
    node->key = key;
    node->value = value;
    if (list->functions.keyRetain) {
        list->functions.keyRetain(key);
    }
    if (list->functions.valueRetain) {
        list->functions.valueRetain(value);
    }
    return node;
}

static void SkipListNodeDestroy(SkipList *list, SkipListNode *node) {
    assert(node);
    if (list->functions.keyRelease) {
        list->functions.keyRelease(node->key);
    }
    if (list->functions.valueRelease) {
        list->functions.valueRelease(node->value);
    }
    free(node);
}

#pragma mark -
#pragma mark SkipList

SkipList *SkipListCreate(SkipListFunctions *functions) {
    SkipList *list = malloc(sizeof(SkipList));
    memcpy(&(list->functions), functions, sizeof(SkipListFunctions));
    return list;
}

void SkipListResetBookmark(SkipList *list) {
    list->bookmark = &(list->head);
}

static SkipListNode *SkipListGetNext(SkipList *list) {
    if (list->bookmark == NULL) return NULL;
    list->bookmarkPrev = list->bookmark;
    list->bookmark = list->bookmark->slNext.next;
    return list->bookmark;
}

void SkipListGetNextKeyValue(SkipListRef list, void **key, void **value) {
    assert(list && key && value);
    SkipListNode *node = SkipListGetNext(list);
    *key = node ? node->key : NULL;
    *value = node ? node->value : NULL;
}

// Should be O(ln n)
void *SkipListFind(SkipListRef list, void *key) {
    // Check the curreny bookmark (why not?)
    if (list->bookmark != NULL && (list->bookmark->key == key)) {
        return list->bookmark->value;
    }
    
    // It's not the bookmark, so perform the normal search algorithm
    
}

void SkipListDestroy(SkipList *list) {
    if (!list) return;
    
    SkipListResetBookmark(list);
    SkipListNode *node = SkipListGetNext(list);
    while (node) {
        SkipListNode *nextNode = SkipListGetNext(list);
        SkipListNodeDestroy(list, node);
        node = nextNode;
    }
    free(list);
}
