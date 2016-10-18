#include <iostream>
#include <chrono>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>


std::vector<char*> parseCommandtoList(std::string command) {
	std::vector<char*> commandList;
	int indexOfLastSpace = 0;

	for (int i = 0; i < command.size() + 1; i++) {
		if (command[i] == ' ') {
			std::string tempSplitCommand = command.substr(indexOfLastSpace, i - indexOfLastSpace);
			indexOfLastSpace = i;
			char * splitCommand = new char[tempSplitCommand.size()];
			std::strcpy(splitCommand, tempSplitCommand.c_str());
			if (i + 1 < command.size()) indexOfLastSpace++;
			commandList.push_back(splitCommand);
		} else if (i == command.size() - 1) {
			std::string tempSplitCommand = command.substr(indexOfLastSpace, i - command.size());
			char * splitCommand = new char[tempSplitCommand.size()];
			std::strcpy(splitCommand, tempSplitCommand.c_str());
			commandList.push_back(splitCommand);
		}
	}
	return commandList;
}

void executeHistory(std::vector<std::string> commandHistory) {
	std::cout << "-- Command History --" << std::endl;
	for(int i = 0; i < commandHistory.size(); i++) {
		std::cout << i + 1 << ": " << commandHistory[i] << std::endl;
	}
	return;
}


int main() {
	typedef std::chrono::high_resolution_clock Time;
	auto t0 = Time::now();
	std::vector<std::string> commandHistory;
	bool exitProgram = false;

	while(!exitProgram) {
		bool isPTime = false;
		bool isValid = false;
		std::string command;
		std::cout << "[cmd]: ";
		std::getline(std::cin, command);
		commandHistory.push_back(command);

		if (command == "exit") {
			isValid = true;
			exitProgram = true;
			return 0;
		} else if (command == "history") {
			isValid = true;
			executeHistory(commandHistory);
		} else if (command == "ptime") {
			isValid = true;
			isPTime = true;
			t0 = Time::now();
		}

		auto forkValue = fork();
		if (forkValue == 0) {
			std::vector<char*> commandList = parseCommandtoList(command);
			if (execvp(commandList[0], commandList.data()) == -1 && isValid == false) {
				std::cout << "Command failure" << std::endl;
			}
			if (isPTime == true) {
				typedef std::chrono::milliseconds milli;
				typedef std::chrono::duration<float> duration;
				auto t1 = Time::now();
				duration difference = t1 - t0;
				std::cout << "Child process took " << difference.count() << " milliseconds long" << std::endl;
			}
		}

		waitpid(forkValue, nullptr, 0);
	}

	return 0;
}
