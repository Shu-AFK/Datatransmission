#define WIN32_LEAN_AND_MEAN

#include "client.h"

// TODO: Need to add authentication on client side

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

/*
 * main
 *
 * It validates the command line parameters and creates an instance of
 * the Client class. It then calls the run() method of the
 * Client object. If any error occurs, it catches the
 * std::runtime_error exception, logs the error message to
 * a log file and displays it on the stderr.
 *
 * Parameters:
 *   - argc: The number of command line parameters
 *   - argv: An array of command line parameters
 *
 * Returns:
 *   Returns 1 if the command line parameters are invalid,
 *   otherwise it does not return.
 */
int _cdecl main(int argc, char **argv)
{
    // Validate the parameters
    if (argc == 2 || argc == 3) {
        Client client(argv[1]);
        try {
            client.run();
        } catch(std::runtime_error &e) {
            std::ofstream log("log.txt", std::ios::app);
            log << e.what() << std::endl;
            log.close();
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    printf("usage: %s server-name [optional port]\n", argv[0]);
    return 1;
}