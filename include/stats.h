/*
 * SD-19
 * Jiarong Li 58177
 * Xiaoji Wu 59800
 * Jiaxin Huang 60808
 */
#ifndef _STATS_H
#define _STATS_H

/* Estrutura que define as estatisticas
 */
struct statistics_t
{
    int num_operations;
    long long total_time;
    int active_clients;
};

/**
 * Function to create and initialize a statistics_t structure
 */
struct statistics_t *stats_create(int num_operations, long long total_time, int active_clients);

/**
 * Function to free the memory allocated for a statistics_t structure
 */
int stats_destroy(struct statistics_t *stats);

#endif