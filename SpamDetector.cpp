#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "HashMap.hpp"

#define ARG_NUM_ERROR_MSG "Usage: SpamDetector <database path> <message path> <threshold>"
#define GENERAL_ERROR "Invalid input"

const int ARG_NUMBER = 4;
const int INPUT_DB_IDX = 1;
const int INPUT_MESSAGE_IDX = 2;
const int INPUT_THRESHOLD_IDX = 3;
const int WORD_LINE_IDX = 0;
const int SCORE_LINE_IDX = 1;
const int LINE_LENGTH = 2;
const int START_SCORE = 0;
const char SPAM_MESSAGE[] = "SPAM";
const char NOT_SPAM_MESSAGE[] = "NOT_SPAM";

/**
 * @brief converts a string to lower case in place
 * @param convertedString the string to convert
 */
void lowerString(std::string &convertedString)
{
    std::transform(convertedString.begin(), convertedString.end(), convertedString.begin(),
                   [](unsigned char c)
                   { return std::tolower(c); });
}

/**
 * @brief checks if a string is a non negative integer
 * @param checkedString the string to check
 * @return true if is non negative integer, false otherwise
 */

bool isNonNegNumber(const std::string &checkedString)
{
    //todo check if it catches \n
    size_t stringIdx = 0;
    if (checkedString[0] == '-') //is negative number (or other invalid string)
    {
        return false;
    }
    for (; stringIdx < checkedString.length(); stringIdx++)
    {
        if (checkedString[stringIdx] < '0' || checkedString[stringIdx] > '9')
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief splits a string into values using a delimeter
 * @param strToSplit the string to split
 * @param delimeter delimeter to seperate between values
 * @return the string split
 */

std::vector<std::string> splitString(const std::string &strToSplit, const char delimeter)
{
    std::stringstream stringStream(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(stringStream, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}

bool validCommaAmount(std:: string &checkedString)
{
    int commaCount = 0;
    for (char strIdx : checkedString)
    {
        if (strIdx == ',')
        {
            commaCount++;
        }
    }
    return commaCount == LINE_LENGTH - 1;
}

/**
 * @brief creates vectors from a file of key value pairs
 * @param words the vector to store the words in
 * @param scores the vector to store scores in
 * @param filePath the file to read from
 */
void readFileIntoVectors(std::vector <std::string> &words, std::vector <int> &scores,
                         std::string filePath)
{
    std::ifstream fileReader(filePath);
    std::string line;
    while (std::getline(fileReader, line))
    {
        std::vector<std::string> lineSplit = splitString(line, ',');
        if (!(validCommaAmount(line) && lineSplit.size() == LINE_LENGTH && isNonNegNumber
            (lineSplit[SCORE_LINE_IDX])))
        {
            throw std::invalid_argument(GENERAL_ERROR);
        }
        std::string currWords = lineSplit[WORD_LINE_IDX];
        lowerString(currWords);
        int score = std::stoi(lineSplit[SCORE_LINE_IDX]);
        if (score < 0)
        {
            throw std::invalid_argument(GENERAL_ERROR);
        }
        words.push_back(currWords);
        scores.push_back(score);
    }
    fileReader.close();
}

/**
 * @brief method to use after an exception arises. prints error message to the user
 */
int exitError(const std::string& errorMessage)
{
    std::cerr << errorMessage << std::endl;
    return EXIT_FAILURE;
}

/**
 * @brief checks if a message is spam or not
 * @param scoreMap the map from words (or sentence to score)
 * @param message the message to be checked
 * @param the threshold for if a message is a spam or not
 * @return true if spam, false otherwise
 */
bool isSpam(HashMap <std::string, int> &scoreMap, std::string message, int threshold)
{
    int message_score = START_SCORE;
    std::ifstream fileReader(message);
    std::string line;
    while (std::getline(fileReader, line))
    {
        lowerString(line);
        for (const auto &pair : scoreMap)
        {
            int timesFound = 0;
            size_t start = line.find(pair.first);
            size_t end = std::string::npos;
            while (start != end)
            {
                timesFound++;
                start = line.find(pair.first, start + (pair.first.size()));
            }
            message_score += timesFound * pair.second;
        }
    }
    return message_score >= threshold;
}

/**
 * @brief checks if a file on a certain path exists
 * @param filePath the path of the file
 * @return true if exists, false otherwise
 */
bool checkFileExists(std::string filePath)
{
    return (bool) std::ifstream (filePath);
}

/**
 * @brief this program receives a message with words and score and checks if the message is spam
 * @param argc the number of arguments for the software
 * @param argv the arguments for the software
 * @return 0 upon success completion, exit failure constant otherwise
 */
int main(int argc, char *argv[])
{
    try
    {
        //intial arguments check
        if (argc != ARG_NUMBER)
        {
            return exitError(ARG_NUM_ERROR_MSG);
        }
        if (!((checkFileExists(argv[INPUT_DB_IDX])) && (checkFileExists(argv[INPUT_MESSAGE_IDX])) &&
            (isNonNegNumber(argv[INPUT_THRESHOLD_IDX])) && std::stoi(argv[INPUT_THRESHOLD_IDX]) >
            0))
        {
            return exitError(GENERAL_ERROR);
        }
        //fild reading
        std::vector <std::string> words;
        std::vector <int> scores;
        readFileIntoVectors(words, scores, argv[INPUT_DB_IDX]);
        HashMap <std::string, int> wordsToScoreMap(words, scores);
        //analyze message
        if (isSpam(wordsToScoreMap, argv[INPUT_MESSAGE_IDX], std::stoi(argv[INPUT_THRESHOLD_IDX])))
        {
            std::cout << SPAM_MESSAGE;
        }
        else
        {
            std::cout << NOT_SPAM_MESSAGE;
        }
        std::cout << std::endl;
    }
    catch (...)
    {
        return exitError(GENERAL_ERROR);
    }
    return 0;
}