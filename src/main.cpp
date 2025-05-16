// src/main.cpp
#include "DualQueue.hpp"
#include "DataBlock.hpp"
#include "Reader.hpp"
#include "IndexBlock.hpp"
#include "Indexer.hpp"
#include "Search.hpp"
#include <iostream>
#include <chrono>

#include <sycl/sycl.hpp>

void print_device_info(const sycl::device &dev, int index)
{
    std::cout << "[" << index << "] "
              << dev.get_info<sycl::info::device::name>() << " (";

    auto type = dev.get_info<sycl::info::device::device_type>();
    switch (type)
    {
    case sycl::info::device_type::cpu:
        std::cout << "CPU";
        break;
    case sycl::info::device_type::gpu:
        std::cout << "GPU";
        break;
    case sycl::info::device_type::accelerator:
        std::cout << "Accelerator";
        break;
    case sycl::info::device_type::host:
        std::cout << "Host";
        break;

    default:
        std::cout << "Unknown";
    }

    std::cout << ")\n";
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input_directory> <num_threads>\n";
        return 1;
    }

    std::string inputPath = argv[1];
    int numThreads = std::atoi(argv[2]);

    std::vector<sycl::device> all_devices;

    auto platforms = sycl::platform::get_platforms();
    int dev_index = 0;

    for (auto &platform : platforms)
    {
        auto devices = platform.get_devices();
        for (auto &dev : devices)
        {
            all_devices.push_back(dev);
            print_device_info(dev, dev_index++);
        }
    }

    if (all_devices.empty())
    {
        std::cerr << "No SYCL devices found.\n";
        return 1;
    }

    int choice;
    std::cout << "\nSelect device index to run on: ";
    std::cin >> choice;

    sycl::device selected_device = all_devices[choice];

    DualQueue<DataBlock> dataQueue;

    Reader reader(dataQueue, inputPath);
    Indexer indexer(dataQueue, numThreads, selected_device);

    reader.start();
    reader.join(); // Wait until all files are read

    auto start_time = std::chrono::high_resolution_clock::now();
    indexer.start();
    indexer.join();
    auto end_time = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> indexing_duration = end_time - start_time;
    std::cout << "\n[Pure Indexing Completed in " << indexing_duration.count() << " seconds]\n";

    while (true)
    {
        std::cout << "\nEnter Search Terms (or type 'exit' to quit): ";
        std::string query;
        std::getline(std::cin, query);
        if (query == "exit")
            break;
        search(indexer, query);
    }

    std::cout << "[Done] Program exiting.\n";
    return 0;
}

// dpcpp -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Iinclude src/*.cpp -o file_retrieval_engine
// icpx -std=c++17 -fsycl -fsycl-targets=nvptx64-nvidia-cuda -Iinclude src/*.cpp -o file_retrieval_engine

// icpx -std=c++17 -fsycl -fsycl-targets=spir64_x86_64,nvptx64-nvidia-cuda -Iinclude src/*.cpp -o file_retrieval_engine


// ./file_retrieval_engine ~/dataset1 1