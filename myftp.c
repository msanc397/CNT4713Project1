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

void myftp_quit() {
	strcpy(buffer, "QUIT\r\n");
	send(sock, buffer, strlen(buffer), 0);
	strcpy(serverMessage, "");
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);
	close(sock);
}

//format ip for port
char* ipFormat(char* ipaddy) {
	char delim[2] = ".";
	char* token; 
	char str[20] = "";
	token = strtok(ipaddy, delim);
	
	while (token != NULL)
	{
		strcat(str, token);
		if (token = strtok(NULL, delim))
		{
			strcat(str, ",");
		}
	}
	
}

int main(int argc, char *argv[]) {
	
	int test, len;
	char* serverName;
	char* IPAddress;
	struct hostent* host;
	struct sockaddr_in sockInfo;
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
			strcpy(buffer, "PORT ");
			//make ip in proper readable format.
			char delim[2] = ".";
			char* token;
			char str[20] = "";
			token = strtok(IPAddress, delim);
			while (token != NULL)
			{
				strcat(str, token);
				if (token = strtok(NULL, delim))
				{
					strcat(str, ",");
				}
			}
			//append ip to port
			strcat(buffer, str);
			printf("%s", str);
			puts("after ip");
			strcat(buffer, ",2,1");
			strcat(buffer, "\r\n");
			puts("cat");
			send(sock, buffer, strlen(buffer), 0);
			puts("cat");
			strcpy(serverMessage, "");
			puts("cpy");
			//rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
			puts("rec");
			printf("Server reply: %.*s", rec, serverMessage);
			strcpy(buffer, "LIST\r\n");
			send(sock, buffer, strlen(buffer), 0);
			strcpy(serverMessage, "");
			rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
			printf("Server reply: %.*s", rec, serverMessage);

		}	
		else if (strncmp(choice, "cd ", 3) == 0) {
			strncpy(fileDir, ((char*)choice)+3, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			//myftp_changedir(fileDir);
		}
		else if (strncmp(choice, "get ", 4) == 0) {
			strncpy(fileDir, ((char*)choice)+4, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			//myftp_getfile(fileDir);
		}
		else if (strncmp(choice, "put ", 4) == 0) {
			strncpy(fileDir, ((char*)choice)+4, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
			//myftp_putfile(fileDir);
		}
		else if (strncmp(choice, "delete ", 7) == 0) {
			strncpy(fileDir, ((char*)choice)+7, strlen(choice)-1);
			sprintf(buffer, "DELE %s\r\n", fileDir);

			send(sock, buffer, strlen(buffer),0);
			rec = recv(sock,serverMessage,strlen(serverMessage),0);
			printf("Server reply: %.*s",rec,serverMessage);
			
			if (strstr(serverMessage, "550") != NULL) {
				printf("DELETE FAILED.\n");
			} else {
				printf("DELETE SUCCESSFUL.\n");
			}
		}
		else {
			printf("Invalid command.\n");
		}
		
	}	
	
	return 0;
}
