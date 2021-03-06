#ifndef CLIENTFUNKTIONEN_H
#define CLIENTFUNKTIONEN_H
#include <string>
#define BUF 1024

void newline();

// For debugging Purposes: Waits until given time, so multiple clients can connect simultaneously
bool waitUntil(time_t w_time);

// Checks if given string contains only numbers
bool isNumerical(char * line);

// Get Password from User ( disables Console echo => Password invisible )
const char* getpass();

// Hanbdle Numeric Input
int handle_NUMERIC_message(std::string message, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len = BUF);

// Handle alphanumeric input
int handle_ALPHANUMERIC_message(std::string message, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len = BUF);


/* Request Handlers */

// Handle the whole LOGIN Request
int handle_LOGIN_request(int socketfd, char* buffer, std::string &username, unsigned int buffer_MAX_len = BUF);

// Handle the whole SEND-Request
int handle_SEND_request(int socketfd, char* buffer, const std::string &username, int recipient_len, int subject_len, unsigned int buffer_MAX_len = BUF);

// Handle the whole READ-Request
int handle_READ_request(int socketfd, char* buffer, const std::string &username, unsigned int buffer_MAX_len = BUF);

// Handle the whole LIST-Request
int handle_LIST_request(int socketfd, char* buffer, const std::string &username, unsigned int buffer_MAX_len = BUF);

// Handle the whole DEL-Request
int handle_DEL_request(int socketfd, char* buffer, const std::string &username, unsigned int buffer_MAX_len = BUF);



#endif
