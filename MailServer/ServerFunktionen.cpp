#include <iostream>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include <ctime>   //wird für den timestamp benötigt um ein eindeutiges txt file zu erstellen
#include <stdio.h> //for remove( ) and rename( )
#include <sys/types.h>
#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <sstream>
#include "ServerFunktionen.h"

//---------------Neuen csv Eintrag erstellen und neues txt File für die Nachricht erstellen und Nachricht hineinschreiben-------------------
/*void create_new_entry(std::string csvfile, std::string send_user, std::string recive_user, std::string subject, std::string message)
{

	//zuerst wird ein Timestamp erstellt
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y%H:%M:%S", timeinfo);
	std::string date(buffer);

	//Danach wird der Filename aus dem send_user, dem Time Stamp und der Endung ".txt" erstellt
	std::string filename = send_user + date + ".txt";

	//Dann wird das File selbst erstellt und die message wird darin eingespeichert und Datei wird wieder geschlossen
	std::ofstream textdata(filename);
	textdata << message << std::endl;
	textdata.close();

	//Nun wird im csv File der neue Eintrag eingefügt
	//message = message.substr(0, text.size()-1); //hier wird das "/0" am ende des gesendeten Textes abgeschnitten
	std::fstream entry;																			//Objekt "file" wird erstellt, dass in das File schreibt
	entry.open(csvfile, std::ios::out | std::ios::app);											//Datei wird geoeffnet "ios_base::app" wird ben�tigt, damit daten in File nicht �berschrieben werden
	entry << send_user << "," << recive_user << "," << subject << "," << filename << std::endl; //Text wird ins File eingeschrieben
	entry.close();
}*/

//----------------------------empfangene Nachrichten suchen und die Betreffs zurückgeben----------------------------------
/*std::vector<std::string> search(std::string csvfile, std::string recive_user)
{
	std::vector<std::string> mysubjects;
	std::string line;
	std::ifstream entry(csvfile);

	while (getline(entry, line, ','))
	{
		getline(entry, line, ',');
		if (line == recive_user)
		{
			getline(entry, line, ',');
			mysubjects.push_back(line);
		}
		getline(entry, line);
	}

	entry.close();
	return mysubjects;
}*/

//--------------------------anzeigen einer bestimmten Nachricht-----------------------
/*std::vector<std::string> show_message(std::string csvfile, std::string recive_user, int file_number)
{

	std::vector<std::string> my_message; //hier wird der Text gespeichert
	std::string line;
	std::ifstream entry(csvfile);
	std::string text_part; //Hier werden die einzelnen Zeilen des Textes gespeichert, bevor sie dem Vector übergeben werden

	int counter = 0;				  //Um zu checken bei der wiefilenten Nachricht wir sind
	while (getline(entry, line, ',')) //sender User
	{

		getline(entry, line, ','); //Empfänger User
		if (line == recive_user)   //check ob Empfänger User der ist, der Abfragt
		{
			counter++;
			if (counter == file_number) //haben die richtige nachricht erreicht
			{
				getline(entry, line, ','); //Betreff

				getline(entry, line); //!!Adresse der txt Datei!!!

				entry.close(); //csv File wird geschlossen

				std::ifstream textfile(line); //txt. File wird geöffnet und gelesen

				while (getline(textfile, text_part))
				{
					my_message.push_back(text_part);
				}
				textfile.close();

				return my_message;
			}
			else //wenn counter nach oben gezäht wurde, aber die file_number noch nicht erreicht hat
			{
				getline(entry, line);
			}
		}
		else //wenn der Empfänger User nicht der Richtige ist
		{
			getline(entry, line);
		}
	}
	entry.close(); //wenn die Nachricht nicht gefunden wurde wird das csv file ebenfalls geschlossen

	//Falls der User eine Filenummer angegeben hat die größer ist als die Anzahl seiner Files wird diese Fehlermeldung in "my_message" gespeichert
	if (counter < file_number)
	{
		my_message.push_back("Fehler. Sie haben nicht so viele Nachrichten");
	}
	return my_message;
}
*/
/* Edited by Natzki */
//--------------------- Nachrichten eines Users zählen und deren Anzahl zurückgeben.---------------------------
/*
int count_messages(std::string csvfile, std::string recive_user)
{
	std::string line;
	std::string textfile;
	std::ifstream oldfile(csvfile);

	int message_count = 0;

	while (getline(oldfile, line, ','))
	{
		getline(oldfile, line, ',');

		if (line == recive_user)
		{
			message_count++;
		}
		getline(oldfile, line, ',');
		getline(oldfile, line);
	}
	if (message_count == 0)
		return -1; // Error Case: Not a single message was found.
	else
		return message_count;
}

std::vector<std::string> list_subjects_and_msgCount(std::string csvfile, std::string recive_user)
{
	std::vector<std::string> count_and_topics;
	std::string line;
	std::ifstream file(csvfile);

	int message_count = 0;

	while (getline(file, line, ','))
	{
		getline(file, line, ',');

		if (line == recive_user)
		{
			message_count++;
			getline(file, line, ',');
			count_and_topics.push_back(line);
		}
		//        else
		//        {
		//            getline(oldfile, line, ','); // Is this needed?
		//        }
		getline(file, line);
	}
	if (message_count == 0)
	{
		message_count = -1; // Error Case: Not a single message was found.
	}
	count_and_topics.push_back(std::to_string(message_count));
	return count_and_topics;
}
/* Edited by Natzki */

