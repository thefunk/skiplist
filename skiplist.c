#include <stdlib.h>
#include <assert.h>

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

typedef struct _SkipListFunctions {
  SkipListKeyComparisonResult keyCompare;
} SkipListFunctions;

typedef struct _SkipList *SkipListRef;

/* End Header */

#pragma mark -
#pragma mark Types

typedef struct _SkipListNode *SkipListNodeRef;

// The next struct is internal and shoul never be exposed through the API
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

SkipListNode *SkipListNodeCreate(void *key, void *value) {

}

void SkipListNodeDestroy(SkipList *list, SkipListNode *node) {

}

#pragma mark -
#pragma mark SkipList

SkipList *SkipListCreate() {
  SkipList *list = malloc(sizeof(SkipList));
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
void SkipListFind(SkipListRef list, void *key, void **value) {
  
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
