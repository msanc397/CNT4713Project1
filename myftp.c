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
	int sock, test, rec, peer;
	char* serverName;
	char* IPAddress;
	struct hostent* host;
	struct sockaddr_in sockInfo;
	char userName[10], password[10], serverMessage[250];
	char loginName[20], loginPass[20];
	char message[] = "OPTS UTF8 ON";
	
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
	
  
	printf("Please enter username:\nUsername: ");
	fgets(userName, 10, stdin);
	printf("Please enter password:\nPassword: ");
	fgets(password, 10, stdin);
	printf("Creating socket...\n");
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Socket formation failed");
		exit(-1);
	}
	printf("Socket creation successful.\n");
	
	strcpy(loginName, "USER ");
	strcat(loginName, userName);
	
	strcpy(loginPass, "PASS ");
	strcat(loginPass, password);
	strcat(loginPass, "\r");
	printf("password: %s", loginPass);
	
	sockInfo.sin_family = AF_INET;
	sockInfo.sin_port = htons(21);
	
	if ((test = (inet_pton(AF_INET, IPAddress, &(sockInfo.sin_addr)))) == 0) {
		perror("Inet_pton failed");
		exit(-1);
	}
	
	//login(sock, sockInfo, loginName, loginPass);
	printf("Connecting...\n");
	if (connect(sock, (struct sockaddr *)&sockInfo, sizeof(sockInfo)) < 0) {
		perror("Connection failed");
		exit(-1);
	}
	printf("Connection successful.\n");
	
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	send(sock, loginName, sizeof(loginName), 0);
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	send(sock, loginPass, strlen(loginPass), 0);
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	
	close(sock);
	return 0;
}
