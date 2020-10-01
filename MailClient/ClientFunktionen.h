#ifndef CLIENTFUNKTIONEN_H
#define CLIENTFUNKTIONEN_H
#include <string>
#define BUF 1024

unsigned int count_elements(char* arr, unsigned int arr_len); /** Obsolete! Could have just used strlen! **/
int handle_SEND_message(std::string message, int socketfd, char* buffer, unsigned int input_len, unsigned int buffer_MAX_len = BUF);
int handle_SEND_request(int socketfd, char* buffer, int sender_len, int recipient_len, int subject_len, unsigned int buffer_MAX_len = BUF);

#endif
