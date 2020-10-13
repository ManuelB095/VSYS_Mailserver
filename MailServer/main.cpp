/* myserver.c */
#include <iostream>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include "ServerFunktionen.h"
#define BUF 1024
#define PORT 6543 /** Not in Use anymore ! Will be entered as parameter by the User **/

/** Aufgabenstellung:

    Der Server wird mit einem Port und einem Verzeichnispfad (Mailspoolsverzeichnis) als Parameter gestartet.
    Er soll als iterativer Server ausgelegt werden ( keine gleichzeitigen Requests// wird aber in Zukunft verlangt!
    Server erkennt und reagiert auf folgende Requests:

    SEND: Senden einer Nachricht vom Client zum Server. ( Server antwortet mit OK\n oder ERR\n )
    LIST: Anzahl und Betreff-Zeile der Nachrichten sollen ausgegeben werden.
    READ: Anzeigen einer bestimmten Nachricht für den User.
    DEL:  Löschen einer Nachricht für den User.
    QUIT: Logout des Clients.

    **/

using std::string;

int main (int argc, char **argv ) {
  int create_socket, new_socket;
   //-----------LAB-------------
    std::string csvfile = "data.csv";
    //-------------------------
  socklen_t addrlen;
  char buffer[BUF];
  unsigned int port = -1; // In error case port = -1
  int size;
  struct sockaddr_in address, cliaddress;

  /* Edited by Natzki */
  if( argc < 2 ){
     printf("Usage: %s PortNumber\n", argv[0]);
     exit(EXIT_FAILURE);
  }
  else
  {
     std::string p(argv[1]); // Convert console argument to C++ String
     port = std::stoi(p); // String to Int.
  }

  if(port <= 1024 || port >= 65535)
  {
     printf("Port number %d not a valid PortNumber!\n", port);
     exit(EXIT_FAILURE);
  }

 /*  */
  create_socket = socket (AF_INET, SOCK_STREAM, 0);

  memset(&address,0,sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; // Automatically fill in IP Address this process is running on
  address.sin_port = htons (port); // well-known ports up till 1023; Ensure user port is somewhere above ( preferably in the thousands )

  if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
     perror("bind error");
     return EXIT_FAILURE;
  }
  listen (create_socket, 5);

  addrlen = sizeof (struct sockaddr_in);

  while (1) {
     printf("Waiting for connections...\n");
     new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
     if (new_socket > 0)
     {
        printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));
        strcpy(buffer,"Welcome to myserver, Please enter your command:\n");
        send(new_socket, buffer, strlen(buffer),0);
     }
     do {
        size = recv (new_socket, buffer, BUF-1, 0); // Returns -1 if connection was lost
        if( size > 0)
        {
           buffer[size] = '\0';
           std::stringstream client_message(buffer);
           string line = "";
           getline(client_message, line);

           if( line == "send" || line == "SEND" ) // => Handle SEND Request
           {
                string sender;
                string recipient;
                string subject_matter;
                string txt_message;
                getline(client_message, sender);
                getline(client_message, recipient);
                getline(client_message, subject_matter);
                while(getline(client_message, line))
                {
                    txt_message += line;
                    txt_message += '\n'; // Keeps original spacing with newlines this way
                }
                create_new_entry(csvfile, sender, recipient, subject_matter, txt_message);
                printf ("Message received: SEND-Request - OK \n");

           }
           if( line == "read" || line == "READ" ) // => Handle READ Request
           {
                string username;
                string temp;
                int message_nr = -1;
                getline(client_message, username);
                getline(client_message, temp); message_nr = std::stoi(temp);

                string sendback_buffer;
                printf ("Processing read request... \n");
                std::vector<std::string> requested_message = show_message(csvfile, username, message_nr);
                for(std::vector<std::string>::iterator i = requested_message.begin(); i != requested_message.end(); ++i)
                {
                    std::cout << *i << std::endl;
                    sendback_buffer += *i;
                    sendback_buffer += '\n';
                }

                send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()),0);

                /** TO DO: Compromise on a better way to handle that. Also: German/English - which one should we use?**/
                if(requested_message.back() == "Fehler. Sie haben nicht so viele Nachrichten")
                {
                    printf ("READ-Request - ERR \n");
                }
                else
                {
                    printf ("READ-Request - OK \n");
                }
           }
           if( line == "list" || line == "LIST" ) // => Handle LIST Request
           {
                string username;
                string temp;
                int message_count = -1;
                getline(client_message, username);

                string sendback_buffer;
                printf("Processing list request... \n");
                std::vector<std::string> topics_and_count = list_subjects_and_msgCount(csvfile, username);

                if(topics_and_count.back() == "-1") // If no messages found, show ERROR and END Request
                {
                    sendback_buffer = "No entries for user '"; sendback_buffer += username; sendback_buffer += "' found...";
                    printf ("LIST-Request - ERR \n");
                }
                else
                {
                    message_count = std::stoi(topics_and_count.back());
                    sendback_buffer = "Number of Messages: "; sendback_buffer += std::to_string(message_count); sendback_buffer += "\n";
                    send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()),0);

                    for(std::vector<std::string>::iterator i = topics_and_count.begin(); i != topics_and_count.end() - 1; ++i)
                    {
                        std::cout << *i << std::endl;
                        sendback_buffer = *i;
                        sendback_buffer += '\n';
                        send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()),0);
                    }
                }
           }

           if( line == "del" || line == "DEL" ) // => Handle DELETE Request
           {
                string username;
                string temp;
                int message_nr = -1;
                getline(client_message, username);
                getline(client_message, temp); message_nr = std::stoi(temp);

                string sendback_buffer;
                printf("Processing delete request... \n");
                bool hasDeleted = delete_message(csvfile, username, message_nr);
                if(hasDeleted)
                {
                  sendback_buffer = "Message of user '"; sendback_buffer += username; sendback_buffer += "' with Msg-Nr.: ";
                  sendback_buffer += std::to_string(message_nr); sendback_buffer += " was successfully deleted!\n";
                  send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()),0);
                  printf ("DEL-Request - OK \n");
                }
                else
                {
                  sendback_buffer = "Could not delete message of user '"; sendback_buffer += username; sendback_buffer += "' with Msg-Nr.: ";
                  sendback_buffer += std::to_string(message_nr); sendback_buffer += "!\n";
                  send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()),0);
                  printf ("DEL-Request - ERR \n");
                }
           }
       //------------------LAB------------------------------------------------
            //std::string message(buffer);                                         //aus den chars wird ein string erstellt
            //create_new_entry(csvfile, send_user, recive_user, subject, message); //die "write" Funktion wird aufgerufen

//            bool deleted = delete_message(csvfile, recive_user, 4);
//
//            //std::cout << "User hat " << count << " Nachrichten empfangen" << std::endl;
//            if (deleted)
//            {
//                std::cout << "Nachricht wurde geloescht" << std::endl;
//            }
        //----------------------------------------------------------
        }
        else if (size == 0)
        {
           printf("Client closed remote socket\n");
           break;
        }
        else
        {
           perror("recv error");
           return EXIT_FAILURE;
        }
     } while (strncmp (buffer, "quit", 4)  != 0);
     close (new_socket);
  }
  close (create_socket);
  return EXIT_SUCCESS;
}
