#define WIN32_LEAN_AND_MEAN

#include "client.h"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int _cdecl main(int argc, char **argv)
{
    // Validate the parameters
    if (argc == 2 || argc == 3) {
        Client client(argv[1]);
        try {
            client.run();
        } catch(std::runtime_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    printf("usage: %s server-name [optional port]\n", argv[0]);
    return 1;
}