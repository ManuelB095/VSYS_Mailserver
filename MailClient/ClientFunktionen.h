#ifndef CLIENTFUNKTIONEN_H
#define CLIENTFUNKTIONEN_H
#include <string>
#define BUF 1024

// Count elements in given char array
unsigned int count_elements(char* arr, unsigned int arr_len); /** Obsolete with strlen! **/

// Checks if given string contains only numbers
bool isNumerical(char * line);

// Hanbdle Numeric Input
int handle_NUMERIC_message(std::string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len = BUF);

// Handle alphanumeric input
int handle_ALPHANUMERIC_message(std::string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len = BUF);


/* Request Handlers */

// Handle the whole SEND-Request
int handle_SEND_request(int socketfd, char* buffer, int sender_len, int recipient_len, int subject_len, unsigned int buffer_MAX_len = BUF);

// Handle the whole READ-Request
int handle_READ_request(int socketfd, char* buffer, unsigned int user_len, unsigned int buffer_MAX_len = BUF);

#endif
