#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h>
#include <dirent.h>

static int sock;
static int sock_data;
static char buffer[1024];

void myftp_quit(){
	strcpy(buffer, "QUIT\r\n");
	send(sock,buffer,strlen(buffer),0);
	close(sock);
}

int main(int argc, char *argv[]) {
	int  test, rec, len;
	char* serverName;
	char* IPAddress;
	struct hostent* host;
	struct sockaddr_in sockInfo;
	char userName[30], password[30], serverMessage[250], buffer[40], buffer2[40], choice[20],*f;
	
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
	
	//continue here

	while (1)
	{
		printf("myftp> ");
		scanf("%s",choice);//User will write ftp> <command> of choice

		if(strcmp(choice,"ls") == 0){
			break;
		}
		
		if(strcmp(choice,"quit")== 0){
				myftp_quit();
				break;
		}
		
				//ftp>ls command in works

				/*recv(sock, &len, sizeof(int),0);
				f = malloc(len);
				recv(sock, f, len, 0);
				printf("The remote directory is listing as it follows:\n");
				break;*/

	close(sock);
	return 0;
}
