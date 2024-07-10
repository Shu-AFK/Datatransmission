#ifndef DATATRANSMISSION_HELPER_H
#define DATATRANSMISSION_HELPER_H

#include <string>
#include <optional>
#include <filesystem>
#include <iostream>

int run_init(std::basic_string<char> port);
int create_start_script(const std::string& cwd, const std::string& port);
std::optional<std::filesystem::path> find_path(std::filesystem::path start_path, const std::string& target_directory);

#endif //DATATRANSMISSION_HELPER_H
