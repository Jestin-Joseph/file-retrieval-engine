// src/Search.cpp
#include "Search.hpp"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

std::vector<std::string> tokenize_query(const std::string& text) {
    std::vector<std::string> tokens;
    std::stringstream ss(text);
    std::string token;
    while (ss >> token) {
        if (token != "and") tokens.push_back(token);
    }
    return tokens;
}

void search(const Indexer& indexer, const std::string& query) {
    auto terms = tokenize_query(query);
    std::unordered_map<int, int> docFrequencyMap;

    for (const auto& term : terms) {
        for (const auto& [docID, freq] : indexer.lookUpTerm(term)) {
            docFrequencyMap[docID] += freq;
        }
    }

    std::vector<std::pair<int, int>> results(docFrequencyMap.begin(), docFrequencyMap.end());
    std::sort(results.begin(), results.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    std::cout << "\n[Top Search Results for \"" << query << "\"]\n";
    int count = 0;
    for (const auto& [docID, totalFreq] : results) {
        std::cout << indexer.getDocument(docID) << ": appears " << totalFreq << " times.\n";
        if (++count >= 10) break;
    }
    if (results.empty()) std::cout << "No matching documents found.\n";
}