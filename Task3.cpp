#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

#define PROCESS_NUM 13
#define MIN_LENGTH 3
#define ALLOCATED_SIZE 2000000

pthread_mutex_t mutex;
pthread_cond_t cond;

//Global Vector for word list
std::vector<std::string> task3GlobalList;
std::vector<std::string> resultList(ALLOCATED_SIZE);

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

//Helper Function that sorts strings third character onwards by index
bool sortStringByIndex(int first, int second) {
    std::string firstWord = task3GlobalList[first];
    std::string secondWord = task3GlobalList[second];

    if (firstWord.substr(2,15) == secondWord.substr(2,15)) {
        return firstWord < secondWord;
    }
    else {
        return firstWord.substr(2) < secondWord.substr(2);
    }
}

//Struct to store arguments for threading
struct arg_struct {
    std::vector<int> arg1;
    int arg2;
};

//Helper Function that sorts strings third character onwards and maps to FIFO file
void* threadMap(void *arguments) {
    //Set up arguments
    arg_struct* args = (arg_struct*)arguments;

    //Cast void pointer to vector
    std::vector <int> &index = args->arg1;

    //Get word length
    int task3MapLength = args->arg2 + MIN_LENGTH;

    //Sort index array
    sort(index.begin(), index.end(), sortStringByIndex);

    //Create wordLen file
    std::string wordLen = std::to_string(task3MapLength) + "FIFOFile";
    std::fstream file;
    file.open(wordLen + ".txt", std::fstream::out);

    //Open FIFO file
    int fd = open(wordLen.c_str(), O_WRONLY);
    
    //Error opening file
    if(fd == -1) {
        perror("Could not open FIFO file\n");
        return nullptr;
    }
    pthread_mutex_lock(&mutex);
    //Write to file
    for (const auto &i : index) {
        file << i << std::endl;
    }

    std::cout << "Wrote to FIFO file, word length: " << task3MapLength << std::endl;
    pthread_mutex_unlock(&mutex);
    pthread_cond_signal(&cond);

    //Close FIFO file
    close(fd);

    //Delete dynamically allocated arguments
    delete args;

    return nullptr;
}

//Helper Function that reads from FIFO file
void* threadReduce(void *arguments) {
    //Set up arguments
    arg_struct* args = (arg_struct*)arguments;

    //String for getting line from file
    std::string line;

    //Get word length
    int task3ReduceLength = args->arg2 + MIN_LENGTH;

    //Open FIFO file
    std::string wordLen = std::to_string(task3ReduceLength) + "FIFOFile";

    std::fstream file;
    file.open(wordLen + ".txt");

    int fd = open(wordLen.c_str(), O_RDONLY);
    sleep(5);
    
    //Error opening file
    if(fd == -1) {
        perror("Could not open FIFO file\n");
        return nullptr;
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);

    //Read file
    int i = 0;
    while(getline(file, line)) {
        i = stoi(line);
        resultList[i] = (task3GlobalList[i]);
    }

    std::cout << "Read from FIFO file, word length: " << task3ReduceLength<< std::endl;
    pthread_mutex_unlock(&mutex);

    //Delete dynamically allocated arguments
    delete args;

    return nullptr;
}

void* map3(void* arg) {
    std::cout << "\nTASK 3\n-------" << std::endl;

    //Initialise index vectors
    std::vector<std::vector<int>> indexes(13);

    //Process strings into index lists
    for(unsigned int i = 0; i < unsigned(task3GlobalList.size()); ++i) {
        indexes[task3GlobalList[i].length() - MIN_LENGTH].push_back(i);
    }

    std::cout << "Created index vectors" << std::endl;

    //Initialise Thread
    pthread_t thread[PROCESS_NUM];

    int numThreads = 0;
    int code;

    //Create threads
    for(int i = 0; i < PROCESS_NUM; i++) {
        //Get arguments
        arg_struct* args = new arg_struct;
        args->arg1 = indexes[i];
        args->arg2 = i;

        //Create Thread
        code = pthread_create(&thread[i], NULL, &threadMap,  (void *)  args);
        if (code) {
            std::cout << "ERROR: pthread_create() returned code: " << code << std::endl;
            break;
        }
        ++numThreads;
    }

    //Wait for threads
    for(int i = 0; i < numThreads; i++) {
        pthread_join(thread[i], NULL);
    }

    return nullptr;
}

void* reduce3(void* arg) {
    //Setup String for getting lines
    std::string line;

    //Setup clean file to write to
    std::ofstream filteredFile;
    filteredFile.open("Task3CleanFile.txt");

    //Initialize threads
    pthread_t thread[PROCESS_NUM];

    int numThreads = 0;
    int code;

    //Create threads
    for(int i = 0; i < PROCESS_NUM; i++) {
        //Get arguments
        arg_struct* args = new arg_struct;
        args->arg2 = i;

        //Create thread
        code = pthread_create(&thread[i], NULL, &threadReduce,  (void *)  args);
        if (code) {
            std::cout << "ERROR: pthread_create() returned code: " << code << std::endl;
            break;
        }
        ++numThreads;
    }

    //Wait for threads
    for(int i = 0; i < numThreads; i++) {
        pthread_join(thread[i], NULL);
    }

    for(const auto &word : resultList) {
        if(word == "") {continue;}
        filteredFile << word << std::endl;
    }

    std::cout << "Finished 13-1 Merge-Sort" << std::endl;
    
    //Close clean file
    filteredFile.close();

    return nullptr;
};

int main(int argc, char** argv) {
    std::vector<std::string> globalList;
    globalList = Task1Filter(argv[1], argv[2]); // Filter the word list

    //Initialize our global list
    for(auto &currentWord : globalList) {
        task3GlobalList.push_back(currentWord);
    }

    //Create FIFO files
    for(int i = 0; i < PROCESS_NUM; ++i) {
        std::string wordLen = std::to_string(i + MIN_LENGTH) + "FIFOFile";
        if (mkfifo(wordLen.c_str(), 0777)) {
            if (errno != EEXIST) {
                perror("Could not create FIFO file\n");
            }
        }
    }

    pthread_t thMap;
    pthread_t thReduce;

    pthread_mutex_init(&mutex, NULL);

    pthread_create(&thMap, NULL, &map3, NULL); //Map Thread
    pthread_create(&thReduce, NULL, &reduce3, NULL); //Reduce Thread

    pthread_join(thMap, NULL);
    pthread_join(thReduce, NULL);

    pthread_mutex_destroy(&mutex);

    return 0;
}