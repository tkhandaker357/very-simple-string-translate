#include <iostream>
#include <string>
#include <string.h>
#include <map>
#include <vector>

auto printVector{ 
    []<typename T>(std::vector<T>& vec) -> void {
        for (size_t i{ 0 }; i < vec.size(); ++i) 
            std::cout << vec[i] << (i != vec.size() - 1 ? ", " : "\n");
    } 
};

auto makeCase(std::string string, bool makeUpper = false) -> std::string {
    std::string newString{};
    for (size_t i{ 0 }; i < string.length(); ++i) {
        newString += (makeUpper ? toupper(string[i]) : tolower(string[i]));
    }       
    return newString;
}

auto capitaliseString(std::string string) -> std::string {
    std::string capitalisedString{ string };

    for (size_t i{ 0 }; i < capitalisedString.length(); ++i) {
        if (capitalisedString[i] != ' ') {
            capitalisedString[i] = toupper(capitalisedString[i]);
            break;
        }
    }
    
    return capitalisedString;
}

// finds specified string at specified index in original string or parses through whole string to find string
auto isInString(std::string string, std::string findInString, bool parseString = false, size_t atPos = 0, bool findFullWord = true,
bool caseSensitive = false) -> bool {
    bool inString{ false };
    if (!caseSensitive) string = makeCase(string, false);

    if (parseString) {
        for (size_t i{ 0 }; i < string.length(); ++i) {
            if (string[i] == findInString[0]) {
                for (size_t j{ 0 }; j < findInString.length(); ++j) {
                    inString = (string[i + j] == findInString[j] ? true : false);
                    if (!inString) break;
                }
            }
        }
    }
    else {
        // checks if first and last character of word is surrounded by whitespace or ending punctuation
        if (atPos != 0 && findFullWord && (atPos + (string.length() - 1) + 1 < string.length() - 1)) {
            if (string[atPos - 1] && string[atPos + (string.length() - 1) + 1] != ' ')
                return inString;
        }
        
        for (size_t i{ 0 }; i < findInString.length(); ++i) {
            inString = (string[atPos + i] == findInString[i] ? true : false);
            if (!inString) break;
        }
    }

    return inString;
}

using instanceMap = std::map<int, int>;
// returns instance number and position of first character of that instance
auto instancesInString(std::string string, std::string findInString, bool findFullWord = true) -> instanceMap {
    int instanceOfSubstr{ 0 };
    instanceMap instances;

    for (size_t i{ 0 }; i < string.length(); ++i) {
        if (isInString(string, findInString, false, i, findFullWord)) {
             ++instanceOfSubstr;
            instances[instanceOfSubstr] = i;
            i += findInString.length() - 1;
        }
    }

    return instances;
}

using charStringVec = std::pair<std::vector<std::string>, std::vector<char>>;
auto splitString(std::string string, std::vector<char> splitters) -> charStringVec {
    std::vector<std::string> splittedString{};
    std::vector<char> splitChars{}; // vector order same as appears in string

    auto isCharInString{
        [&](char& currentLetter) -> bool {
            bool isInString{ false };
            for (size_t i{ 0 }; i < splitters.size(); ++i) {
                if (currentLetter == splitters[i])
                    isInString = true;
            }
            return isInString;
        }
    };
    std::string sentence{};
    for (size_t i{ 0 }; i < string.length(); ++i) {
        if (isCharInString(string[i])) {
            splittedString.push_back(sentence);
            splitChars.push_back(string[i]);
            sentence = "";
        }
        else 
            sentence += string[i];
    }

    return std::pair<std::vector<std::string>, std::vector<char>>{ splittedString, splitChars };
}

auto replaceInString(std::string string, std::string replaceWhat, std::string replaceWith, bool oneWord = false, int atInstance = 0) 
-> std::string {
    std::string newString{};
    instanceMap instancesOfWord{ instancesInString(string, replaceWhat) };
    bool isWordReplaced{ false };

    // this piece of code gets repeated three times due to the conditions that are gonna be met beforehand
    auto replaceWord{
        [&](size_t& stringIndex) -> std::string {
            stringIndex += (replaceWhat.length() - 1); 
            isWordReplaced = (oneWord && stringIndex == instancesOfWord[atInstance] ? true : false);
            return replaceWith; 
        }
    };

    for (size_t i{ 0 }; i < string.length(); ++i) {
        newString += (isInString(string, replaceWhat, false, i, true) ? 
                        (oneWord == true ? 
                            (i == instancesOfWord[atInstance] ? 
                                replaceWord(i) 
                                : std::string{ string[i] }) 
                        : replaceWord(i)) 
                     : std::string{ string[i] });
    }

    return newString;
}

using wordMap = std::map<std::string, std::string>;
// works only for cString organised 2-dimensionally, keyValueSeparator.first denoting each k:v pair, and .second denoting each k and v
auto cStringToMap(const char* cString, std::pair<char, char> keyValueSeparator) -> wordMap {
    wordMap newMap{};
    std::string key{}, value{};
    std::vector<std::string> splitStringMap{ splitString(std::string{ cString }, { keyValueSeparator.first }).first };
    std::vector<std::pair<std::string, std::string>> mapContents{};

    for (size_t i{ 0 }; i < splitStringMap.size(); ++i) {
        key = splitStringMap[i].substr(0, splitStringMap[i].find(keyValueSeparator.second));
        value = splitStringMap[i].substr(splitStringMap[i].find(keyValueSeparator.second) + 1, std::string::npos);
        mapContents.push_back(std::pair<std::string, std::string>{ key, value });
        key = ""; value = "";
    }
    
    for (size_t i{ 0 }; i < mapContents.size(); ++i)
        newMap.emplace(mapContents[i]);

    return newMap;
}

auto translateStory (std::string story, wordMap wordBank, bool capitalisation = true) -> std::string {
    std::string newStory{ ' ' + (capitalisation ? (story[story.length() - 1] != ('.' || '!' || '?') ? story + '.' : story) : story) };
    for (const auto& [originalWord, newWord] : wordBank)
        newStory = replaceInString(newStory, std::string{ ' ' + originalWord + ' '}, std::string{  ' ' + newWord + ' ' });

    // check for periods in string and if capitalistion bc if capitalisation is on then we need to worry about capitalising each sentence
    if (isInString(story, ".", true, 0, false) && capitalisation) {
        charStringVec splittedStory{ splitString(newStory, { '.', '?', '!' }) };
        std::vector<std::string> capitalisedParagraph{};
        
        for (size_t i{ 0 }; i < splittedStory.first.size(); ++i) {
            std::string newString{ splittedStory.first[i] };
            newString = capitaliseString(newString) += splittedStory.second[i];
            capitalisedParagraph.push_back(newString);
        }

        newStory = "";
        for (size_t i{ 0 }; i < capitalisedParagraph.size(); ++i) {
            newStory += capitalisedParagraph[i];
        }
    }
    else if (capitalisation)
        newStory = capitaliseString(newStory);
    
    return newStory;
}

extern "C" {
    auto PyTranslateStory(const char* story, const char* wordBankFile, bool capitalisation = true) -> const char* {
        std::string translatedStory{ translateStory(std::string{ story }, cStringToMap(wordBankFile, { '\n', ':' })) };
        char* cTranslatedStory = new char[translatedStory.length() + 1];
        strcpy(cTranslatedStory, translateStory(std::string{ story }, cStringToMap(wordBankFile, { '\n', ':' })).c_str());

        return cTranslatedStory;
    }
}
