/* myserver.c */
#include <ldap.h>
#include <iostream>
#include <vector>
#include <thread>
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


#include <fstream>
#include <sys/stat.h>

#include "ServerFunktionen.h"

#define BUF 1024
#define MAX_THREADS 10
#define WAIT 1800 // Seconds to wait, before a new connection can be established again.
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

void HandleClient(int new_socket);

int main(int argc, char **argv)
{
   mkdir("MessageFolder", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creats the MessageFolder if it does not exist

   int create_socket,
       new_socket;

   socklen_t addrlen;
   char buffer[BUF];
   unsigned int port = -1; // In error case port = -1
   /*int size*/;
   struct sockaddr_in address, cliaddress;

   /*time_t nextTry = time(0);*/

   std::vector<std::thread>server_threads;
   int num_of_threads = 0;


   ////////////////////////////////////////////////////////////////////////////
   // LDAP config
   // anonymous bind with user and pw empty
   /*const char *ldapUri = "ldap://ldap.technikum-wien.at:389";
   const int ldapVersion = LDAP_VERSION3;*/

   // read username (bash: export ldapuser=<yourUsername>)
   /*char ldapBindUser[256];
   char ldapBindPassword[256];
   char rawLdapUser[128];*/

   /*bool isLoggedIn = false;
   int login_attempts = 0;*/ // Needs multiple of these with threading!


   ////////////////////////////////////////////////////////////////////////////

   if (argc < 2)
   {
      printf("Usage: %s PortNumber\n", argv[0]);
      exit(EXIT_FAILURE);
   }
   else
   {
      std::string p(argv[1]); // Convert console argument to C++ String
      port = std::stoi(p);    // String to Int.
   }

   if (port <= 1024 || port >= 65535)
   {
      printf("Port number %d not a valid PortNumber!\n", port);
      exit(EXIT_FAILURE);
   }

   create_socket = socket(AF_INET, SOCK_STREAM, 0);

   memset(&address, 0, sizeof(address));
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY; // Automatically fill in IP Address this process is running on
   address.sin_port = htons(port);       // well-known ports up till 1023; Ensure user port is somewhere above ( preferably in the thousands )

   if (bind(create_socket, (struct sockaddr *)&address, sizeof(address)) != 0)
   {
      perror("bind error");
      return EXIT_FAILURE;
   }
   listen(create_socket, 5);

   addrlen = sizeof(struct sockaddr_in);

   while (1)
   {
      printf("Waiting for connections...\n");
      new_socket = accept(create_socket, (struct sockaddr *)&cliaddress, &addrlen);
      if (new_socket < 0 || num_of_threads >= 10)
      {
         strcpy(buffer, "Refused Connection!");
         send(new_socket, buffer, strlen(buffer), 0);
         printf("Refused Connection from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
         close(new_socket);
         if(num_of_threads >= 0)
            num_of_threads--;
      }
      else if (new_socket > 0 && num_of_threads < 10)
      {
      /*----------------------------------------------------------------------------------------------------------------------------------------- */
      /** This is where Thread should start! Parameters: new_socket Variables needed from above: int size; char buffer[BUF]; Keep track of Client: Idk, IP? But we only connect with localhost...**/
      /*----------------------------------------------------------------------------------------------------------------------------------------- */
         printf("Client connected from %s:%d...\n", inet_ntoa(cliaddress.sin_addr), ntohs(cliaddress.sin_port));
         num_of_threads++;
         std::thread new_thread(HandleClient, new_socket);
         new_thread.detach();
         server_threads.push_back(std::move(new_thread));

        // Not sure if actually needed, since Threads are independent of one another?
         for (std::thread & th : server_threads)
         {
            if (th.joinable())
            {
               th.join();
               num_of_threads--;
            }

         }
      }

//      do
//      {
//        // TOO many LOGIN attempts -> Set Timer for next Try
//         if(login_attempts >= 3)
//         {
//            printf("Too many attempts made. Closing connection...\n");
//            nextTry = time(0) + WAIT; // This makes a kind of barebones "Timestamp" to check whether client can try reconnecting again.
//            string sendback_buffer = "Server refused connection due to invalid LOGIN attempts. Please try again later";
//            send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//
//            /* CLient closes after receiving this message !*/
//
//         }
//        /*----------------------------------------------------------------------------------------------------------------------------------------- */
//        /** TO DO: Remember Client IP for Timeout! **/
//        /*----------------------------------------------------------------------------------------------------------------------------------------- */
//         time_t now = time(0);
//         if(now < nextTry)
//         {
//            string sendback_buffer = "Timeout still active. Please try again later.";
//            send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//         }
//
//         size = recv(new_socket, buffer, BUF - 1, 0); // Returns -1 if connection was lost
//         if (size > 0)
//         {
//            buffer[size] = '\0';
//            std::stringstream client_message(buffer);
//            string line = "";
//            getline(client_message, line);
//
//            /** LDAP Login **/
//            if (line == "login" || line == "LOGIN")
//            {
//                string username;
//                string passwd;
//
//                getline(client_message, username);
//                getline(client_message, passwd);
//
//                strcpy(rawLdapUser,username.c_str());
//                strcpy(ldapBindPassword, passwd.c_str());
//                sprintf(ldapBindUser, "uid=%s,ou=people,dc=technikum-wien,dc=at", rawLdapUser);
//                printf("user set to: %s\n", ldapBindUser);
//
//               // general
//               int rc = 0; // return code
//
//               // setup LDAP connection
//               // https://linux.die.net/man/3/ldap_initialize
//               LDAP *ldapHandle;
//               rc = ldap_initialize(&ldapHandle, ldapUri);
//               if (rc != LDAP_SUCCESS)
//               {
//                  fprintf(stderr, "ldap_init failed\n");
//                  continue;
//               }
//               printf("connected to LDAP server %s\n", ldapUri);
//
//               rc = ldap_set_option(
//                   ldapHandle,
//                   LDAP_OPT_PROTOCOL_VERSION, // OPTION
//                   &ldapVersion);             // IN-Value
//               if (rc != LDAP_OPT_SUCCESS)
//               {
//                  fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n", ldap_err2string(rc));
//                  ldap_unbind_ext_s(ldapHandle, NULL, NULL);
//                  continue;
//               }
//
//               rc = ldap_start_tls_s(
//                   ldapHandle,
//                   NULL,
//                   NULL);
//               if (rc != LDAP_SUCCESS)
//               {
//                  fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
//                  ldap_unbind_ext_s(ldapHandle, NULL, NULL);
//                  continue;
//               }
//
//               BerValue bindCredentials;
//               bindCredentials.bv_val = (char *)ldapBindPassword;
//               bindCredentials.bv_len = strlen(ldapBindPassword);
//               BerValue *servercredp; // server's credentials
//               rc = ldap_sasl_bind_s(
//                   ldapHandle,
//                   ldapBindUser,
//                   LDAP_SASL_SIMPLE,
//                   &bindCredentials,
//                   NULL,
//                   NULL,
//                   &servercredp);
//               if (rc != LDAP_SUCCESS)
//               {
//                  fprintf(stderr, "LDAP bind error: %s\n", ldap_err2string(rc));
//                  ldap_unbind_ext_s(ldapHandle, NULL, NULL);
//                  login_attempts++;
//                  string sendback_buffer = "LOGIN-ERR\n";
//                  send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//                  continue;
//               }
//
//               // If BIND was successful => Signal Client the OK
//               isLoggedIn = true;
//               string sendback_buffer = "LOGIN-OK\n";
//               send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//
//               ldap_unbind_ext_s(ldapHandle, NULL, NULL);
//               continue;
//            }
//
//            if(isLoggedIn)
//            {
//                if (line == "send" || line == "SEND") // => Handle SEND Request
//                {
//                   string sender;
//                   string recipient;
//                   string subject_matter;
//                   string txt_message;
//                   getline(client_message, sender);
//                   getline(client_message, recipient);
//                   getline(client_message, subject_matter);
//                   while (getline(client_message, line))
//                   {
//                      txt_message += line;
//                      txt_message += '\n'; // Keeps original spacing with newlines this way
//                   }
//                   create_new_entry(sender, recipient, subject_matter, txt_message);
//                   string sendback_buffer = "SEND-Request - SUCCESSFUL\n";
//                   send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//                   printf("Message received: SEND-Request - OK \n");
//                }
//                if (line == "read" || line == "READ") // => Handle READ Request
//                {
//                   string username;
//                   string temp;
//                   int message_nr = -1;
//                   getline(client_message, username);
//                   getline(client_message, temp);
//                   message_nr = std::stoi(temp);
//
//                   string sendback_buffer;
//                   printf("Processing read request... \n");
//                   std::vector<std::string> requested_message = show_message(username, message_nr);
//                   for (std::vector<std::string>::iterator i = requested_message.begin(); i != requested_message.end(); ++i)
//                   {
//                      std::cout << *i << std::endl;
//                      sendback_buffer += *i;
//                      sendback_buffer += '\n';
//                   }
//
//                   send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//
//                   /** TO DO: Compromise on a better way to handle that. Also: German/English - which one should we use?**/
//                   if (requested_message.back() == "Fehler. Sie haben nicht so viele Nachrichten")
//                   {
//                      printf("READ-Request - ERR \n");
//                   }
//                   else
//                   {
//                      printf("READ-Request - OK \n");
//                   }
//                }
//                if (line == "list" || line == "LIST") // => Handle LIST Request
//                {
//                   string username;
//                   string temp;
//                   //int message_count = -1; Unused
//                   getline(client_message, username);
//                   std::cout << username << std::endl;
//                   string sendback_buffer;
//                   printf("Processing list request... \n");
//                   //std::vector<std::string> topics_and_count = list_subjects_and_msgCount(csvfile, username);
//                   std::vector<std::string> topics_and_count = list_subjects_and_msgCount(username); //new list_subjects_and_msgCount function
//
//                   if (topics_and_count[0] == "-1") // If no messages found, show ERROR and END Request
//                   {
//                      sendback_buffer = "No entries for user '";
//                      sendback_buffer += username;
//                      sendback_buffer += "' found... \n";
//                      printf("LIST-Request - ERR \n");
//                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//                   }
//                   else
//                   {
//                      for (std::string i : topics_and_count)
//                      {
//                         std::cout << i << std::endl;
//                         sendback_buffer += "<";
//                         sendback_buffer += i;
//                         sendback_buffer += ">:";
//                         sendback_buffer += "\n";
//                      }
//                      /* A single send instead of n-consecutive ones, since clien seems to have trouble receiving the SEND Requests in a quick fashion.*/
//                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//                   }
//                }
//
//                if (line == "del" || line == "DEL") // => Handle DELETE Request
//                {
//                   string username;
//                   string temp;
//                   int message_nr = -1;
//                   getline(client_message, username);
//                   getline(client_message, temp);
//                   message_nr = std::stoi(temp);
//
//                   string sendback_buffer;
//                   printf("Processing delete request... \n");
//                   bool hasDeleted = delete_message(username, message_nr);
//                   if (hasDeleted)
//                   {
//                      sendback_buffer = "Message of user '";
//                      sendback_buffer += username;
//                      sendback_buffer += "' with Msg-Nr.: ";
//                      sendback_buffer += std::to_string(message_nr);
//                      sendback_buffer += " was successfully deleted!\n";
//                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//                      printf("DEL-Request - OK \n");
//                   }
//                   else
//                   {
//                      sendback_buffer = "Could not delete message of user '";
//                      sendback_buffer += username;
//                      sendback_buffer += "' with Msg-Nr.: ";
//                      sendback_buffer += std::to_string(message_nr);
//                      sendback_buffer += "!\n";
//                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//                      printf("DEL-Request - ERR \n");
//                   }
//                }
//            }
//            else if(!isLoggedIn)
//            {
//                printf("Unauthorized Command - User not logged in!");
//                string sendback_buffer;
//                sendback_buffer = "ERR\n";
//                send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
//            }
//         }
//         else if (size == 0)
//         {
//            printf("Client closed remote socket\n");
//            break;
//         }
//         else
//         {
//            perror("recv error");
//            return EXIT_FAILURE;
//         }
//      } while (strncmp(buffer, "quit", 4) != 0 && login_attempts <= 3);
//      close(new_socket);
   }
   close(create_socket);
   return EXIT_SUCCESS;
}


void HandleClient(int new_socket)
{
    int size;
    char buffer[BUF];

     ////////////////////////////////////////////////////////////////////////////
   // LDAP config
   // anonymous bind with user and pw empty
   const char *ldapUri = "ldap://ldap.technikum-wien.at:389";
   const int ldapVersion = LDAP_VERSION3;

   // read username (bash: export ldapuser=<yourUsername>)
   char ldapBindUser[256];
   char ldapBindPassword[256];
   char rawLdapUser[128];

   time_t nextTry = time(0);
   bool isLoggedIn = false;
   int login_attempts = 0; // Needs multiple of these with threading!

    strcpy(buffer, "Welcome to myserver, Please enter your command:\n");
    send(new_socket, buffer, strlen(buffer), 0);

    do
      {
        // TOO many LOGIN attempts -> Set Timer for next Try
         if(login_attempts >= 3)
         {
            printf("Too many attempts made. Closing connection...\n");
            nextTry = time(0) + WAIT; // This makes a kind of barebones "Timestamp" to check whether client can try reconnecting again.
            string sendback_buffer = "Server refused connection due to invalid LOGIN attempts. Please try again later";
            send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);

            /* CLient closes after receiving this message !*/

         }
        /*----------------------------------------------------------------------------------------------------------------------------------------- */
        /** TO DO: Remember Client IP for Timeout! **/
        /*----------------------------------------------------------------------------------------------------------------------------------------- */
         time_t now = time(0);
         if(now < nextTry)
         {
            string sendback_buffer = "Timeout still active. Please try again later.";
            send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
         }

         size = recv(new_socket, buffer, BUF - 1, 0); // Returns -1 if connection was lost
         if (size > 0)
         {
            buffer[size] = '\0';
            std::stringstream client_message(buffer);
            string line = "";
            getline(client_message, line);

            /** LDAP Login **/
            if (line == "login" || line == "LOGIN")
            {
                string username;
                string passwd;

                getline(client_message, username);
                getline(client_message, passwd);

                strcpy(rawLdapUser,username.c_str());
                strcpy(ldapBindPassword, passwd.c_str());
                sprintf(ldapBindUser, "uid=%s,ou=people,dc=technikum-wien,dc=at", rawLdapUser);
                printf("user set to: %s\n", ldapBindUser);

               // general
               int rc = 0; // return code

               // setup LDAP connection
               // https://linux.die.net/man/3/ldap_initialize
               LDAP *ldapHandle;
               rc = ldap_initialize(&ldapHandle, ldapUri);
               if (rc != LDAP_SUCCESS)
               {
                  fprintf(stderr, "ldap_init failed\n");
                  continue;
               }
               printf("connected to LDAP server %s\n", ldapUri);

               rc = ldap_set_option(
                   ldapHandle,
                   LDAP_OPT_PROTOCOL_VERSION, // OPTION
                   &ldapVersion);             // IN-Value
               if (rc != LDAP_OPT_SUCCESS)
               {
                  fprintf(stderr, "ldap_set_option(PROTOCOL_VERSION): %s\n", ldap_err2string(rc));
                  ldap_unbind_ext_s(ldapHandle, NULL, NULL);
                  continue;
               }

               rc = ldap_start_tls_s(
                   ldapHandle,
                   NULL,
                   NULL);
               if (rc != LDAP_SUCCESS)
               {
                  fprintf(stderr, "ldap_start_tls_s(): %s\n", ldap_err2string(rc));
                  ldap_unbind_ext_s(ldapHandle, NULL, NULL);
                  continue;
               }

               BerValue bindCredentials;
               bindCredentials.bv_val = (char *)ldapBindPassword;
               bindCredentials.bv_len = strlen(ldapBindPassword);
               BerValue *servercredp; // server's credentials
               rc = ldap_sasl_bind_s(
                   ldapHandle,
                   ldapBindUser,
                   LDAP_SASL_SIMPLE,
                   &bindCredentials,
                   NULL,
                   NULL,
                   &servercredp);
               if (rc != LDAP_SUCCESS)
               {
                  fprintf(stderr, "LDAP bind error: %s\n", ldap_err2string(rc));
                  ldap_unbind_ext_s(ldapHandle, NULL, NULL);
                  login_attempts++;
                  string sendback_buffer = "LOGIN-ERR\n";
                  send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
                  continue;
               }

               // If BIND was successful => Signal Client the OK
               isLoggedIn = true;
               string sendback_buffer = "LOGIN-OK\n";
               send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);

               ldap_unbind_ext_s(ldapHandle, NULL, NULL);
               continue;
            }

            if(isLoggedIn)
            {
                if (line == "send" || line == "SEND") // => Handle SEND Request
                {
                   string sender;
                   string recipient;
                   string subject_matter;
                   string txt_message;
                   getline(client_message, sender);
                   getline(client_message, recipient);
                   getline(client_message, subject_matter);
                   while (getline(client_message, line))
                   {
                      txt_message += line;
                      txt_message += '\n'; // Keeps original spacing with newlines this way
                   }
                   create_new_entry(sender, recipient, subject_matter, txt_message);
                   string sendback_buffer = "SEND-Request - SUCCESSFUL\n";
                   send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
                   printf("Message received: SEND-Request - OK \n");
                }
                if (line == "read" || line == "READ") // => Handle READ Request
                {
                   string username;
                   string temp;
                   int message_nr = -1;
                   getline(client_message, username);
                   getline(client_message, temp);
                   message_nr = std::stoi(temp);

                   string sendback_buffer;
                   printf("Processing read request... \n");
                   std::vector<std::string> requested_message = show_message(username, message_nr);
                   for (std::vector<std::string>::iterator i = requested_message.begin(); i != requested_message.end(); ++i)
                   {
                      std::cout << *i << std::endl;
                      sendback_buffer += *i;
                      sendback_buffer += '\n';
                   }

                   send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);

                   /** TO DO: Compromise on a better way to handle that. Also: German/English - which one should we use?**/
                   if (requested_message.back() == "Fehler. Sie haben nicht so viele Nachrichten")
                   {
                      printf("READ-Request - ERR \n");
                   }
                   else
                   {
                      printf("READ-Request - OK \n");
                   }
                }
                if (line == "list" || line == "LIST") // => Handle LIST Request
                {
                   string username;
                   string temp;
                   //int message_count = -1; Unused
                   getline(client_message, username);
                   std::cout << username << std::endl;
                   string sendback_buffer;
                   printf("Processing list request... \n");
                   //std::vector<std::string> topics_and_count = list_subjects_and_msgCount(csvfile, username);
                   std::vector<std::string> topics_and_count = list_subjects_and_msgCount(username); //new list_subjects_and_msgCount function

                   if (topics_and_count[0] == "-1") // If no messages found, show ERROR and END Request
                   {
                      sendback_buffer = "No entries for user '";
                      sendback_buffer += username;
                      sendback_buffer += "' found... \n";
                      printf("LIST-Request - ERR \n");
                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
                   }
                   else
                   {
                      for (std::string i : topics_and_count)
                      {
                         std::cout << i << std::endl;
                         sendback_buffer += "<";
                         sendback_buffer += i;
                         sendback_buffer += ">:";
                         sendback_buffer += "\n";
                      }
                      /* A single send instead of n-consecutive ones, since clien seems to have trouble receiving the SEND Requests in a quick fashion.*/
                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
                   }
                }

                if (line == "del" || line == "DEL") // => Handle DELETE Request
                {
                   string username;
                   string temp;
                   int message_nr = -1;
                   getline(client_message, username);
                   getline(client_message, temp);
                   message_nr = std::stoi(temp);

                   string sendback_buffer;
                   printf("Processing delete request... \n");
                   bool hasDeleted = delete_message(username, message_nr);
                   if (hasDeleted)
                   {
                      sendback_buffer = "Message of user '";
                      sendback_buffer += username;
                      sendback_buffer += "' with Msg-Nr.: ";
                      sendback_buffer += std::to_string(message_nr);
                      sendback_buffer += " was successfully deleted!\n";
                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
                      printf("DEL-Request - OK \n");
                   }
                   else
                   {
                      sendback_buffer = "Could not delete message of user '";
                      sendback_buffer += username;
                      sendback_buffer += "' with Msg-Nr.: ";
                      sendback_buffer += std::to_string(message_nr);
                      sendback_buffer += "!\n";
                      send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
                      printf("DEL-Request - ERR \n");
                   }
                }
            }
            else if(!isLoggedIn)
            {
                printf("Unauthorized Command - User not logged in!");
                string sendback_buffer;
                sendback_buffer = "ERR\n";
                send(new_socket, sendback_buffer.c_str(), strlen(sendback_buffer.c_str()), 0);
            }
         }
         else if (size == 0)
         {
            printf("Client closed remote socket\n");
            break;
         }
         else
         {
            perror("recv error");
            return /*EXIT_FAILURE*/;
         }
      } while (strncmp(buffer, "quit", 4) != 0 && login_attempts <= 3);
      close(new_socket);
      return /*EXIT_SUCCESS*/;
   }












