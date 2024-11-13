/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Function to create and initialize a statistics_t structure
 */
struct statistics_t *stats_create(int num_operations, long long total_time, int active_clients)
{
    struct statistics_t *stats = (struct statistics_t *)malloc(sizeof(struct statistics_t));
    if (stats == NULL)
    {
        return NULL;
    }

    stats->num_operations = num_operations;
    stats->total_time = total_time;
    stats->active_clients = active_clients;
    return stats;
}
/**
 * Function to free the memory allocated for a statistics_t structure
 */
int stats_destroy(struct statistics_t *stats)
{
    if (stats == NULL)
    {
        return -1;
    }

    free(stats);
    return 0;
}