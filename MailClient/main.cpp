/* myclient.c */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
/* Edited by Natzki */
#include <string>
#include <algorithm>
#include "ClientFunktionen.h"
/* Edited by Natzki */
#include <string.h>
// #define BUF 1024
//#define PORT 6543

/** Aufgabenstellung:

    Client wird mit IP-Addresse und Port Parametern gestartet.
    Der Client connected via Stream-Sockets via IP/Port und schickt Requests an den Server.
    Folgende Requests sollen möglich sein:

    SEND: Senden einer Nachricht vom Client zum Server.
    LIST: Anzahl und Betreff-Zeile der Nachrichten sollen ausgegeben werden.
    READ: Anzeigen einer bestimmten Nachricht für den User.
    DEL:  Löschen einer Nachricht für den User.
    QUIT: Logout des Clients.

     **/

/* Edited by Natzki */
int main (int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;

  /* Edited by Natzki */
  unsigned int port = -1;
  std::string ip_address = "ERR";

  if( argc < 3 ){
     printf("Usage: %s IP-Address PortNumber\n", argv[0]);
     exit(EXIT_FAILURE);
  }
  else
  {
     std::string p(argv[2]); // Convert console argument to C++ String
     port = std::stoi(p); // String to Int.
  }
  /*  */

  if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
  {
     perror("Socket error");
     return EXIT_FAILURE;
  }

  /* Edited by Natzki */
  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons (port);
  if(strcmp(argv[1], "localhost") == 0)
    {ip_address = "127.0.0.1";}
  else
    {ip_address = argv[1];}
  const char* ip_address_c = ip_address.c_str();
  inet_aton (ip_address_c, &address.sin_addr); // Returns ZERO on failure ( unlike every other socket related function )! Takes IP in dotted form (a.b.c.d) and stores it in structure address.sin_addr ( Network Byte Order )
  /* Edited by Natzki */

  if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
  {
     printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
     size=recv(create_socket,buffer,BUF-1, 0);
     if (size>0)
     {
        buffer[size]= '\0';
        printf("%s",buffer);
     }
  }
  else
  {
     perror("Connect error - no server available");
     return EXIT_FAILURE;
  }

  do {
     printf ("Send message: ");

     /* Edited by Natzki */

     /** SEND - FUNCTION: **/

     // SEND\n
     fgets (buffer, BUF, stdin); // Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.
     if(strcmp(buffer, "SEND\n") == 0 || strcmp(buffer, "send\n") == 0)
     {
        if(handle_SEND_request(create_socket, buffer, 8, 8, 80) == -1)
        {
            memset(&buffer,'\0',sizeof(buffer)); // Reset buffer to ZERO
            perror("SEND Request not successful! Abort...");
            continue; // Continue with loop
        }
        printf("Successfully sent message!\n");
     }
     if(strcmp(buffer, "READ\n") == 0 || strcmp(buffer, "read\n") == 0)
     {
        if(handle_READ_request(create_socket, buffer, 8) == -1)
        {
            memset(&buffer,'\0',sizeof(buffer)); // Reset buffer to ZERO
            perror("READ Request not successful! Abort...");
            continue; // Continue with loop
        }
        printf("Successfully sent READ-Request!\n Waiting on server...\n");

        // Response from Server:
        printf("READ: \n");
        memset(&buffer,'\0',sizeof(buffer));
        size=recv(create_socket,buffer,BUF-1, 0);
        if (size>0)
        {
           buffer[size]= '\0';
           printf("%s",buffer);
        }
     }
      if(strcmp(buffer, "LIST\n") == 0 || strcmp(buffer, "list\n") == 0)
     {
        if(handle_LIST_request(create_socket, buffer, 8) == -1)
        {
            memset(&buffer,'\0',sizeof(buffer)); // Reset buffer to ZERO
            perror("READ Request not successful! Abort...");
            continue; // Continue with loop
        }
        printf("Successfully sent LIST-Request!\n Waiting on server...\n");

        // Response from Server:
        printf("LIST: \n");
        memset(&buffer,'\0',sizeof(buffer));

        size = recv(create_socket, buffer, BUF-1, 0);
        buffer[size] = '\0';
        printf("%s", buffer);

        /* Somehow only works, if client is restarted. Could not figure out exact problem yet.
           To accomodate this for now: Changed Server-Code to single SEND for this single RECV */
//        do
//        {
//            size = recv(create_socket, buffer, BUF-1, 0);
//            buffer[size] = '\0';
//            printf("%s", buffer);
//            memset(&buffer,'\0',sizeof(buffer));
//
//        }while(strcmp(buffer, ">>TERMINATE<<") != 0);
     }
     if(strcmp(buffer, "DEL\n") == 0 || strcmp(buffer, "del\n") == 0)
     {
        if(handle_DEL_request(create_socket, buffer, 8) == -1)
        {
            memset(&buffer,'\0',sizeof(buffer)); // Reset buffer to ZERO
            perror("DEL Request not successful! Abort...");
            continue; // Continue with loop
        }
        printf("Successfully sent DEL-Request!\n Waiting on server...\n");

        // Response from Server:
        printf("DEL: \n");
        memset(&buffer,'\0',sizeof(buffer));
        size=recv(create_socket,buffer,BUF-1, 0);
        if (size>0)
        {
           buffer[size]= '\0';
           printf("%s",buffer);
        }
     }
     /* Edited by Natzki */
  }
  while (strcmp (buffer, "quit\n") != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}
