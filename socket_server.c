#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<dirent.h> 
#include<stdlib.h> 
#include <sys/stat.h> 
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
 
int printDirFile(); 
int getIPaddress();
 
 
int socket_server()
{
    int socket_desc , c, numbytes;
    struct sockaddr_in server ;
    //char *message;
	//char fileName[128];
	char* fileName = "lenna.bmp";
	struct stat filestat;
	char buf[1000000];
	FILE *fp;
	int toExit = 1;

     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        puts("bind failed");
        return 1;
    }
    puts("bind done");
	getIPaddress();
     
    //Listen
    listen(socket_desc , 3);
	while(1){
		int new_socket;
		struct sockaddr_in  client;
		if(toExit == 0)
			break;
		//Accept and incoming connection
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);
		new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (new_socket<0)
		{
			perror("accept failed");
			return 1;
		}
		 
		puts("Connection accepted");
		printDirFile();
		//memset(fileName, '\0', sizeof(fileName));
		//printf("Select a file\n");
		//scanf("%s",fileName);
		if ( lstat(fileName, &filestat) < 0){
			exit(1);
		}
	printf("The file size of server send is %lun", filestat.st_size);

	fp = fopen(fileName, "rb");


	//Sending file
	while(!feof(fp)){
		numbytes = fread(buf, sizeof(char), sizeof(buf), fp);
		printf("fread %d bytes, \n", numbytes);
		numbytes = write(new_socket, buf, numbytes);
		printf("Sending %d bytesn\n",numbytes);
		toExit = 0;
	}
	fclose(fp);
	close(new_socket);

    /*
		if( recv(new_socket, &ret , sizeof(ret) , 0) < 0)
		{
			puts("receive fail");
		}
		else
		{
			client_event = ntohl(ret);
			if(client_event == 1)
			{	
				printDirFile();
			}
			else if(client_event == 2)
			{
				message = "Desert.jpg";
				if(send(socket_desc , message , strlen(message) , 0) < 0)
				{
					puts("Send failed");
					return 1;
				}
			}
			else{}
		}
		printf("client_event = %d\n",client_event);
		//close(new_socket);
		*/
	}
	
	
    //Reply to the client
    //message = "Hello Client , socket server close, bye\n";
    //write(new_socket , message , strlen(message));
	printf("Hello Client , socket server close, bye\n");
	close(socket_desc);
     
    return 0;
}
  
int printDirFile()  
{  
    DIR *dirp;  
    struct dirent *direntp;  
    dirp = opendir("/home");  
    if(dirp != NULL)  
    {  
        while(1)  
        {  
            direntp = readdir(dirp);  
            if(direntp == NULL)  
                break;  
            else if(direntp->d_name[0] != '.')  
                printf("%s\n", direntp->d_name);  
        }  
        closedir(dirp);  
        return 0;  
    }  
  
    return 0;  
}  

int getIPaddress()
{
	struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;
 
    getifaddrs(&ifAddrStruct);
 
    while (ifAddrStruct!=NULL) {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer); 
        } else if (ifAddrStruct->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifAddrStruct->ifa_name, addressBuffer); 
        } 
        ifAddrStruct=ifAddrStruct->ifa_next;
    }
	return 0;
}