#ifndef DATATRANSMISSION_STATE_H
#define DATATRANSMISSION_STATE_H

#include <vector>

#include "structs.h"

int saveState(const std::string &filename, const std::vector<Connection> &connections, std::string &error);
int loadState(const std::string &path, std::vector<Connection> &connections, std::string &error);

bool isSingleByteChars(const std::string& str);
void appendBinaryLength(std::stringstream& stream, uint32_t length);
std::string getState(const std::vector<Connection>& connections);
std::string compressState(const std::string &state);
std::string decompressState(const std::string& compressedData, int originalSize);

#endif //DATATRANSMISSION_STATE_H
