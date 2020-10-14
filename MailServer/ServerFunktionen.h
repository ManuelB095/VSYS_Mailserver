#ifndef SERVERFUNKTIONEN_H
#define SERVERFUNKTIONEN_H
#include <iostream>
#include <ostream>
#include <string>

//neue nachricht empfangen
void create_new_entry(std::string csvfile, std::string send_user, std::string recive_user, std::string subject, std::string message);

//wie viele Nachrichten hab ich empfangen, wie lauten die Betreffs
std::vector<std::string> search(std::string csvfile, std::string recive_user);

//anzeigen einer bestimmten Nachricht
std::vector<std::string> show_message(std::string csvfile, std::string recive_user, int file_number);

// gibt Anzahl der Nachrichten eines gewissen Users zurück
int count_messages(std::string csvfile, std::string recive_user);

std::vector<std::string> list_subjects_and_msgCount(std::string csvfile, std::string recive_user);


//löschen einer Nachricht
bool delete_message(std::string csvfile, std::string recive_user, int file_number);

#endif
