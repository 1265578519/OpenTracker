#ifndef CRITBIT_H_
#define CRITBIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <compiler.h>

typedef struct {
  void *root;
} critbit0_tree;

att_pure
int critbit0_contains(critbit0_tree *t, const char *u) noexcept;

int critbit0_insert(critbit0_tree *t, const char *u) noexcept;

int critbit0_delete(critbit0_tree *t, const char *u) noexcept;

void critbit0_clear(critbit0_tree *t) noexcept;

int critbit0_allprefixed(critbit0_tree *t, const char *prefix,
                         int (*handle) (const char *, void *), void *arg) noexcept;

#ifdef __cplusplus
};
#endif

#endif
