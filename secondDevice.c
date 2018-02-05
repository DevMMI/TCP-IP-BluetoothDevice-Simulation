/* Device Pairing Coding Challenge
 SecondDevice
 Mohamed Mohamud
 Jan 22nd 2018  */

#ifdef __APPLE__
#  define error printf
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT_VAL 12461
#define MIN_SOCKET 1200
#define MAX_SOCKET 49151
#define BUFFER_SIZE 11
#define HELLOMSG_SIZE 66


// Delarations
unsigned char checksum (unsigned char ptr);
int deterministicRandomNumberGenerator(int seed, int max_number, int minimum_number);
int receiveSignal(int sockfd);
int receiveHelloMessage(int sockfd);
int sendHelloMessage(int sockfd);
int sendSignal(int newsockfd);


typedef enum
{
    false,
    true
}bool;

typedef struct packet
{
  unsigned char signal;
  unsigned char checksum;
}PKT;

int main(int argc, char const *argv[])
{
  // Variable Declarations
  char *ip_val = "localhost";
  int port_val = PORT_VAL;
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  FILE *received_file;
  portno = port_val;
  int old_time;
  int current_time;
  int bytes_read;
  int time_difference;
  int sleep_time;
  int i = 0;
  int error_occurred = 0;
  int time_assigned = 0;
  int short_sleep_time = 3;
  int long_sleep_time = 10;
  int numberofBursts = 6;

  // Network Loop
  while(i < numberofBursts/2){

    // Create proper time interval of 10 seconds
    if(time_assigned){
      sleep(long_sleep_time);
    }
    else{
      sleep(short_sleep_time);
    }
    old_time = (int)time(NULL);

    // MARK - Establish connection with partner device
    printf("ESTABLISING NEW CONNECTION........");
    printf("on port %d\n", portno);
    error_occurred = 0; // reassign error_occurred back to false

  	sockfd = socket(AF_INET, SOCK_STREAM, 0);
  	if (sockfd < 0){
  		error("ERROR opening socket");
      error_occurred = 1;
    }
  	server = gethostbyname(ip_val);
  	if (server == NULL) {
  		fprintf(stderr,"ERROR, no such host\n");
  		error_occurred = 1;
  	}
    if(!error_occurred){
    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	serv_addr.sin_family = AF_INET;
    	bcopy((char *)server->h_addr,
    		(char *)&serv_addr.sin_addr.s_addr,
    		server->h_length);
    	serv_addr.sin_port = htons(portno);
    	if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0){
    		error("ERROR connecting\n");
        exit(-1);
      }

        // MARK - Begin Communications with partner device
        char buffer[11];
        int bytes_written;
        int sent = sendHelloMessage(sockfd); // Send 'hello' message
        if(sent){
          int receive = receiveHelloMessage(sockfd); // Receive 'hello' message in turn
          if(receive){
            int checksum_correct = receiveSignal(sockfd); // Receive an 8 bit signal and it's checksum, verify checksum
            if(checksum_correct){
              buffer[0] = '1';
              buffer[1] = '\0';
              printf("Received signal correctly, sending response...\n");
              bytes_written = write(sockfd, buffer, sizeof(buffer));
            }else{
              buffer[0] = '0';
              buffer[1] = '\0';
              printf("Received signal incorrectly, sending response...\n");
              bytes_written = write(sockfd, buffer, sizeof(buffer));
            }
          }
          }

          // - Break Connection
          printf("BREAKING CONNECTION........\n\n");
          close(sockfd);
          sleep(long_sleep_time);
          time_assigned = 1;


          // MARK - Establish new connection
          printf("ESTABLISING NEW CONNECTION........");
          portno = deterministicRandomNumberGenerator(portno, MAX_SOCKET, MIN_SOCKET);
          printf("on port %d\n", portno); fflush(stdout);
          sockfd = socket(AF_INET, SOCK_STREAM, 0);
        	if (sockfd < 0){
        		error("ERROR opening socket");
            error_occurred = 1;
          }
        	server = gethostbyname(ip_val);
        	if (server == NULL) {
        		fprintf(stderr,"ERROR, no such host\n");
        		error_occurred = 1;
        	}
          if(!error_occurred){
          	bzero((char *) &serv_addr, sizeof(serv_addr));
          	serv_addr.sin_family = AF_INET;
          	bcopy((char *)server->h_addr,
          		(char *)&serv_addr.sin_addr.s_addr,
          		server->h_length);
          	serv_addr.sin_port = htons(portno);
          	if (connect(sockfd,&serv_addr,sizeof(serv_addr)) < 0){
          		error("ERROR connecting\n");
              exit(-1);
            }

            // MARK - Begin Communications with partner device
            if(!error_occurred){
              fflush(stdout);
              int receive = receiveHelloMessage(sockfd); // Receive a 'hello' message
              if(receive){
                int sent = sendHelloMessage(sockfd); // Send 'hello' message in turn
                if(sent){
                  int signal_sent = sendSignal(sockfd); // Send out an 8 bit signal and it's checksum
                  if(signal_sent){
                    int error_code = listenForErrorCode(sockfd); // Listen for whether partner device received signal properly
                    if(error_code){
                      printf("Success!, <FIRST_DEVICE> reports that signal was received correctly\n");
                    }
                    else{
                      printf("Failure!, <FIRST_DEVICE> reports that signal was received incorrectly\n");
                    }
                  }
                }
              }

            // - Break Connection
            close(sockfd);
            printf("BREAKING CONNECTION........\n\n");

      }
    }
      //  - Deterministically choose next random socket
      portno = deterministicRandomNumberGenerator(portno, MAX_SOCKET, MIN_SOCKET);
      i = i + 1;
    }
  }
} // end main


