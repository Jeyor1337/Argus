#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

void print_usage() {
    printf("Usage: argus [options]\n");
    printf("Options:\n");
    printf("  -c, --charset <string>   Custom character set\n");
    printf("  -m, --min <int>          Minimum word length\n");
    printf("  -M, --max <int>          Maximum word length\n");
    printf("  -v, --verbose            Verbose mode\n");
    printf("  -l, --lines <int>        Maximum number of lines to generate\n");
    printf("  -t, --threads <int>      Number of threads\n");
    printf("  -o, --output <string>    Output file name\n");
    printf("  -h, --help               Print this help message\n");
}

typedef struct {
    char *charset;
    int min_len;
    int max_len;
    long long max_lines;
    char *output_file;
    int thread_id;
    int num_threads;
    long long *line_count;
    pthread_mutex_t *mutex;
} thread_args;

void generate(FILE *fp, char *charset, int min_len, int max_len, long long *line_count, long long max_lines, char *word, int len, pthread_mutex_t *mutex) {
    if (len >= min_len) {
        pthread_mutex_lock(mutex);
        if (max_lines == -1 || *line_count < max_lines) {
            word[len] = '\0';
            fprintf(fp, "%s\n", word);
            (*line_count)++;
        }
        pthread_mutex_unlock(mutex);
    }

    if (len >= max_len) {
        return;
    }

    if (max_lines != -1 && *line_count >= max_lines) {
        return;
    }

    for (int i = 0; i < strlen(charset); i++) {
        word[len] = charset[i];
        generate(fp, charset, min_len, max_len, line_count, max_lines, word, len + 1, mutex);
    }
}

void *generate_thread(void *args) {
    thread_args *t_args = (thread_args *)args;
    char *word = malloc(t_args->max_len + 1);

    FILE *fp = fopen(t_args->output_file, "a");

    for (int i = t_args->thread_id; i < strlen(t_args->charset); i += t_args->num_threads) {
        word[0] = t_args->charset[i];
        generate(fp, t_args->charset, t_args->min_len, t_args->max_len, t_args->line_count, t_args->max_lines, word, 1, t_args->mutex);
    }

    fclose(fp);
    free(word);
    return NULL;
}

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"charset", required_argument, 0, 'c'},
        {"min",     required_argument, 0, 'm'},
        {"max",     required_argument, 0, 'M'},
        {"verbose", no_argument,       0, 'v'},
        {"lines",   required_argument, 0, 'l'},
        {"threads", required_argument, 0, 't'},
        {"output",  required_argument, 0, 'o'},
        {"help",    no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    char *charset = NULL;
    int min_len = 1;
    int max_len = 8;
    int verbose = 0;
    long long max_lines = -1;
    int num_threads = 1;
    char *output_file = "results.txt";

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "c:m:M:vl:t:o:h", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                charset = optarg;
                break;
            case 'm':
                min_len = atoi(optarg);
                break;
            case 'M':
                max_len = atoi(optarg);
                break;
            case 'v':
                verbose = 1;
                break;
            case 'l':
                max_lines = atoll(optarg);
                break;
            case 't':
                num_threads = atoi(optarg);
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'h':
                print_usage();
                return 0;
            default:
                print_usage();
                return 1;
        }
    }

    if (charset == NULL) {
        fprintf(stderr, "Error: charset is a required argument.\n");
        print_usage();
        return 1;
    }

    if (verbose) {
        printf("Configuration:\n");
        printf("  Charset: %s\n", charset);
        printf("  Min length: %d\n", min_len);
        printf("  Max length: %d\n", max_len);
        printf("  Verbose: %s\n", verbose ? "yes" : "no");
        printf("  Max lines: %lld\n", max_lines);
        printf("  Threads: %d\n", num_threads);
        printf("  Output file: %s\n", output_file);
    }

    // Clear the output file before starting
    FILE *fp = fopen(output_file, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: could not open output file %s\n", output_file);
        return 1;
    }
    fclose(fp);

    pthread_t threads[num_threads];
    thread_args t_args[num_threads];
    long long line_count = 0;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    for (int i = 0; i < num_threads; i++) {
        t_args[i].charset = charset;
        t_args[i].min_len = min_len;
        t_args[i].max_len = max_len;
        t_args[i].max_lines = max_lines;
        t_args[i].output_file = output_file;
        t_args[i].thread_id = i;
        t_args[i].num_threads = num_threads;
        t_args[i].line_count = &line_count;
        t_args[i].mutex = &mutex;
        pthread_create(&threads[i], NULL, generate_thread, &t_args[i]);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    if (verbose) {
        printf("Generated %lld lines.\n", line_count);
    }

    return 0;
}