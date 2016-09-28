#include <iostream>
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

int main() {
	bool exitProgram = false;

	while(!exitProgram) {
		std::string command;
		std::cout << "[cmd]: ";
		std::getline(std::cin, command);

		if (command == "exit") {
			exitProgram = true;
			return 0;
		}

		auto forkValue = fork();
		if (forkValue == 0) {
			std::vector<char*> commandList = parseCommandtoList(command);
			execvp(commandList[0], commandList.data());
			return true;
		}
		waitpid(forkValue, nullptr, 0);
	}

	return 0;
}
