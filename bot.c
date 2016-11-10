/**************************/
/*     IRC BOT: eXBoT     */
/*   Created By eXtruct   */
/**************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 6667
#define MAXBUF 1000

void usage(char *name)
{
	printf("Usage: %s <hostname> <channel>\nExample: %s irc.localhost.com #chan\n", name, name);
	exit(1);
}

int ircconnect(char *ircserver)
{
	int sockfd;			//Socket descriptor
	struct sockaddr_in server;	//Server struct
	struct hostent *he;		//Host struct
	
	he = gethostbyname(ircserver);	//Get host

	/* Creating socket */
	printf("Creating socket... ");
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
		perror("socket");
		exit(1);
	}
	printf("Done\n");

	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr = *((struct in_addr *)he->h_addr);
	memset(&(server.sin_zero), '\0', 8);

	/* Connect to server */
	printf("Connecting to %s... ",ircserver);
	if( (connect(sockfd, (struct sockaddr *)&server,  sizeof(struct sockaddr))) == -1 ) {
		perror("connect");
		exit(1);
	}
	printf("Done\n");
	return sockfd;
}

char* enter(int sockfd, char *chan)
{
	int nbytes;
	char nick[] = "eXBoT";
	char buffer[MAXBUF];
	char ping[MAXBUF];
	char *pong;

	/* Send nick */
	printf("Sending nick %s... ", nick);
	sprintf(buffer, "NICK %s\r\n", nick);
	if( (nbytes = send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
		perror("send");
		exit(1);
	}
	printf("Done\n");

	/* Send user */
	printf("Sending user... ");
	sprintf(buffer, "USER ident 8 * :my name is eXBoT\r\n");
	if( (nbytes = send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
		perror("send");
		exit(1);
	}
	printf("Done\n");
	sleep(2);

	/* Recive ping */
	printf("Reciving ping... ");
	if( (nbytes = recv(sockfd, ping, MAXBUF, 0)) == -1 ) {
		perror("recv");
		error(1);
	}
	pong = strstr(ping, "PING :");
	pong = strstr(pong, ":");
	printf("Done\n");

	/* Send pong */
	printf("Sending pong... ");
	sprintf(buffer, "PONG %s\r\n", pong);
	if( (nbytes = send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
		perror("send");
		exit(1);
	}
	printf("Done\n");

	/* Join to channel */
        printf("Joining to channel %s... ", chan);
        sprintf(buffer, "JOIN %s\r\n", chan);
        if( (nbytes = send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
                perror("send");
                exit(1);
        }
        printf("Done\n");

	/* Send hello message */
        printf("Sending \"hello\" message... ");
        sprintf(buffer, "PRIVMSG %s :Hello\r\n", chan);
        if( (nbytes = send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
                perror("send");
                exit(1);
        }
        printf("Done\n");

	return pong;
}

void commands(int sockfd, char *chan, char *pong, char *prog)
{
	char buffer[MAXBUF];
	char servmess[MAXBUF];
	char *temp;

	while(1) {
		/* Recive message */
		if( (recv(sockfd, servmess, strlen(servmess), 0)) == -1 ) {
			perror("recv");
			exit(1);
		}

		/* Check ping */
		sprintf(buffer, "PING :");
		if( (strstr(servmess, buffer)) != NULL) {
			printf("Sending pong... ");
			sprintf(buffer, "PONG %s\r\n", pong);
			if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
				perror("send");
				exit(1);
			}
			printf("Done\n");
		}

		/* Info Line */
		sprintf(buffer, "PRIVMSG %s :!info", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			sprintf(buffer, "PRIVMSG %s :My name is eXBoT. Version 2.0 Beta. Creator: eXtruct. Language: C.\n", chan);
			printf("sending info... ");
			if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
				perror("send");
				exit(1);
			}
			printf("Done\n");
		}

		/* Help */
		sprintf(buffer, "PRIVMSG %s :!help", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			sprintf(buffer, "PRIVMSG %s :eXBoT Commands: !info (show info), !quit (quit), !help (show help), !master (show bot masters), !nick <botnick> (change bot nick), !say <chan> <:message> (send message to chan), !op <nick> (give op to nick), !deop <nick> (take op from nick)\n", chan);
			printf("sending info... ");
			if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
				perror("send");
				exit(1);
			}
			printf("Done\n");
		}

		/* Masters */
		sprintf(buffer, "PRIVMSG %s :!master", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			sprintf(buffer, "PRIVMSG %s :My master is: eXtruct\n", chan);
			printf("sending info... ");
			if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
				perror("send");
				exit(1);
			}
			printf("Done\n");
		}

		/* Nick */
		sprintf(buffer, "PRIVMSG %s :!nick", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			temp = strstr(servmess, "nick");
			temp = strstr(temp, " ");
			if( (strstr(servmess, ":eXtruct")) != NULL ) {
				sprintf(buffer, "NICK %s\n\r", temp);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
			else {
				sprintf(buffer, "PRIVMSG %s :You are not my master. Asshole ^-^\n", chan);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
		}

		/* Op */
		sprintf(buffer, "PRIVMSG %s :!op", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			temp = strstr(servmess, "op");
			temp = strstr(temp, " ");
			if( (strstr(servmess, ":eXtruct")) != NULL ) {
				sprintf(buffer, "MODE %s +o %s\n\r", chan, temp);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
			else {
				sprintf(buffer, "PRIVMSG %s :You are not my master. Asshole ^-^\n", chan);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
		}

		/* Deop */
		sprintf(buffer, "PRIVMSG %s :!deop", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			temp = strstr(servmess, "deop");
			temp = strstr(temp, " ");
			if( (strstr(servmess, ":eXtruct")) != NULL ) {
				sprintf(buffer, "MODE %s -o %s\n\r", chan, temp);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
			else {
				sprintf(buffer, "PRIVMSG %s :You are not my master. Asshole ^-^\n", chan);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
		}


		/* Say */
		sprintf(buffer, "PRIVMSG %s :!say", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			temp = strstr(servmess, "say");
			temp = strstr(temp, " ");
			if( (strstr(servmess, ":eXtruct")) != NULL ) {
				sprintf(buffer, "PRIVMSG %s\n\r", temp);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
			else {
				sprintf(buffer, "PRIVMSG %s :You are not my master. Asshole ^-^\n", chan);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
		}

		/* Quit */
		sprintf(buffer, "PRIVMSG %s :!quit", chan);
		if( (strstr(servmess, buffer)) != NULL ) {
			if( (strstr(servmess, ":eXtruct")) != NULL ) {
				sprintf(buffer, "PRIVMSG %s :Iam not wanted here\n", chan);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
				break;
			}
			else {
				sprintf(buffer, "PRIVMSG %s :You are not my master. Asshole ^-^\n", chan);
				printf("sending info... ");
				if( (send(sockfd, buffer, strlen(buffer), 0)) == -1 ) {
					perror("send");
					exit(1);
				}
				printf("Done\n");
			}
		}

	}	
	printf("Disconnecting... ");
	close(sockfd);
	printf("Done\n");
}

int main(int argc, char *argv[])
{
	int sockfd;
	char *pong;

	/* Usage */
	if( argc != 3 )
		usage(argv[0]);

	/* Call ircconenct to connect */
	sockfd = ircconnect(argv[1]);

	/* Entert paramatres like nick user join... */
	pong = enter(sockfd, argv[2]);

	/* Commands function */
	commands(sockfd, argv[2], pong, argv[0]);

	return 0;
}

