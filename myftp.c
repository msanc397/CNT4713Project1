#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>
#include <ctype.h>

static int sock;
static int sock_data;
static int rec;
static char buffer[1024];
static char serverMessage[1024];
static char* IPAddress;
static struct sockaddr_in dataInfo;
static struct sockaddr_in sockInfo;

void myftp_quit() {
	strcpy(buffer, "QUIT\r\n");
	send(sock, buffer, strlen(buffer), 0);
	strcpy(serverMessage, "");
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	printf("Quit Successful.\n");
	close(sock);
}

int myftp_passivemode(char *ipAddress, int *port) {
	int h1, h2, h3, h4, p1, p2;
	char *result;
	
	strcpy(buffer, "PASV\r\n");
	send(sock, buffer, strlen(buffer), 0);
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	if (rec != 227)
		return 0;
	result = strrchr(serverMessage, '(');
	sscanf(result, "(%d,%d,%d,%d,%d,%d)", &h1, &h2, &h3, &h4, &p1, &p2);

	*port = (p1*256) + p2;
	return 1;
}

int myftp_datasocketOpen(int port) {
	dataInfo.sin_family = AF_INET;
	dataInfo.sin_port = htons(port);
	dataInfo.sin_addr = sockInfo.sin_addr;

	printf("Connecting...\n");
	if (connect(sock_data, (struct sockaddr *)&sockInfo, sizeof(sockInfo)) < 0) {
		perror("Connection failed");
		exit(-1);
	}
	printf("Connection successful.\n");


}
int myftp_getfile(char* fileName) {
	int port;
	
	myftp_passivemode(IPAddress, &port);
	printf("Port = %d\n", port);

	myftp_datasocketOpen(port);
	
	
	
	close(sock_data);
	return rec==226;
}
int main(int argc, char *argv[]) {
	int test, len;
	char* serverName;
	struct hostent* host;
	char userName[30], password[30], choice[20], fileDir[20];
	
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
		
	sockInfo.sin_family = AF_INET;
	sockInfo.sin_port = htons(21);
	
	printf("Creating socket...\n");
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Socket formation failed");
		exit(-1);
	}
	printf("Socket creation successful.\n");
	
	printf("Creating data socket...\n");
	if ((sock_data = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Data socket formation failed");
		exit(-1);
	}
	printf("Data socket creation successful.\n");
	
	if ((test = (inet_pton(AF_INET, IPAddress, &(sockInfo.sin_addr)))) == 0) {
		perror("Inet_pton failed");
		exit(-1);
	}
	printf("Connecting...\n");
	if (connect(sock, (struct sockaddr *)&sockInfo, sizeof(sockInfo)) < 0) {
		perror("Connection failed");
		exit(-1);
	}
	printf("Connection successful.\n");
	
  
	printf("Please enter username:\n");
	scanf("%s", userName);
	printf("Please enter password:\n");
	scanf("%s", password);
	
	strcpy(buffer, "USER ");
	strcat(buffer, userName);
	strcat(buffer, "\r\n");
	strcpy(userName, buffer);
	strcpy(buffer, "");
	strcpy(buffer, "PASS ");
	strcat(buffer, password);
	strcat(buffer, "\r\n");
	strcpy(password, buffer);
	
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	
	send(sock, userName, (int)strlen(userName), 0);
	
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	
	send(sock, password, (int)strlen(password), 0);
	
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	getchar();
	while (1) {
		printf("myftp> ");
		fgets(choice, 20, stdin);
		strtok(choice, "\n");
		
		if (strcmp(choice, "quit") == 0) {
			myftp_quit();
			break;
		}
		else if (strcmp(choice, "ls") == 0) {
			printf("ls method here\n");
		}	
		else if (strncmp(choice, "cd ", 3) == 0) {
			strncpy(fileDir, ((char*)choice)+3, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			//myftp_changedir(fileDir);
		}
		else if (strncmp(choice, "get ", 4) == 0) {
			strncpy(fileDir, ((char*)choice)+4, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			myftp_getfile(fileDir);
		}
		else if (strncmp(choice, "put ", 4) == 0) {
			strncpy(fileDir, ((char*)choice)+4, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			//myftp_putfile(fileDir);
		}
		else if (strncmp(choice, "delete ", 7) == 0) {
			strncpy(fileDir, ((char*)choice)+7, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			//myftp_deletefile(fileDir);
		}
		else {
			printf("Invalid command.\n");
		}
		
	}	
	
	return 0;
}
