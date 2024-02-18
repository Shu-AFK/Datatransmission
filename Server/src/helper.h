#ifndef DATATRANSMISSION_HELPER_H
#define DATATRANSMISSION_HELPER_H

#include <string>

int run_init(std::basic_string<char> port);
int create_start_script(const std::string& cwd, const std::string& port);

#endif //DATATRANSMISSION_HELPER_H
