//
// Created by ksv on 31.03.2022.
//

#ifndef SEARCH_ENGINE_INVERTEDINDEX_H
#define SEARCH_ENGINE_INVERTEDINDEX_H


#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <memory>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

struct Entry {
    size_t doc_id;
    size_t count;
// Данный оператор необходим для проведения тестовых сценариев
    bool operator ==(const Entry& other) const {
        return (doc_id == other.doc_id &&
                count == other.count);
    }
};

class InvertedIndex {
    std::mutex* mutex;
public:

/**
* Обновить или заполнить базу документов, по которой будем совершать
поиск
* @param texts_input содержимое документов
*/
    void UpdateDocumentBase(std::vector<std::string> input_docs);
/**
* Метод определяет количество вхождений слова word в загруженной базе
документов
* @param word слово, частоту вхождений которого необходимо определить
* @return возвращает подготовленный список с частотой слов
*/
    std::vector<Entry> GetWordCount(const std::string& word);
private:
    std::vector<std::string> docs; // список содержимого документов
    mutable std::map<std::string, std::vector<Entry>> freq_dictionary; // частотный словарь
    void RunThread(size_t docIndex, const std::string &doc);
};

#endif //SEARCH_ENGINE_INVERTEDINDEX_H
