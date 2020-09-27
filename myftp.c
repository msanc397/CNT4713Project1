

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
 static char send_buffer[1024];
 static char receive_buffer[1024]; 
 static char* IPAddress;
 static struct sockaddr_in dataInfo;
 static struct sockaddr_in sockInfo;

 //IP Address format for port
char *ipFormat(char* ipaddy){
	char delim[2] = ".";
	char *token;
	char str[20] = "";
	token = strtok(ipaddy,delim);

	while(token != NULL){
		strcat(str,token);
		if(token==strtok(NULL,delim)){
			strcat(str,",");
		}
	}
}
 
//ftp entering in passive mode
int myftp_passivemode(int *port) {
 	int h1, h2, h3, h4, p1, p2;
 	char *result;

 	strcpy(buffer, "PASV\r\n");
 	send(sock, buffer, strlen(buffer), 0);
	strcpy(serverMessage, "");
 	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
 	printf("Server reply: %.*s", rec, serverMessage);
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
 	if (connect(sock_data, (struct sockaddr *)&dataInfo, sizeof(dataInfo)) < 0) {
 		perror("Connection failed");
 		exit(-1);
 	}
 	
	return printf("Connection successful.\n");
 }

//myftp> get
 int myftp_getfile(char* fileName){
	 int port, fileSize;

	strcpy(buffer, "TYPE I\r\n");
 	strcpy(serverMessage, "");
 	send(sock, buffer, strlen(buffer), 0);
 	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
 	printf("Server reply: %.*s", rec, serverMessage);

 	myftp_passivemode(&port);
 	myftp_datasocketOpen(port);

 	strcpy(buffer, "");
 	sprintf(buffer, "SIZE %s\r\n", fileName);
 	send(sock, buffer, strlen(buffer), 0);

 	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
 	fileSize = atoi(serverMessage + 4);

 	sprintf(buffer, "RETR %s\r\n", fileName);
 	send(sock, buffer, strlen(buffer), 0);
 	strcpy(serverMessage, "");

	 myftp_passivemode(&port);
	 printf("Port = %d\n",port);

	 while ((rec = recv(sock, serverMessage, sizeof(serverMessage), 0)) > 0) {
 		serverMessage[rec] = '\0';
 		printf("Server reply: %s", serverMessage);
 		fflush(stdout);

 		if (strstr(serverMessage, "150") == NULL) {
 			printf("GET FAILED.\n");
 			close(sock_data);
 			return 0;
 		}
 	}

 	close(sock_data);
 	return 1;

 }


//myftp>put upload a file
int myftp_putfile(char* fileName){
	int port, fileSize;
	strcpy(buffer, "TYPE I\r\n");
	strcpy(serverMessage,"");
	send(sock,buffer,strlen(buffer),0);
	rec = recv(sock,serverMessage,strlen(serverMessage),0);
	printf("Server reply: %.*s",rec,serverMessage);

	myftp_passivemode(&port);
	myftp_datasocketOpen(port);

	strcpy(buffer,"");
	sprintf(buffer, "SIZE %s\r\n",fileName);
	send(sock,buffer,strlen(buffer),0);

	rec = recv(sock,serverMessage,strlen(serverMessage),0);
	fileSize = atoi(serverMessage+4);

	sprintf(buffer, "STOR %s\r\n", fileName);
 	send(sock, buffer, strlen(buffer), 0);
 	strcpy(serverMessage, "");

	 myftp_passivemode(&port);
	 printf("Port = %d\n",port);

	 while ((rec = recv(sock, serverMessage, sizeof(serverMessage), 0)) > 0) {
 		serverMessage[rec] = '\0';
 		printf("Server reply: %s", serverMessage);
 		fflush(stdout);

 		if (strstr(serverMessage, "150") == NULL) {
 			printf("PUT FAILED.\n");
 			close(sock_data);
 			return 0;
 		}
 	}

 	close(sock_data);
 	return 1;

}

 //delete file
 	void myftp_deletefile(char*fileName){
	
	strcpy(buffer, "DELE\r\n");
 	strcpy(serverMessage, "");
	 send(sock, buffer, strlen(buffer), 0);
 	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
 	printf("Server reply: %.*s", rec, serverMessage);
	 printf("File deleted.");
     close(sock);

 }

 void myftp_list(){
		int port;

		myftp_passivemode(&port);
		myftp_datasocketOpen(port);
		strcpy(buffer, "NLST\r\n");
 		send(sock, buffer, (int)strlen(buffer), 0);
 		strcpy(serverMessage, "");
 	
	 	while (1) {
 			rec = recv(sock_data, serverMessage, sizeof(serverMessage), 0);
 			printf("Server reply: %s", serverMessage);
 			strcpy(serverMessage, "");
 		}

		 close(sock_data);
		  rec=226;
 }
 
