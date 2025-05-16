// include/Indexer.hpp
#pragma once
#include "DualQueue.hpp"
#include "DataBlock.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <sycl/sycl.hpp>

class Indexer
{
public:
    Indexer(DualQueue<DataBlock> &inputQueue, int numThreads, sycl::device selected_device);
    void start();
    void join();

    std::vector<std::pair<int, int>> lookUpTerm(const std::string &term) const;
    std::string getDocument(int docID) const;

private:
    void process_blocks();

    DualQueue<DataBlock> &inputQueue;
    int nextDocID;
    int numThreads;
    sycl::device selected_device;
    std::unordered_map<int, std::string> documentIndex;
    std::unordered_map<std::string, std::vector<std::pair<int, int>>> invertedIndex;
    const std::unordered_map<int, std::string> &getDocumentIndex() const;
    const std::unordered_map<std::string, std::vector<std::pair<int, int>>> &getInvertedIndex() const;
    std::vector<std::thread> threads;
    std::mutex metaMutex;
    sycl::queue q;
};