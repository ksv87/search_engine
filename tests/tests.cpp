//
// Created by ksv on 27.03.2022.
//

#include "gtest/gtest.h"
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

TEST(TestCaseConverterJSON, LoadConfig)
{
    auto converterJSON = ConverterJSON();
    EXPECT_EQ(converterJSON.LoadConfig(), true);
}

TEST(TestCaseConverterJSON, GetTextDocuments)
{
    auto converterJSON = ConverterJSON();
    converterJSON.LoadConfig();
    auto texts = converterJSON.GetTextDocuments();
    EXPECT_EQ(texts.size(), 3);
    EXPECT_EQ(texts[0],"best first file");
    EXPECT_EQ(texts[1],"second file");
    EXPECT_EQ(texts[2],"third file");
}

TEST(TestCaseConverterJSON, LoadRequests)
{
    auto converterJSON = ConverterJSON();
    EXPECT_EQ(converterJSON.LoadRequests(), true);
    EXPECT_LE(converterJSON.req.requests.size(), 1000);
}

TEST(TestCaseConverterJSON, GetRequests)
{
    auto converterJSON = ConverterJSON();
    converterJSON.LoadRequests();
    auto requests = converterJSON.GetRequests();
    EXPECT_EQ(requests.size(), 3);
    EXPECT_EQ(requests[0],"first file best");
    EXPECT_EQ(requests[1],"what");
    EXPECT_EQ(requests[2],"third");
}

TEST(TestCaseConverterJSON, PutAnswer){
    std::string text = "{\"answers\":{\"request001\":{\"relevance\":[{\"docid\":0,\"rank\":0.8999999761581421},{\"docid\":1,\"rank\":0.800000011920929},{\"docid\":2,\"rank\":0.699999988079071}],\"result\":true},\"request002\":{\"relevance\":[{\"docid\":0,\"rank\":0.8999999761581421},{\"docid\":1,\"rank\":0.800000011920929},{\"docid\":2,\"rank\":0.699999988079071}],\"result\":true},\"request003\":{\"relevance\":[{\"docid\":0,\"rank\":0.8999999761581421},{\"docid\":1,\"rank\":0.800000011920929},{\"docid\":2,\"rank\":0.699999988079071}],\"result\":true}}}";
    auto converterJSON = ConverterJSON();
    converterJSON.LoadRequests();
    std::vector<std::vector<RelativeIndex>> answers;
    for (const auto& r: converterJSON.req.requests) {
        std::vector<RelativeIndex> answer;
        auto pair1 = RelativeIndex(0, 0.9);
        answer.push_back(pair1);
        auto pair2 = RelativeIndex(1, 0.8);
        answer.push_back(pair2);
        auto pair3 = RelativeIndex(2, 0.7);
        answer.push_back(pair3);
        answers.push_back(answer);
    }
    converterJSON.PutAnswers(answers);
    auto file = std::ifstream("../resources/answers.json");
    std::string text_file;
    file >> text_file;
    EXPECT_EQ(text, text_file);
}

using namespace std;

void TestInvertedIndexFunctionality(
        const vector<string>& docs,
        const vector<string>& requests,
        const std::vector<vector<Entry>>& expected
) {
    std::vector<std::vector<Entry>> result;
    InvertedIndex idx;

    idx.UpdateDocumentBase(docs);

    for(auto& request : requests) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
    }

    ASSERT_EQ(result, expected);
}

TEST(TestCaseInvertedIndex, TestBasic) {
    const vector<string> docs = {
            "london is the capital of great britain",
            "big ben is the nickname for the Great bell of the striking clock"
    };
    const vector<string> requests = {"london", "the"};
    const vector<vector<Entry>> expected = {
            {
                    {0, 1}
            }, {
                    {0, 1}, {1, 3}
            }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestBasic2) {
    const vector<string> docs = {
            "milk milk milk milk water water water",
            "milk water water",
            "milk milk milk milk milk water water water water water",
            "americano cappuccino"
    };

    const vector<string> requests = {"milk", "water", "cappuccino"};
    const vector<vector<Entry>> expected = {
            {
                    {0, 4}, {1, 1}, {2, 5}
            }, {
                    {0, 3}, {1, 2}, {2, 5}
            }, {
                    {3, 1}
            }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseInvertedIndex, TestInvertedIndexMissingWord) {
    const vector<string> docs = {
            "a  b  c  d  e  f  g  h  i  j  k  l",
            "statement"
    };
    const vector<string> requests = {"m", "statement"};
    const vector<vector<Entry>> expected = {
            {
		{}
            }, {
                    {1, 1}
            }
    };
    TestInvertedIndexFunctionality(docs, requests, expected);
}

TEST(TestCaseSearchServer, TestSimple) {
const vector<string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "Americano Cappuccino"
};

const vector<string> request = {"milk water", "sugar"};
const std::vector<vector<RelativeIndex>> expected = {
        {
                {2, 1},
                {0, 0.7},
                {1, 0.3}
        },
        {
                {}
        }
};

InvertedIndex idx;
idx.UpdateDocumentBase(docs);

SearchServer srv(idx);

std::vector<vector<RelativeIndex>> result = srv.search(request);

ASSERT_EQ(result, expected);
}

TEST(TestCaseSearchServer, TestTop5) {
const vector<string> docs = {
        "london is the capital of great britain",
        "paris is the capital of france",
        "berlin is the capital of germany",
        "rome is the capital of italy",
        "madrid is the capital of spain",
        "lisboa is the capital of portugal",
        "bern is the capital of switzerland",
        "moscow is the capital of russia",
        "kiev is the capital of ukraine",
        "minsk is the capital of belarus",
        "astana is the capital of kazakhstan",
        "beijing is the capital of china",
        "tokyo is the capital of japan",
        "bangkok is the capital of thailand",
        "welcome to moscow the capital of russia the third rome",
        "amsterdam is the capital of netherlands",
        "helsinki is the capital of finland",
        "oslo is the capital of norway",
        "stockholm is the capital of sweden",
        "riga is the capital of latvia",
        "tallinn is the capital of estonia",
        "warsaw is the capital of poland",
};

const vector<string> request = {"moscow is the capital of russia"};

const std::vector<vector<RelativeIndex>> expected = {
        {
				{7, 1},
				{14, 1},
				{0, 0.666666687},
				{1, 0.666666687},
				{2, 0.666666687}
		}
};

InvertedIndex idx;
idx.UpdateDocumentBase(docs);

SearchServer srv(idx);
std::vector<vector<RelativeIndex>> result = srv.search(request);

ASSERT_EQ(result, expected);
}

