#ifndef CRITBIT_H_
#define CRITBIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/* for __pure__ if we are compiling under dietlibc */
#include <stddef.h>

#ifndef __pure__
#define __pure__
#endif

typedef struct {
  void *root;
} critbit0_tree;

int critbit0_contains(critbit0_tree *t, const char *u) __pure__;
int critbit0_insert(critbit0_tree *t, const char *u);
int critbit0_delete(critbit0_tree *t, const char *u);
void critbit0_clear(critbit0_tree *t);
int critbit0_allprefixed(critbit0_tree *t, const char *prefix,
                         int (*handle) (const char *, void *), void *arg);

#ifdef __cplusplus
};
#endif

#endif
