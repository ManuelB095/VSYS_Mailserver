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
/* Edited by Natzki */
#include <string.h>
#define BUF 1024
#define PORT 6543

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
using std::string;

unsigned int count_elements(char* arr, unsigned int arr_len); /** Obsolete! Could have just used strlen! **/
int handle_SEND_message(string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len = BUF);
int handle_SEND_request(int socketfd, char* buffer, int sender_len, int recipient_len, int subject_len, unsigned int buffer_MAX_len = BUF);








/* Edited by Natzki */


int main (int argc, char **argv) {
  int create_socket;
  char buffer[BUF];
  struct sockaddr_in address;
  int size;

  /* Edited by Natzki */
  unsigned int port = -1;
  string ip_address = "ERR";

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
     /* Edited by Natzki */
  }
  while (strcmp (buffer, "quit\n") != 0);
  close (create_socket);
  return EXIT_SUCCESS;
}

/* Edited by Natzki */

/* Function Block */

unsigned int count_elements(char* arr, unsigned int arr_len)
{
    unsigned int counter = 0;
    while(arr[counter] != '\0' && arr[counter] != '\n')
    {
        ++counter;
    }
    return counter;
}

int handle_SEND_message(string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len /*= BUF*/)
{
    printf("%s", message.c_str()); // Some kind of unholy C/C++ abomination; but wanted to be consistent with printf instead of using cout
    char temp_buf[buffer_MAX_len] = {'\0'}; // Bit useless with so many chars... could just make input_len + 1 ?
    fgets(temp_buf, buffer_MAX_len, stdin);

    if( strlen(temp_buf) > input_len) // For example: sender has 8-character limit
    {
        printf("Input exceeds character limit! Abort...\n");
        return -1; // Consistent with other socket error functions
    }
    strcat(buffer, temp_buf);
    return 0;
}

int handle_SEND_request(int socketfd, char* buffer, int sender_len, int recipient_len, int subject_len, unsigned int buffer_MAX_len /*= BUF*/)
{

    // Communicate SEND Request to Server, so he knows what to expect!
    memset(buffer, '\0', buffer_MAX_len*sizeof(char));
    strcat(buffer, "SEND\n");

    // Step 1: Ask for sender
    string message = "Please enter sender (max. ";
    message += std::to_string(sender_len); message += " characters): ";
    if(handle_SEND_message(message, socketfd, buffer, sender_len) == -1)
    {
        return -1;
    }

    // Step 2: Ask for recipient
    message = "Please enter recipient (max. ";
    message += std::to_string(recipient_len); message += " characters): ";
    if(handle_SEND_message(message, socketfd, buffer, recipient_len) == -1)
    {
        return -1;
    }

    // Step 3: Ask for message-subject
    message = "Please enter the subject-matter of your Mail (max. ";
    message += std::to_string(subject_len); message += " characters): ";
    if(handle_SEND_message(message, socketfd, buffer, subject_len) == -1)
    {
        return -1;
    }

    // Step 4: Ask for message-body
    message = "Please enter your message (max. ";
    int message_len = buffer_MAX_len - sender_len - recipient_len - subject_len;
    message += std::to_string(message_len); message += " characters)\n Type '.' , then press ENTER to end your message:\n";
    printf("%s", message.c_str()); // Some kind of unholy C/C++ abomination; but wanted to be consistent with printf instead of using cout
    char temp_buffer[buffer_MAX_len] = {'\0'};
    do // Writes to buffer until either '.\n' or buffer is full, which causes an error message to display
    {
        fgets(temp_buffer, buffer_MAX_len, stdin);
        if(strlen(temp_buffer) + strlen(buffer) <= 1024)
            {strcat(buffer,temp_buffer);}
        else
        {
            printf("Input exceeds character limit! Abort...\n");
            return -1;
        }
    }while(buffer[strlen(buffer)-2] != '.');

    // Step 5: Send the whole message:
    send(socketfd, buffer, strlen(buffer), 0);

    return 0;
}

/* Edited by Natzki */




