#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>


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

int main(int argc, char** argv) {
  Task1Filter(argv[1], argv[2]); // Filter the word list

  return 0;
}