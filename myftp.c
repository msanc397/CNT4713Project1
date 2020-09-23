#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>

int main(int argc, char *argv[]) {
	char* serverName;
	char* IPAddress;
	struct hostent* host;
	char userName[10];
	char password[10];
	
	if (argc < 2 || argc > 2) {
		printf("Usage: %s <server name>\nPlease try again and enter server name.\n", argv[0]);
		exit(-1);
	}
	if (argc == 2) {
		serverName = argv[1];
	}
	host = gethostbyname(serverName);
	if (host == NULL) {
		printf("Server could not be found. Please try again.\n");
		exit(-1);
	}
	IPAddress = inet_ntoa(*((struct in_addr*)host->h_addr_list[0])); 
	printf("IP address = %s\n", IPAddress);
  
	printf("Server name is '%s'.\n", serverName);
	
	printf("Please enter username:\nUsername: ");
	fgets(userName, 10, stdin);
	strtok(userName, "\n");
	printf("Please enter password:\nPassword: ");
	fgets(password, 10, stdin);
	strtok(password, "\n");
	
	return 0;
}
