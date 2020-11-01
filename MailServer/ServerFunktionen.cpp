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
#include <thread>
#include <mutex>
#include "ServerFunktionen.h"

std::mutex stop;

void newline()
{
	printf("\n");
}

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

	stop.lock();

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
	stop.unlock();
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
	stop.lock();
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
	stop.unlock();
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
	
	std::string directory = "MessageFolder/" + recive_user;

	stop.lock();

	std::vector<std::string> file_names = read_directory(directory);

	if (file_number + 2 > file_names.size())
	{
		my_message.push_back("Fehler. Sie haben nicht so viele Nachrichten");
	}
	else
	{
		if(file_names[0] == "." || file_names[0] == "..") // If Windows Folder-System, "." and ".." are first; Linux they are last!
			{file_number += 2;}															   // DO NOT FORGET to reset counter!
		std::ifstream textfile(directory + "/" + file_names[file_number - 1]); //txt. File wird geöffnet und gelesen
		std::string text_part;
		while (getline(textfile, text_part))
		{
			my_message.push_back(text_part);
		}
		textfile.close();
	}

	stop.unlock();

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

	std::lock_guard<std::mutex> lock(stop);

	std::vector<std::string> file_names = read_directory(directory);

	if (file_number > file_names.size())
	{

		return false;
	}

	file_number -= 2;
	std::string path = directory + "/" + file_names[file_number - 1];

	remove(path.c_str());

	return true;
}
