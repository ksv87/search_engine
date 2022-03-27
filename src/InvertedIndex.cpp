//
// Created by ksv on 31.03.2022.
//

#include "InvertedIndex.h"

#include <utility>
#include <thread>
#include "ConverterJSON.h"

void InvertedIndex::RunThread(size_t docIndex, const std::string &doc) {

    bool isWord = false;
    int i = 0;
    while (doc[i] == ' ' && doc[i] != '\0'){
        i++;
    }
    int indexWordStart = 0;
    int indexWordEnd = 0;
    while (doc[i] != '\0') {
        if (doc[i] != ' ' && !isWord)
        {
            isWord = true;
            indexWordStart = i;
        }
        else if (doc[i] == ' ' || doc[i+1] == '\0'){
            isWord = false;
            if(doc[i] == ' ')
                indexWordEnd = i;
            else
                indexWordEnd = i+1;
            std::string currentWord = doc.substr(indexWordStart, indexWordEnd-indexWordStart);
            std::string result(currentWord.size(), ' ');
            std::transform(currentWord.begin(), currentWord.end(), result.begin(), tolower);
            currentWord = result;
            bool entryFound = false;
            mutex->lock();
            if(freq_dictionary.find(currentWord) != freq_dictionary.end()){
                auto &currentEntry = freq_dictionary[currentWord];
                for (auto &current: currentEntry) {
                    if(current.doc_id == docIndex){
                        current.count++;
                        entryFound = true;
                    }
                }
                if(!entryFound){
                    auto entry = Entry();
                    entry.doc_id = docIndex;
                    entry.count = 1;
                    currentEntry.push_back(entry);
                }
            }else{
                auto currentEntry = std::vector<Entry>();
                auto entry = Entry();
                entry.doc_id = docIndex;
                entry.count = 1;
                currentEntry.push_back(entry);
                freq_dictionary[currentWord] = currentEntry;
            }
            mutex->unlock();
        }
        i++;
    }
}

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = std::move(input_docs);
    size_t docIndex = 0;
    std::vector<std::thread*> threads;
    this->mutex = new std::mutex;
    auto thread_pool = boost::asio::thread_pool();
    for (const auto& doc: docs) {
        boost::asio::post(thread_pool, boost::bind(&InvertedIndex::RunThread, this, docIndex, doc));
        docIndex++;
    }
    thread_pool.join();
    delete this->mutex;
    for(auto& [key, value]: freq_dictionary){
            std::sort(value.begin(), value.end(),
                      [] (Entry const& a, Entry const& b) { return a.doc_id < b.doc_id; });
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::string result(word.size(), ' ');
    std::transform(word.begin(), word.end(), result.begin(), ::tolower);
    if(freq_dictionary.find(word) != freq_dictionary.end()){
        auto entry = freq_dictionary[word];
        return entry;
    }else{
        auto entry = Entry();
        entry.doc_id = 0;
        entry.count = 0;
        auto resp = std::vector<Entry>();
        resp.push_back(entry);
        return resp;
    }
}
