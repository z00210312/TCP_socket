/*				TCPRCV.C
*/
/* This is the receiver program. It opens a socket, sets the receive
window as requested, and then begins an infinite loop. Each time through 
the loop it accepts a connection and prints out messages from it. When 
the connection breaks or a termination message comes through it, the 
program accepts a new connection.  Compile the program on a unix system
by using gcc -o tcprcv tcprcv.c
The format of the command line is:  tcprcv  
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <strings.h>
#include <fcntl.h>

#define TRUE 1
#define SERVER_PORT 50100



main()
{
  int sd, length;
  struct sockaddr_in rcvr;
  int sda;
  int buflen = 10000;
  char buf[10000];
  int rcvwin, optlen;
  int rval;
  int seqno;

  rcvwin = buflen;

  /* create socket */
  sd = socket(AF_INET, SOCK_STREAM, 0);
  if(sd < 0) {
    perror("opening stream socket");
    exit(1);
  }

  /* name socket using wildcards */
  rcvr.sin_family = AF_INET;
  rcvr.sin_addr.s_addr = INADDR_ANY;
  rcvr.sin_port = htons(SERVER_PORT);
  if(bind(sd, (struct sockaddr *)&rcvr, sizeof(rcvr))) {
    perror("binding socket name");
    exit(1);
  }

  /* find out assigned port number and print out */
  length = sizeof(rcvr);
  if(getsockname(sd, (struct sockaddr *)&rcvr, &length)) {
    perror("getting socket name");
    exit(1);
  }
  printf("Socket has port #%d\n",ntohs(rcvr.sin_port));


  /* Can get same effect by doing reads of the correct size. */
  rcvwin = buflen;

  /* Accept connections from the transmitter */
  listen(sd, 5);
  do {
	seqno = 0;
	int fd = open ("Proj2Data.txt", O_RDONLY);
	int i = 1;
	char tempchar;
    sda = accept(sd,0,0);
    if(sda == -1)
      perror("accept");
    else
      do { 
        bzero(buf,sizeof(buf));
        if((rval = recv(sda,buf,rcvwin,MSG_WAITALL))<0)
          perror("reading message stream");
        if(rval == 0)
          printf("Ending connection\n");
        else {
			seqno++;
			sscanf(buf, "%d", &seqno);
			for(i;i<sizeof(buf);i++){
				read(fd, &tempchar, 1);
				buf[i] = tempchar;
			}
			printf("Received packet: seqno = %d length = %d\n, buf = %s\n ", seqno, rval, buf); 
			send(sda,buf,buflen,0);
        }
      }while (rval != 0);
    close(sda);

  }while (TRUE);

}
