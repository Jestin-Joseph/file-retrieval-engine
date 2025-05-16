#pragma once
#include "DataBlock.hpp"
#include "DualQueue.hpp"
#include <string>
#include <filesystem>
#include <fstream>
#include <thread>

namespace fs = std::filesystem;

class Reader {
public:
    Reader(DualQueue<DataBlock>& queue, const std::string& inputPath);
    void start();
    void join();

private:
    void read_files();

    DualQueue<DataBlock>& dataQueue_;
    std::string inputPath_;
    std::thread thread_;
};
