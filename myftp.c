
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


 void myftp_quit(){ 
 strcpy(buffer, "QUIT\r\n");
 	send(sock, buffer, strlen(buffer), 0);
 	strcpy(serverMessage, "");
 	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
 	printf("Server reply: %.*s", rec, serverMessage);
 	close(sock);
 }

int main(int argc, char *argv[]) {
	int test, len;
	char* serverName;
	char* IPAddress;
	struct hostent* host;
	struct sockaddr_in sockInfo;
	char userName[10], password[10], choice[20], fileDir[20];
	
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
	printf("Server reply: %.*s",rec, serverMessage);

	send(sock, userName, (int)strlen(userName),0);

	rec = recv(sock,serverMessage,sizeof(serverMessage),0);
	printf("Server reply: %.*s", rec,serverMessage);

	send(sock,password,(int)strlen(password),0);
	
	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
	printf("Server reply: %.*s", rec, serverMessage);

	//getchar();
	while (1)
	{
		//User will write ftp> <command> of choice
		printf("myftp> ");
		fgets(choice, 20,stdin);
		strtok(choice,"\n");

		//list files in the current directory on the remote server
		if(strcmp(choice,"ls") == 0){
			//code
			printf("ls remote file method here\n");
		} //Currently working on ftp>ls command
			

		//myftp> cd remote-dir goes here
		//change current directory to "remote-dir" on the remote server.
		else if (strncmp(choice, "cd", 3) == 0) {
 			printf("change directory method here\n");
 		}

		//myftp> get remote-file
		//download file "remote-file" from remote server to local machine. 
 		else if (strncmp(choice, "get", 4) == 0) {
 			printf("get remote file method here\n");
 		}

		//myftp> put local-file
		//upload file "local-file" from local machine to to remote server.
 		else if (strncmp(choice, "put", 4) == 0) {
 			printf("put remote file method here\n");
 		}

		//myftp> delete remote-file
		//delete  file "remote-file" from remote server.
 		else if (strncmp(choice, "delete", 7) == 0) {
 			printf("delete remote file method here\n");
 		}

		//ftp server will quit and 
		//socket will close.
		else if(strcmp(choice,"quit")== 0){ 
				myftp_quit();
				break;
		}
 		else {
 			printf("Invalid command.\n");
 		}
		 
	}
	
	//close(sock);
	return 0;
}
