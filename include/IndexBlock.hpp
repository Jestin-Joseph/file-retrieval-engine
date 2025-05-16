// include/IndexBlock.hpp
#pragma once
#include <string>
#include <unordered_map>

struct IndexBlock {
    int docID;
    std::string filePath;
    std::unordered_map<std::string, int> termFrequencies;

    IndexBlock() = default;
    IndexBlock(int id, const std::string& path) : docID(id), filePath(path) {}
};