// MARK - Method definitions
int receiveHelloMessage(int newsockfd){  // Hello Message Reception
  char reply[HELLOMSG_SIZE];
  int bytes_read = read(newsockfd, reply, sizeof(reply));
  printf("Received a message! <%s>\n", reply);
  fflush(stdout);
  if(bytes_read == -1){
    error("returning 0 Receiving Hello Message");
    return 0;
  }
  return 1;
}

int sendHelloMessage(int newsockfd){  // Hello Message SendOff
  char reply[HELLOMSG_SIZE];
  const char *p = "Hello you've connected to <SECOND_DEVICE>\0";
  memset(&reply[0], 0, sizeof(reply));
  strncpy(reply, p, sizeof(reply));
  int bytes_written = write(newsockfd, reply, sizeof(reply));
  if(bytes_written == -1){
    error("returning 0 Sending Hello Message");
    return 0;
  }
  return 1;
}

int sendSignal(int newsockfd){ // Send out an 8 bit signal and it's checksum
  char reply[BUFFER_SIZE];
  PKT p; // Create packet
  // Assign signal & create and assign checksum
  p.signal = '1';
  int my_checksum = checksum(p.signal);
  p = (PKT){p.signal, my_checksum};
  reply[0] = p.signal;
  reply[1] = my_checksum;
  reply[2] = '\0';
  // Write packet to socket
  int bytes_written = write(newsockfd, reply, sizeof(reply));
  printf("Sending signal.....\n");
  fflush(stdout);
  if(bytes_written == -1){
    error("returning 0 Sending Signal");
    return 0;
  }
  return 1;
}

int listenForErrorCode(int newsockfd){ // Listen for whether partner device received signal properly
  char reply[BUFFER_SIZE];
  int bytes_read = read(newsockfd, reply, sizeof(reply)); // listen for error code (0)
  if(bytes_read == -1){
    error("returning 0 Listening for Error Code");
    return 0;
  }
  else{
    return reply[0];
  }
}

int receiveSignal(int sockfd){ // Receive a signal and return -1 if the checksum is incorrect
  char reply[BUFFER_SIZE];
  int bytes_read;

  memset(&reply[0], 0, sizeof(reply));
  bytes_read = read(sockfd, reply, sizeof(reply));
  printf("Received a signal, confirming with checksum...\n");
  PKT p;
  if(bytes_read == -1){
    return -1;
  }
  else{
    p.signal = reply[0];
    p.checksum = reply[1];
    int my_checksum = checksum(p.signal);

    if(my_checksum != p.checksum){
      error("Returning 0 Receiving Signal");
      return 0;
    }
    return 1;
}
}

unsigned char checksum (unsigned char ptr) { // A simple checksum for a single byte signal
    return ptr + 5;
}

int deterministicRandomNumberGenerator(int seed, int max_number, int minimum_number){ // deterministically generate a random number
  srand(seed);
  seed = seed % (max_number + 1 - minimum_number) + minimum_number;
  return seed;
}