//---------------------Nachricht in csv und mit der dazugehörigen txt. Datei löschen.---------------------------

/*bool delete_message(std::string csvfile, std::string recive_user, int file_number)
{
	//Zuerst wird herausgefunden um welche Line und text file es sich handelt, die gelöscht werden soll
	std::string line;
	std::string textfile;
	std::ifstream oldfile(csvfile);

	int message_number = 0; //wann bin ich an der file_number
	int line_number = 0;	//welche tatsächliche line_number ist die message_number im csv file
	int count = 0;

	while (getline(oldfile, line, ','))
	{
		line_number++;
		getline(oldfile, line, ',');

		if (line == recive_user)
		{
			message_number++;

			if (message_number == file_number)
			{
				getline(oldfile, line, ',');
				getline(oldfile, line);
				textfile = line;
				oldfile.close();
				break;
			}
		}
		getline(oldfile, line);
	}
	if (message_number < file_number)
	{
		oldfile.close();
		return false;
	}
	//Nun wird ein neues csv File erstellt und jede Line bis auf die zu löschende Line vom alten csv file in das neue csv file übertragen
	std::ofstream newfile;
	oldfile.open(csvfile); //dasfile wird neu geöffnet, damit es wieder von anfang an ausgelesen wird
	newfile.open("tmp.csv", std::ios::out | std::ios::app);

	std::cout << "Zu löschende Line: " << line_number << std::endl;
	std::cout << "Zu löschendes Textfile: " << textfile << std::endl;

	while (getline(oldfile, line))
	{

		if (count != line_number)
		{
			newfile << line << std::endl;
			/* Edited by Natzki */
// Commented this out for now, since it is not needed to show in console anymore.
// std::cout << count << " " << line << std::endl;
/* Edited by Natzki */
/*	}
		count++; //der Count ist nach dem Durchlauf, da im csv File die erste Line immer leer ist.
	}
	oldfile.close();
	newfile.close();
	remove(csvfile.c_str()); //c.string() wird benötigt um den string "csvfile" in einen c.string umzuwandeln mit dem der remove Befehl arbeiten kann
	remove(textfile.c_str());
	rename("tmp.csv", csvfile.c_str()); //auch der rename Befehl benötigt einen c.string (also einen const char zum arbeiten)

	return true;
}*/

//#########################################################################################################################################################
//#########################################################################################################################################################

//Überarbeitetes Filesystem

//Create new directery for messages if it does not exist

