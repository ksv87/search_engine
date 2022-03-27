//
// Created by ksv on 05.04.2022.
//

#include <algorithm>
#include <unordered_set>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "SearchServer.h"
#include "ConverterJSON.h"

std::vector<std::vector<RelativeIndex>> SearchServer::search(const std::vector<std::string> &queries_input) {

    auto answers = std::vector<std::vector<RelativeIndex>>();

    for (const auto& request: queries_input) {

        auto words = std::unordered_set<std::string>();

        bool isWord = false;
        int i = 0;
        while (request[i] == ' ' && request[i] != '\0'){
            i++;
        }
        int indexWordStart = 0;
        int indexWordEnd = 0;
        while (request[i] != '\0') {
            if (request[i] != ' ' && !isWord)
            {
                isWord = true;
                indexWordStart = i;
            }
            else if (request[i] == ' ' || request[i+1] == '\0'){
                isWord = false;
                if(request[i] == ' ')
                    indexWordEnd = i;
                else
                    indexWordEnd = i+1;
                std::string currentWord = request.substr(indexWordStart, indexWordEnd-indexWordStart);
                std::string result(currentWord.size(), ' ');
                std::transform(currentWord.begin(), currentWord.end(), result.begin(), tolower);
                currentWord = result;
                words.insert(currentWord);

            }
            i++;
        }

        std::vector<std::pair<std::string, int>> wordsCount;

        for(const auto& word: words){
            auto freq_dictionary = this->_index.GetWordCount(word);
            int sum = 0;
            for (const auto& entry: freq_dictionary) {
                sum += entry.count;
            }
            auto wordCount = std::make_pair(word,sum);
            wordsCount.push_back(wordCount);
        }

        std::sort(wordsCount.begin(), wordsCount.end(),
                  [] (std::pair<std::string, int> const& a, std::pair<std::string, int> const& b) { return a.second < b.second; });

        auto answer = std::vector<RelativeIndex>();

        if(wordsCount[0].second != 0) {

            auto docs = std::map<size_t, std::vector<std::pair<std::string, size_t>>>();

            for (const auto &[key, value]: wordsCount) {
                auto freq_dictionary = this->_index.GetWordCount(key);
                for(const auto& entry: freq_dictionary){

                    if (docs.count(entry.doc_id) == 0) {
                        docs[entry.doc_id] = std::vector<std::pair<std::string, size_t>>();
                    }
                    docs[entry.doc_id].push_back(std::make_pair(key, entry.count));

                }
            }

            float max = 0.0;
            auto rel_map = std::map<size_t, size_t>();
            for (const auto& [doc_key, doc_value]: docs) {
                rel_map[doc_key] = 0;
                for(const auto& [key, value]: doc_value){
                    rel_map[doc_key]+= value;
                }
                if(rel_map[doc_key] > max) max = rel_map[doc_key];
            }

            for(const auto& [key, value]: rel_map){
                auto rel = RelativeIndex(key, (float)value / max);
                answer.push_back(rel);

                std::sort(answer.begin(), answer.end(),
                          [] (RelativeIndex const& a, RelativeIndex const& b) { return a.rank > b.rank; });

                auto conv = ConverterJSON();
                conv.LoadConfig();
                auto max_responses = conv.cf.config.max_responses;

                while(answer.size() > max_responses){
                    answer.pop_back();
                }

            }

        }

        answers.push_back(answer);

    }

    return answers;
}
