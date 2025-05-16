// src/Reader.cpp
#include "Reader.hpp"
#include <iostream>

Reader::Reader(DualQueue<DataBlock>& queue, const std::string& inputPath)
    : dataQueue_(queue), inputPath_(inputPath) {}

void Reader::start() {
    thread_ = std::thread(&Reader::read_files, this);
}

void Reader::join() {
    if (thread_.joinable()) {
        thread_.join();
        dataQueue_.done();
    }
}

void Reader::read_files() {
    for (const auto& entry : fs::recursive_directory_iterator(inputPath_)) {
        if (entry.is_regular_file()) {
            std::ifstream file(entry.path());
            if (file) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                DataBlock block{ entry.path().string(), buffer.str() };
                dataQueue_.push(block);
            }
        }
    }
    std::cout << "[Reader] Done reading files.\n";
}