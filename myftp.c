#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	char* serverName;
	if (argc < 2 || argc > 2) {
		printf("Usage: %s <server name>\nPlease try again and enter server name.\n", argv[0]);
		exit(-1);
	}
	if (argc == 2) {
		serverName = argv[1];
	}
	
	printf("Server name is '%s'.\n", serverName);
	return 0;
}
