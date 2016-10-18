#include <iostream>
#include <chrono>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdio>
#include <cstdlib>
#include <csignal>

const int PIPE_COUNT = 2;
const int PIPE_READ_END = 0;
const int PIPE_WRITE_END = 1;
const int STDIN = 0;
const int STDOUT = 1;
sig_atomic_t alertFlat = 0;


void handlerSignal(int s){
  alertFlat = 1;
}

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

std::string getNextPath(std::string toPath, std::string currentPath) {
  std::string nextPath = "/";
  if(toPath == "..") {
    int lastInstanceIndex = currentPath.find_last_of("/\\");
    nextPath = currentPath.substr(0, lastInstanceIndex);
  } else if(toPath[0] != '/'){
    toPath.insert(0, 1, '/');
    nextPath = currentPath.append(toPath);
  } else {
    nextPath = toPath;
  }
  std::cout << "nextPath " << nextPath << std::endl;
  return nextPath;
}

void executeCD(std::vector<char *> commandList) {
  char buffer[100];
  char *path = getcwd(buffer, 100);
  if (!path) {
      std::cout << "CD Error" << std::endl;
  } else {
      std::string currentPath = path;
      std::string toPath = commandList[1];
      std::string path =  getNextPath(toPath, currentPath);

      int dirStatus = chdir(path.c_str());
      if (dirStatus < 0) {
        std::cout << "No Such File or directory." << std::endl;
      }
  }

  return;
}


int getPipeIndex(std::vector<char*> commandList) {
  for (int i = 0; i < commandList.size(); i++) {
    std::string tempString(commandList[i]);
    if (tempString == "|") {
			return i;
		}
  }
  return -1;
}

int getIsCarrot(std::vector<char*> commandList) {
  for (int i = 0; i < commandList.size(); i++) {
    std::string tempString(commandList[i]);
    if (tempString == "^") {
			return true;
		}
  }
  return false;
}

void executePipe(std::vector<char*> commandList, int pipeIndex) {
    int pids[PIPE_COUNT];
    pipe(pids);
    int savedStdout = dup(STDOUT);
    int savedStdin = dup(STDIN);

    pid_t pid = fork();
    if (pid == 0) {
			std::vector<char*> beforePipe(commandList.begin(), commandList.begin() + pipeIndex);
      dup2(pids[PIPE_WRITE_END], STDOUT);

      execvp(beforePipe[0], beforePipe.data());
    }
    // // Second child will 'more' whatever input comes down over the pipe
    pid_t pid2 = fork();
    if (pid2 == 0) {
      dup2(pids[PIPE_READ_END], STDIN);
      // // This is key, in order to terminate the input from the pipe
      // have to close off the write end, otherwise the 'more' command
      // will continue to wait for additional data.
      close(pids[PIPE_WRITE_END]);

      std::vector<char*> afterPipe(commandList.begin() + pipeIndex + 1, commandList.end());


      execvp(afterPipe[0], afterPipe.data());
    }
    // // Wait for the first child to finish
    int status;
    waitpid(pid, &status, 0);

    // // Fully close down the pipe, and yes, for whatever reason, it requires
    // the parent process to close both ends, even though the second child
    // already closed the write end...not sure I fully understand this.
    close(pids[PIPE_WRITE_END]);
    close(pids[PIPE_READ_END]);
    waitpid(pid2, &status, 0);
    // // Restore standard out and in, so our program will be back to normal when complete
    dup2(savedStdout, STDOUT);
    dup2(savedStdin, STDIN);
    return;
}


int main() {
  std::signal(SIGINT, handlerSignal);

	typedef std::chrono::high_resolution_clock Time;
	auto t0 = Time::now();
	std::vector<std::string> commandHistory;
	bool exitProgram = false;

	while(!exitProgram) {
		bool isPTime = false;
		bool isValid = false;

    char buffer[100];
    char *path = getcwd(buffer, 100);
    std::string stringPath = path;

		std::string command;
		std::cout << "[" << stringPath << ":] ";
		std::getline(std::cin, command);
		commandHistory.push_back(command);

		if (alertFlat != 0) {
			std::cout << "Sorry, No Control C for you. use exit" << std::endl;
			alertFlat = 0;
		}

		if (command == "exit") {
			isValid = true;
			exitProgram = true;
			break;
			return 0;
		} else if (command == "history") {
			isValid = true;
			executeHistory(commandHistory);
		} else if (command == "ptime") {
			isValid = true;
			isPTime = true;
			t0 = Time::now();
		}

    std::vector<char*> commandList = parseCommandtoList(command);
    int pipeIndex = getPipeIndex(commandList);
		bool isCarrot = getIsCarrot(commandList);

    if (pipeIndex != -1) {
      executePipe(commandList, pipeIndex);
    } else if(isCarrot == true) {
			std::string tempCommand = commandHistory[std::stoi(commandList[1])];
			std::vector<char*> commandList = parseCommandtoList(tempCommand);
			pid_t forkValue = fork();
			if (forkValue == 0) {
				execvp(commandList[0], commandList.data());
			}

			int status;
      waitpid(forkValue, &status, 0);
		} else {
      pid_t forkValue2 = fork();
      if (forkValue2 == 0) {
        std::string stringCommand(commandList[0]);
        if (stringCommand == "cd") {
          executeCD(commandList);
        } else if (execvp(commandList[0], commandList.data()) == -1 && isValid == false) {
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
      int status;
      waitpid(forkValue2, &status, 0);
    }

	}

	return 0;
}