void create_new_entry(std::string send_user, std::string recive_user, std::string subject, std::string message)
{

	//if file for recive_user does not exist, create one
	std::string recive_user_folder = "MessageFolder/" + recive_user;

	//string changed to char for the "mkdir" order
	char *folderpath = new char[recive_user_folder.size() + 1]; // +1 wegen \0
	strcpy(folderpath, recive_user_folder.c_str());
	//Create dir for recive_user if it does not exist yet
	mkdir(folderpath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	//get list of fils in the folder
	std::vector<std::string> files_list = read_directory(recive_user_folder);
	//find the id for the file name
	int id = 0;

	if (files_list[0] != "-1")
	{
		for (std::string filename : files_list)
		{
			int cutingpoint = filename.find(".");
			std::string number = filename.substr(0, cutingpoint);

			std::stringstream transform(number);
			int x = 0;
			transform >> x;
			if (x > id)
			{
				id = x;
			}
		}
	}
	id++;
	std::string s = std::to_string(id);
	//create the filename
	std::string new_name = s + ".txt";

	//create the complete directory
	std::string new_file = recive_user_folder + "/" + new_name;

	//Dann wird das File selbst erstellt und die message wird darin eingespeichert und Datei wird wieder geschlossen
	std::ofstream textdata(new_file);
	textdata << subject << std::endl;
	textdata << message << std::endl;
	textdata.close();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
//reads filenames of a directory

std::vector<std::string> read_directory(std::string directory)
{
	std::vector<std::string> filelist;
	DIR *dp;
	struct dirent *dirp;

	if ((dp = opendir(directory.c_str())) == NULL)
	{
		filelist.push_back("-1");
		return filelist;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		filelist.push_back(std::string(dirp->d_name));
	}
	closedir(dp);
	return filelist;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//new list_subjects_and_msgCount

std::vector<std::string> list_subjects_and_msgCount(std::string recive_user)
{
	std::vector<std::string> mysubjects;

	std::string directory = "MessageFolder/" + recive_user;
	std::vector<std::string> text_files = read_directory(directory);

	//-------------------
	std::string line;
	int filenumbers = text_files.size();
	filenumbers -= 2; //die files . und .. werden abgezogen
	if (filenumbers > 0)
	{
		std::string number = std::to_string(filenumbers);
		mysubjects.push_back("Number of files: " + number);

		for (std::string file : text_files)
		{

			if (file == "." || file == "..") //. und .. werden ignoriert
			{
				continue;
			}
			else
			{
				std::ifstream my_file(directory + "/" + file);
				getline(my_file, line);
				mysubjects.push_back(line);
				my_file.close();
			}
		}
	}
	else
	{

		mysubjects.push_back("-1");
	}

	return mysubjects;
}

//------------------------------------------------------------------------------------------------------------------

//new show_message function

std::vector<std::string> show_message(std::string recive_user, int file_number)
{
	std::vector<std::string> my_message;
	if (file_number == 0)
	{
		my_message.push_back("Nachricht Nummer 0 existiert nicht");
		return my_message;
	}

	file_number += 2;
	std::string directory = "MessageFolder/" + recive_user;

	std::vector<std::string> file_names = read_directory(directory);

	if (file_number > file_names.size())
	{
		my_message.push_back("Fehler. Sie haben nicht so viele Nachrichten");
	}
	else
	{
		std::ifstream textfile(directory + "/" + file_names[file_number - 1]); //txt. File wird geöffnet und gelesen
		std::string text_part;
		while (getline(textfile, text_part))
		{
			my_message.push_back(text_part);
		}
		textfile.close();
	}
	return my_message;
}

//-------------------------------------------------------------------------------------------------------------

//new delete_message function

bool delete_message(std::string recive_user, int file_number)
{
	std::string directory = "MessageFolder/" + recive_user;

	if (file_number == 0)
	{
		return false;
	}
	file_number += 2;

	std::vector<std::string> file_names = read_directory(directory);

	if (file_number > file_names.size())
	{
		return false;
	}

	std::string path = directory + "/" + file_names[file_number - 1];
	remove(path.c_str());

	return true;
}