//QUIT
 void myftp_quit(){ 
 strcpy(buffer, "QUIT\r\n");
 	send(sock, buffer, strlen(buffer), 0);
 	strcpy(serverMessage, "");
 	rec = recv(sock, serverMessage, sizeof(serverMessage), 0);
 	printf("Server reply: %.*s", rec, serverMessage);
	 printf("Quit successful!");
 	close(sock);
 }


//Main
int main(int argc, char *argv[]) {
    int test, len, numBytes;
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
 	printf("LOGIN SUCCESSFUL.\n");
 	
	 getchar();

 	char s[100];
    while (1)
	{
		//User will write ftp> <command> of choice
		printf("myftp> ");
		fgets(choice, 20,stdin);
		strtok(choice,"\n");

		//list files in the current directory on the remote server
		if(strcmp(choice,"ls") == 0){
			myftp_list();
			strcpy(buffer,"PORT ");
			//make ip a readable format
			char delim[2]=".";
			char* token;
			char str[20]="";
			token = strtok(IPAddress,delim);
			while(token != NULL)
			{
				if(token == strtok(NULL,delim)){
					strcpy(str,",");
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
			
		//myftp> cd remote-dir goes here
		//change current directory to "remote-dir" on the remote server.
		else if (strncmp(choice, "cd", 3) == 0) {
			 sprintf(fileDir, "CWD %s\r\n", choice+3);
	 		 send(sock, fileDir, strlen(fileDir),0);
			 rec = recv(sock, serverMessage, sizeof(serverMessage), 0);

 			 printf("Server reply: %.*s", rec, serverMessage);
 			 if (strstr(serverMessage, "250") != NULL) {
 					printf("CD SUCCESSFUL.\n");
 			 } else {
 				printf("CD FAILED.\n");
 			}
		}

		//myftp> get remote-file
		//download file "remote-file" from remote server to local machine. 
 		else if (strncmp(choice, "get", 4) == 0) {
 		strncpy(fileDir, ((char*)choice)+4, strlen(choice)-1);
		printf("File name: %s\n",fileDir);	
		myftp_getfile(fileDir);
 		}

		//myftp> put local-file
		//upload file "local-file" from local machine to to remote server.
 		else if (strncmp(choice, "put", 4) == 0) {
			 sprintf(fileDir,"PORT ");
 			strncpy(fileDir, ((char*)choice)+4, strlen(choice)-1);
			printf("File name: %s\n", fileDir);
 		}

		//myftp> delete remote-file
		//delete  file "remote-file" from remote server.
 		else if (strncmp(choice, "delete", 7) == 0) {
			 strncpy(fileDir,((char*)choice)+7, strlen(choice)-1);
			 printf("File name: %s\n",fileDir);
			 sprintf(buffer, "DELE %s\r\n", fileDir);

			 send(sock,fileDir,strlen(fileDir),0);
			 rec = recv(sock,serverMessage,strlen(serverMessage),0);

			 if(strstr(serverMessage,"550") != NULL){
				 printf("DELETE FAILED. \n");
			 }
			 else{
				 printf("DELETE SUCCESSFUL");
			 }
			 
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
		 
		 return 0;
	}
}