// include/Search.hpp
#pragma once
#include "Indexer.hpp"
#include <string>
#include <vector>

std::vector<std::string> tokenize_query(const std::string& text);
void search(const Indexer& indexer, const std::string& query);