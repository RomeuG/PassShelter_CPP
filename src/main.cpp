#include <iostream>
#include <sstream>
#include <iomanip>

#include <vector>

#include <sqlite3.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "main.h"
#include "sqlite.h"

#include "aes.h"
#include "enc.h"

// verify if file exists
bool file_exists(const std::string& file_name)
{
	struct stat buffer;
	return(stat(file_name.c_str(), &buffer) == 0); 
}

// some info about the program itself and stuff used by the program
void print_stats(void)
{
	std::cout << "PassShelter - " << PSVERSION << "\n";
	std::cout << "SQLite3 - " << sqlite3_libversion() << ":" << sqlite3_libversion_number() << "\n";
}

// splits the string from the command prompt
	std::vector<std::string>
split_string(std::string user_string)
{
	std::vector<std::string> splitted_string;
	std::istringstream cmd_buffer(user_string);

	while(cmd_buffer)
	{
		std::string temp;
		cmd_buffer >> temp;
		splitted_string.push_back(temp);
	}

	return splitted_string;
}

// main function
int main(int argc, char* argv[])
{
	std::string db_file = "pshelter.db";
	sqlite3* sqlite_db = NULL;
	SQLite sql_class(sqlite_db);

	std::string user_cmd;
	std::vector<std::string> cmd_words;

	bool loop = true;

	print_stats();

	if(!file_exists(db_file))
	{
		std::cout << "File 'pshelter.db' does not exist. Creating...\n";
		std::cout << "File successfully created.\n";
	}
	else
	{
		std::cout << "File 'pshelter.db' found.\n";
	}

	while(loop)
	{
		std::cout << "Insert command: \n";
		std::getline(std::cin, user_cmd);

		cmd_words = split_string(user_cmd);

		if(cmd_words.size() != 0)
		{
			if (cmd_words[0] == "ins")
			{
				if(cmd_words.size() == 5)
				{
					sql_class.sql_insert(cmd_words[1], cmd_words[2], cmd_words[3]);	
				}
				else
				{
					std::cout << "Wrong number of arguments.\n";
				}
			}
			else if(cmd_words[0] == "rm")
			{
				if(cmd_words.size() == 5)
				{
					sql_class.sql_delete(cmd_words[1], cmd_words[2], cmd_words[3]);	
				}
				else
				{
					std::cout << "Wrong number of arguments.\n";
				}
			}
			else if(cmd_words[0] == "genkey")
			{
				uint8_t buff[32], iv[16], key[16], word[32];

				generate_iv(iv);
				generate_key(key);
				generate_string(word, 32);
				AES128_CBC_encrypt_buffer(buff, word, 32, key, iv);

				print_encrypted_key(buff, 32);
			}
			else if(cmd_words[0] == "test")
			{
				if(cmd_words.size() == 3)
				{
					//std::string word = "Isto_e_um_testee";
					//uint8_t buff[16], iv[16], key[16];
					int ws = cmd_words[1].length(); 
					uint8_t buff[ws], iv[16], key[16], out[ws];
					
					generate_iv(iv);
					generate_key(key);
					AES128_CBC_encrypt_buffer(buff, (uint8_t*)cmd_words[1].c_str(), ws, key, iv);

					print_encrypted_key(buff, ws);

					AES128_CBC_decrypt_buffer(out, buff, ws, key, iv);

					print_decrypted_key(out, ws);

				}
				else
				{
					std::cout << "Wrong number of arguments.\n";
				}
			}
			else if(cmd_words[0] == "exit" || cmd_words[0] == "quit")
			{
				// loop = false;
				exit(EXIT_SUCCESS);	
			}
			else
			{
				std::cout << "Invalid command.\n";
			}
		}
	}
	return 0;
}
