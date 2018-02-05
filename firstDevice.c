/* 75F Coding Challenge
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
#define IP_VAl "localhost"

// Delarations
unsigned char checksum (unsigned char ptr);
int deterministicRandomNumberGenerator(int seed, int max_number, int minimum_number);
int receiveHelloMessage(int newsockfd);
int sendHelloMessage(int newsockfd);
int sendSignal(int newsockfd);
int receiveSignal(int newsockfd);
int listenForErrorCode(int newsockfd);

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


int main(int argc, char const *argv[]){

  // Variable Declarations
  int port_val = PORT_VAL;
  int sockfd, newsockfd, portno, clilen, bytes_read;
  struct sockaddr_in serv_addr, cli_addr;
  int n, itemRequestNum;
  int error_occurred = 0;
  int i = 0;
  portno = port_val;
  int old_time;
  int current_time;
  int time_difference;
  int sleep_time;

  // Network Loop
  while (i < 3){

    // MARK - Establish connect point and wait for connection
    printf("ESTABLISING NEW CONNECTION........");
    printf("on port %d\n", portno);
    error_occurred = 0; // reassign error_occurred back to false
    if (port_val == -1) { //if we're missing directory or the port value
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // setup socket
    if (sockfd < 0){
       error("ERROR opening socket");
       error_occurred = 1;
     }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
      if(!error_occurred){
        if (bind(sockfd, (struct sockaddr *) &serv_addr,
                 sizeof(serv_addr)) < 0){
                 error_occurred = 1;
                 error("ERROR on binding");
               }
        listen(sockfd,5);
        listen(sockfd,5);
        clilen = sizeof(cli_addr);
        printf("waiting for connection...\n");
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);


        if (newsockfd < 0){
          error("ERROR on accept");
          exit(-1);
        }

        // MARK - Begin Communications with partner device
        int receive = receiveHelloMessage(newsockfd); // Receive a 'hello' message
        if(receive){
          int sent = sendHelloMessage(newsockfd); // Send 'hello' message in turn
          if(sent){
            int signal_sent = sendSignal(newsockfd); // Send out an 8 bit signal and it's checksum
            if(signal_sent){
              int error_code = listenForErrorCode(newsockfd); // Listen for whether partner device received signal properly
              if(error_code){
                printf("Success!, <SECOND_DEVICE> reports that signal was received correctly\n");
              }
              else{
                printf("Failure!, <SECOND_DEVICE> reports that signal was received incorrectly\n");
              }
            }
          }

        }

        // - Break Connection
        printf("BREAKING CONNECTION........\n\n");
        sleep(1);
        close(sockfd); close(newsockfd);
        printf("ESTABLISING NEW CONNECTION........");

        //  - Deterministically choose next random socket
        portno = deterministicRandomNumberGenerator(portno, MAX_SOCKET, MIN_SOCKET);
        printf("on port %d\n", portno);
        fflush(stdout);

        // MARK - Establish connect point and wait for connection

        sockfd = socket(AF_INET, SOCK_STREAM, 0); // setup socket
        if (sockfd < 0){
           error("ERROR opening socket");
           error_occurred = 1;
         }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_addr.s_addr = INADDR_ANY;
        serv_addr.sin_port = htons(portno);
          if(!error_occurred){
            if (bind(sockfd, (struct sockaddr *) &serv_addr,
                     sizeof(serv_addr)) < 0){
                     error_occurred = 1;
                     error("ERROR on binding");
                   }
            listen(sockfd,5);
            listen(sockfd,5);
            clilen = sizeof(cli_addr);
            printf("waiting for connection......\n");
            newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

          if (newsockfd < 0){
            error("ERROR on accept");
            exit(-1);
          }

          // MARK - Begin Communications with partner device
          char buffer[11];
          int bytes_written;
          int sent = sendHelloMessage(newsockfd);
          if(sent){
            int receive = receiveHelloMessage(newsockfd);
            if(receive){
              int checksum_correct = receiveSignal(newsockfd);
              if(checksum_correct){
                buffer[0] = '1';
                buffer[1] = '\0';
                printf("Received signal correctly, sending response...\n");
                bytes_written = write(newsockfd, buffer, sizeof(buffer));
              }else{
                buffer[0] = '0';
                buffer[1] = '\0';
                printf("Received signal incorrectly, sending response...\n");
                bytes_written = write(newsockfd, buffer, sizeof(buffer));
              }
            }
            }

            // - Break Connection
            sleep(1);
            close(sockfd); close(newsockfd);
            printf("BREAKING CONNECTION........\n\n");

        }
      }

      //  - Deterministically choose next random socket
      portno = deterministicRandomNumberGenerator(portno, MAX_SOCKET, MIN_SOCKET);
      i = i + 1;
  }

    } // end main


// MARK - Method definitions
int receiveHelloMessage(int newsockfd){  // Hello Message Reception
  char reply[HELLOMSG_SIZE];
  int bytes_read = read(newsockfd, reply, sizeof(reply));
  printf("Received a message! <%s>\n", reply);
  if(bytes_read == -1){
    error("returning 0 Receiving Hello Message");
    return 0;
  }
  return 1;
}

int sendHelloMessage(int newsockfd){  // Hello Message SendOff
  char reply[HELLOMSG_SIZE];
  const char *p = "Hello you've connected to <FIRST_DEVICE>\0";
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
  printf("Sending signal.....\n");
  int bytes_written = write(newsockfd, reply, sizeof(reply));
  if(bytes_written == -1){
    error("returning 0 Sending Signal");
    return 0;
  }
  return 1;
}

int listenForErrorCode(int newsockfd){ // Listen for whether partner device received signal properly
  char reply[BUFFER_SIZE];
  int bytes_read = read(newsockfd, reply, sizeof(reply)); // listen for error code (0)
  //printf("%s\n", reply);
  if(bytes_read == -1){
    error("returning 0 Listening for Error Code");
    return 0;
  }
  else{
    return reply[0];
  }
}

int receiveSignal(int newsockfd){ // receive a signal and return 0 if the checksum is incorrect
  char reply[BUFFER_SIZE];
  int bytes_read;

  memset(&reply[0], 0, sizeof(reply));
  bytes_read = read(newsockfd, reply, sizeof(reply));
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
