// src/Indexer.cpp
#include "Indexer.hpp"
#include <sstream>
#include <iostream>
#include <mutex>
#include <cctype>
#include <sycl/sycl.hpp>

namespace
{
    std::vector<std::string> tokenize(const std::string &text)
    {
        std::vector<std::string> tokens;
        std::string token;
        for (char ch : text)
        {
            if (std::isalnum(ch))
            {
                token += std::tolower(ch);
            }
            else if (!token.empty())
            {
                tokens.push_back(token);
                token.clear();
            }
        }
        if (!token.empty())
        {
            tokens.push_back(token);
        }
        return tokens;
    }
}

Indexer::Indexer(DualQueue<DataBlock> &inputQueue, int numThreads, sycl::device selected_device)
    : inputQueue(inputQueue), nextDocID(1), numThreads(numThreads), q(selected_device)
{
    std::cout << "Running app on: "
              << q.get_device().get_info<sycl::info::device::name>() << "\n";
}

void Indexer::start()
{
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back(&Indexer::process_blocks, this);
    }
}

void Indexer::join()
{
    for (auto &thread : threads)
    {
        if (thread.joinable())
            thread.join();
    }
}

void Indexer::process_blocks()
{
    DataBlock block;
    while (inputQueue.pop(block))
    {
        int docID = nextDocID++;

        {
            std::lock_guard<std::mutex> lock(metaMutex);
            documentIndex[docID] = block.filePath;
        }

        const std::string &content = block.content;
        size_t textSize = content.size();

        std::vector<int> isWordChar(textSize, 0);

        {
            sycl::buffer<char, 1> content_buf(content.data(), sycl::range<1>(textSize));
            sycl::buffer<int, 1> isWordChar_buf(isWordChar.data(), sycl::range<1>(textSize));

            q.submit([&](sycl::handler &h)
                     {
                auto content_acc = content_buf.get_access<sycl::access::mode::read>(h);
                auto isWordChar_acc = isWordChar_buf.get_access<sycl::access::mode::write>(h);

                h.parallel_for(sycl::range<1>(textSize), [=](sycl::id<1> i) {
                    char ch = content_acc[i];
                    // isWordChar_acc[i] = (std::isalnum(ch) ? 1 : 0);
                    isWordChar_acc[i] = ((ch >= '0' && ch <= '9') || 
                     (ch >= 'A' && ch <= 'Z') || 
                     (ch >= 'a' && ch <= 'z')) ? 1 : 0;

                }); });
            q.wait();
        }

        std::unordered_map<std::string, int> localFreq;
        std::string currentToken;
        for (size_t i = 0; i < textSize; ++i)
        {
            if (isWordChar[i])
            {
                // currentToken += std::tolower(content[i]);
                currentToken += content[i];
            }
            else
            {
                if (!currentToken.empty())
                {
                    localFreq[currentToken]++;
                    currentToken.clear();
                }
            }
        }
        if (!currentToken.empty())
        {
            localFreq[currentToken]++;
        }

        {
            std::lock_guard<std::mutex> lock(metaMutex);
            for (const auto &[term, freq] : localFreq)
            {
                invertedIndex[term].emplace_back(docID, freq);
            }
        }

        // std::cout << "[Indexed] " << block.filePath << " as docID " << docID << "\n";
    }

    std::cout << "[Done] Indexing complete. Total documents: " << nextDocID - 1 << "\n";
}

const std::unordered_map<int, std::string> &Indexer::getDocumentIndex() const
{
    return documentIndex;
}

const std::unordered_map<std::string, std::vector<std::pair<int, int>>> &Indexer::getInvertedIndex() const
{
    return invertedIndex;
}

std::vector<std::pair<int, int>> Indexer::lookUpTerm(const std::string &term) const
{
    auto it = invertedIndex.find(term);
    if (it != invertedIndex.end())
    {
        return it->second;
    }
    else
    {
        return {};
    }
}

std::string Indexer::getDocument(int docID) const
{
    auto it = documentIndex.find(docID);
    if (it != documentIndex.end())
    {
        return it->second;
    }
    else
    {
        return "Unknown Document";
    }
}
