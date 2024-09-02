#ifndef DATATRANSMISSION_STATE_H
#define DATATRANSMISSION_STATE_H

#include <vector>

#include "structs.h"

int safeState(const std::string &filename, const std::vector<Connection> &connections);
int loadState(const std::string &path);

#endif //DATATRANSMISSION_STATE_H
