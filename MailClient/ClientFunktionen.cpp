#include "ClientFunktionen.h"
#include <algorithm>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


/* Function Block */

unsigned int count_elements(char* arr, unsigned int arr_len) // Obsolete with strlen
{
    unsigned int counter = 0;
    while(arr[counter] != '\0' && arr[counter] != '\n')
    {
        ++counter;
    }
    return counter;
}

bool isNumerical(char* line)
{
    std::string temp(line);

    for( char const &s: temp)
    {
        if(s != '\n' && (s < '0' || s > '9'))
        {return false;}
    }
    return true;
}

int handle_NUMERIC_message(std::string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len /*= BUF*/)
{
    printf("%s", message.c_str());
    char temp_buf[buffer_MAX_len] = {'\0'};
    fgets(temp_buf, buffer_MAX_len, stdin);

    while(!isNumerical(temp_buf))
    {
        printf("Your input contained non-numerical characters!");
        printf("%s", message.c_str());
        memset(temp_buf, '\0', buffer_MAX_len*sizeof(char));
        fgets(temp_buf, buffer_MAX_len, stdin);
    }

    if( strlen(temp_buf) > input_len) // For example: sender has 8-character limit
    {
        printf("Input exceeds character limit! Abort...\n");
        return -1; // Consistent with other socket error functions
    }
    strcat(buffer, temp_buf);
    return 0;
}

int handle_ALPHANUMERIC_message(std::string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len /*= BUF*/)
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
    std::string message = "Please enter sender (max. ";
    message += std::to_string(sender_len); message += " characters): ";
    if(handle_ALPHANUMERIC_message(message, socketfd, buffer, sender_len) == -1)
    {
        return -1;
    }

    // Step 2: Ask for recipient
    message = "Please enter recipient (max. ";
    message += std::to_string(recipient_len); message += " characters): ";
    if(handle_ALPHANUMERIC_message(message, socketfd, buffer, recipient_len) == -1)
    {
        return -1;
    }

    // Step 3: Ask for message-subject
    message = "Please enter the subject-matter of your Mail (max. ";
    message += std::to_string(subject_len); message += " characters): ";
    if(handle_ALPHANUMERIC_message(message, socketfd, buffer, subject_len) == -1)
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

int handle_READ_request(int socketfd, char* buffer, unsigned int user_len, unsigned int buffer_MAX_len /*= BUF*/)
{
    // Communicate SEND Request to Server, so he knows what to expect!
    memset(buffer, '\0', buffer_MAX_len*sizeof(char));
    strcat(buffer, "READ\n");

    // Step 1: Ask for Username
    std::string message = "Please enter username (max. ";
    message += std::to_string(user_len); message += " characters): ";
    if(handle_ALPHANUMERIC_message(message, socketfd, buffer, user_len) == -1)
    {
        return -1;
    }

    // Step 2: Ask for Message-Number ( Included digit size, because buffer can only hold 1024 characters and last one must be '\0'
    message = "Please enter the message-number (max. ";
    int input_len = buffer_MAX_len - user_len;
    message += std::to_string(input_len); message += " digits): ";
    if(handle_NUMERIC_message(message, socketfd, buffer, input_len) == -1)
    {
        return -1;
    }

    // Step 3: Send the whole message:
    send(socketfd, buffer, strlen(buffer), 0);

    return 0;
}
