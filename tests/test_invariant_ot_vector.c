#include <check.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Include the vector operations header */
#include "opentracker/ot_vector.h"

/* Define a reasonable upper bound for peer count per torrent */
#define MAX_PEERS_PER_TORRENT 10000

START_TEST(test_vector_allocation_bounded)
{
    /* Invariant: Vector allocation must not grow unbounded - there should be
     * a maximum limit on entries to prevent memory exhaustion attacks */
    
    ot_vector vector;
    memset(&vector, 0, sizeof(ot_vector));
    
    /* Test allocation counts: boundary case, attack case, valid case */
    size_t test_counts[] = {
        1,                      /* Valid: single allocation */
        MAX_PEERS_PER_TORRENT,  /* Boundary: at limit */
        MAX_PEERS_PER_TORRENT + 1000  /* Attack: exceeds reasonable limit */
    };
    int num_tests = sizeof(test_counts) / sizeof(test_counts[0]);
    
    for (int i = 0; i < num_tests; i++) {
        size_t count = test_counts[i];
        size_t allocated = 0;
        
        /* Simulate repeated allocations as an attacker would do */
        for (size_t j = 0; j < count && j < MAX_PEERS_PER_TORRENT + 100; j++) {
            void *ptr = vector_find_or_insert(&vector, (void*)&j, sizeof(size_t), 
                                               sizeof(size_t), 0);
            if (ptr != NULL) {
                allocated++;
            }
        }
        
        /* Security invariant: allocation count should be bounded */
        ck_assert_msg(allocated <= MAX_PEERS_PER_TORRENT,
                      "Vector grew beyond safe limit: %zu entries (test %d)",
                      allocated, i);
        
        /* Cleanup */
        if (vector.data) {
            free(vector.data);
            memset(&vector, 0, sizeof(ot_vector));
        }
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_vector_allocation_bounded);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}