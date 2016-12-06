#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int SEQUENCE_LENGTH = 1000;
const int PAGE_RANGE = 255;
const int PAGING_LIMIT = 100;

void printAnomaly(int sequence, int fault1, int frameSize, int fault2) {
  std::cout << "Anomaly Discovered!" << std::endl;
  std::cout << "    Sequence: " << sequence << std::endl;
  std::cout << "    Page Faults: " << fault1 << " @ Frame Size: " << frameSize << std::endl;
  std::cout << "    Page Faults: " << fault2 << " @ Frame Size: " << frameSize + 1 << std::endl;
  return;
}

std::vector<int> generateRandomPagingSequence() {
  std::vector<int> pagingSequence;
  for (int i = 0; i < SEQUENCE_LENGTH; i++) pagingSequence.push_back(rand() % PAGE_RANGE + 1);
  return pagingSequence;
}

int countAnamomalies() {
  int anomalyCount = 0;
  for (int i = 0; i < PAGING_LIMIT; i++) {
    int currentFault = 0;
    int tempFault = 0;
    std::vector<int> randomSequence = generateRandomPagingSequence();
    for (int j = 1; j < PAGING_LIMIT; j++) {
      std::unordered_map<int, int> searchList;
      std::queue<int> faultQueue;
      int sequenceFirst = randomSequence[0];
      faultQueue.push(sequenceFirst);
      searchList.insert({sequenceFirst, sequenceFirst});
      tempFault = currentFault;
      // because a fault has already been added
      currentFault = 1;
      for (int z = 1; z < SEQUENCE_LENGTH; z++) {
        int checker = randomSequence[z];
        // std::cout << faultQueue.size() << " : " << j << std::endl;
        if (searchList.count(checker) == 1) {
          // break out of the loop
          continue;
        } else if (faultQueue.size() <= j) {
          currentFault++;
          searchList.insert({checker, checker});
          faultQueue.push(checker);
        } else {
          currentFault++;
          searchList.erase(faultQueue.front());
          faultQueue.pop();
          faultQueue.push(checker);
          searchList.insert({checker, checker});
        }
      }

      // std::cout << currentFault << " : " << tempFault << std::endl;
      if (j > 1 && currentFault > tempFault) {
        anomalyCount++;
        printAnomaly(i, tempFault, j, currentFault);
      }
    }
  }
  return anomalyCount;
}

int main() {
  srand (time(NULL));
  std::cout << "For my test of this program I have been getting aproximately 2 - 11" << std::endl;
  std::cout << " anamolies and on my mac it would take about 2 seconds to run with the unordered_map" << std::endl;
  std::cout << "Length of memory reference string: " << SEQUENCE_LENGTH << std::endl;
  std::cout << "Frames of physical memory: " << PAGING_LIMIT << std::endl;
  int anomalyCount = countAnamomalies();
  std::cout << "Anomaly detected " << anomalyCount << " times." << std::endl;

  return 0;
}
