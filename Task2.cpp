#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <algorithm>

#define PROCESS_NUM 13
#define MIN_LENGTH 3


//Helper Function that sorts strings third character onwards
bool sortString(std::string firstWord, std::string secondWord) {
    if (firstWord.substr(2,15) == secondWord.substr(2,15)) {
        return firstWord < secondWord;
    }
    else {
        return firstWord.substr(2) < secondWord.substr(2);
    }
}

std::vector<std::string> Task1Filter(std::string dirtyFile, std::string cleanFile) {
    std::cout << "\nTASK 1\n-------" << std::endl;
    //Setup source data file
    std::ifstream file(dirtyFile);
    std::string line;

    //Initialise vector
    std::vector<std::string> wordList;

    //Process strings into vector
    while (std::getline(file, line)) {
        wordList.push_back(line);
    }
    file.close();

    std::cout << "Placed strings from text file into a vector" << std::endl;

    //Remove duplicates
    wordList.erase(unique(wordList.begin(), wordList.end()), wordList.end());

    std::cout << "Removed Duplicates from vector" << std::endl;

    //Filter word list
    wordList.erase( std::remove_if(
        wordList.begin(), wordList.end(),
        []( std::string word ) { 
            //Filter words only with lowercase alphabet letters
            return word.find_first_not_of("abcdefghijklmnopqrstuvwxyz") != std::string::npos
            //Filter word lengths only with range 3-15
            || word.length() < 3 || word.length() > 15; 
        } ),
    wordList.end() );

    std::cout << "Filter words only with lowercase alphabet letters and removed words not within range 3-15 characters from vector" << std::endl;
    
    //Sort string by third character onwards
    sort(wordList.begin(), wordList.end(), sortString);
    
    std::cout << "Sorted words from third character onwards" << std::endl;

    //Setup clean file to write to
    std::ofstream filteredFile;
    filteredFile.open(cleanFile);

    //Write to clean file contents of vector
    for (const auto &currentWord : wordList) {
        filteredFile << currentWord << "\n";
    }
    
    std::cout << "Placed strings from vector into clean text file" << std::endl;

    //Close clean file
    filteredFile.close();
    
    return wordList;
};

void map2(std::vector<std::string> globalList) {
    std::cout << "\nTASK 2\n-------" << std::endl;
    //Initialise vector lists
    std::vector<std::vector<std::string>> wordList(PROCESS_NUM);

    //Process strings into vector lists
    for (const auto &currentWord : globalList) {
        wordList[currentWord.length() - MIN_LENGTH].push_back(currentWord);
    }

    std::cout << "Placed strings from text file into a vectors" << std::endl;

    //Initialize process ID
    pid_t pid;

    //Call Fork
    for(int i = 0; i < PROCESS_NUM; ++i) {
        pid = fork();

        //Error State
        if (pid == -1) {
            perror("fork");
        }

        if (pid == 0) {
            //Sort string by third character onwards
            sort(wordList[i].begin(), wordList[i].end(), sortString);

            //Create new File
            std::ofstream newFile;
            newFile.open(std::to_string(i+MIN_LENGTH) + "WordLen.txt");

            //Write to clean file contents of vector
            for (const auto &currentWord : wordList[i]) {
                newFile << currentWord << "\n";
            }

            //Close file
            newFile.close();
    
            std::cout << "Wrote to file of different word length: " << i+MIN_LENGTH << std::endl;

            //Exit Child Process
            exit(0);
        }
    }

    for(int i = 0; i < PROCESS_NUM; ++i) { 
        wait(NULL);
    }
    
}

void reduce2() {
    //Setup Vector to sort
    std::vector<std::string> currentWords;

    //Setup wordlist Vector
    std::vector<std::vector<std::string>> wordList(PROCESS_NUM);

    //Setup String for getting lines
    std::string line;

    //Setup clean file to write to
    std::ofstream filteredFile;
    filteredFile.open("Task2CleanFile.txt");

    for(int i = 0; i < PROCESS_NUM; ++i) {
        //Open each word length file
        std::ifstream file;
        file.open(std::to_string(i+MIN_LENGTH) + "WordLen.txt");

        //Process first lines to vector for sorting
        std::getline(file, line);
        currentWords.push_back(line);
        wordList[i].push_back(line);

        //Process strings into vector
        while (std::getline(file, line)) {
            wordList[i].push_back(line);
        }

        //Close Input file
        file.close();
    }

    std::cout << "Stored different word lengths into vectors" << std::endl;

    //Length of word
    int length;

    //Integer vector to keep track of index
    std::vector<int> index(PROCESS_NUM, 0);

    while(currentWords.size() != 0) {
        //Sort current words
        sort(currentWords.rbegin(), currentWords.rend(), sortString);

        //Get lowest sorted word
        filteredFile << currentWords.back() << "\n";

        //Get length of word
        length = currentWords.back().length() - MIN_LENGTH;

        //Remove word from current vector
        currentWords.pop_back();

        //Check if there are no more words in text file
        if(unsigned(index[length] + 1) == unsigned(wordList[length].size())) {
            continue;
        }

        //Increment index
        index[length] += 1;

        //Get new word from text file
        currentWords.push_back(wordList[length][index[length]]);
    }

    std::cout << "Finished 13-1 Merge-Sort" << std::endl;
    
    //Close clean file
    filteredFile.close();
};

int main(int argc, char** argv) {
    std::vector<std::string> globalList;
    globalList = Task1Filter(argv[1], argv[2]); // Filter the word list

    map2(globalList); // Map Task
    reduce2(); // Reduce Task

    return 0;
}