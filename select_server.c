#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include <unistd.h>

#define MAX_CLIENTS 100

struct groups
{
	int group_num;
	int connections[100];
};

int main(int argc, char*argv[])
{
	if(argc != 2)
	{
		printf("wrong input format input, ./exefile portnum\n");
		return (-1);
	}
	
	int port = atoi(argv[1]);
	int i,j,k,maxi,maxg, maxfd, listenfd, connfd, sockfd,nready,group_num,clientfd;
	int clients[MAX_CLIENTS];
	struct groups group[100];
	for(i = 0;i<100;i++)
	{
		group[i].group_num = -1;
		for(j = 0;j<100;j++)
		{
			group[i].connections[j] = -1;
		}
	}
	char buf[1000];
	char msg[1000];
	char check[6] = "GROUP";
	char first[6];
	char gnum[10];
	fd_set allset,rset;
	socklen_t clilen;
  	struct sockaddr_in cliaddr, servaddr;
  	
  	listenfd = socket (AF_INET, SOCK_STREAM, 0);
  	bzero (&servaddr, sizeof (servaddr));
  	servaddr.sin_family = AF_INET;
  	servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  	servaddr.sin_port = htons (port);
  	bind (listenfd, (struct sockaddr *) &servaddr, sizeof (servaddr));
 	listen (listenfd,20);
  	
  	for(i = 0;i<MAX_CLIENTS;i++)
  	{
  		clients[i] = -1;
  	}
  	maxfd = listenfd;
        FD_ZERO (&allset);
        FD_SET (listenfd, &allset);
	maxg = 0;
	maxi  = -1;	
  	while(1)
  	{
  		rset = allset;		
      		nready = select (maxfd + 1, &rset, NULL, NULL, NULL);
		
		if (FD_ISSET (listenfd, &rset))
		{
			clilen = sizeof (cliaddr);
			connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
  			for (i = 0; i < MAX_CLIENTS; i++)
			{	
				if (clients[i] < 0)
	      			{
					clients[i] = connfd;	
					break;
	      			}
	      		}
	       		if (i == MAX_CLIENTS)
	       		{	
	       			printf ("too many clients");
				exit(0);
	       		}
	       		FD_SET (connfd, &allset);	
	       		if (connfd > maxfd)
	       			maxfd = connfd;	
	       		if (i > maxi)
	    			maxi = i;
	    		if (--nready <= 0)
	    			continue;		
	    		
	    	}
	    	
	    	for (i = 0; i <= maxi; i++)
		{			/* check all clients for data */
	  		if ((sockfd = clients[i]) < 0)
	    			continue;
	  		if (FD_ISSET (sockfd, &rset))
	    		{
	    			memset(buf,'\0',sizeof(buf));
	      			int n = read (sockfd, buf,1000);
	      			if(n == 0)
	      			{
	      				/*write code to remove from group*/
	      				close (sockfd);
		  			FD_CLR (sockfd, &allset);
		  			clients[i] = -1;
	      			}
	      			else{
	      			memset(first,'\0',sizeof(first));
	      			strncpy(first,buf,5);
	      			
	      			 if(strcmp(first,check) == 0)
	      			{
	      				/*printf("ready\n");*/
	      				if(buf[5] != 'M')
	      				{
	      					memset(gnum,'\0',sizeof(gnum));
	      					strcpy(gnum,&buf[6]);
	      					group_num = atoi(gnum);
	      					for(j = 0;j<maxg;j++)
	      					{
	      						if(group[j].group_num == group_num)
	      							break;
	      					}
	      					group[j].group_num = atoi(gnum);
	      					for(k = 0;k<100;k++)
	      					{
	      						if(group[j].connections[k] == -1)
	      						{
	      							printf("added client to group %s\n",gnum);
	      							group[j].connections[k] = sockfd;
	      							break;
	      						}
	      					}
	      					if(k == 100)
	      						printf("group limit exceeded, client couldnt be added, wait!");
	      					if(j == maxg)
	      						maxg++;
	      				}
	      				else if(buf[6] == 'S'&&buf[7] == 'G')
	      				{
	      					int temp = 0;
	      					k = 9;
	      					memset(gnum,'\0',sizeof(gnum));
	      					memset(msg,'\0',sizeof(msg));
	      					while(buf[k]!='$')
	      					{
	      						gnum[k-9] = buf[k];
	      						k++;
	      					}
	      					group_num = atoi(gnum);
	      					k++;
	      					while(k<strlen(buf))
	      					{
	      						msg[temp] = buf[k];
	      						k++;
	      						temp++;
	      					}
	      					msg[temp] = '\0';
	      					for(j = 0;j<maxg;j++)
	      					{
	      						if(group[j].group_num == group_num)
	      						{
	      							for(k = 0;k<100;k++)
	      							{
	      								if(group[j].connections[k]!=-1)
	      								{
	      									sockfd = group[j].connections[k];
	      									write(sockfd,msg,temp);
	      								}
	      							}
	      						}
	      					}
	      				}
	      				
	      			}
	      			else
	      			{
	      				/*printf("%d\n",maxi);*/
	      				for(j = 0;j<=maxi;j++)
	      				{
	      					
	      					if(clients[j]!= -1)
	      					{
	      						
	      						clientfd = clients[j];
	      						write(clientfd,buf,n);
	      					}
	      					
	      				}
	      				
	      			}
			}

	}}
	
	      		
  	}
}
