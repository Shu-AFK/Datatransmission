#define WIN32_LEAN_AND_MEAN

#include <cstdlib>
#include <memory>

#include "client.h"

std::unique_ptr<Client> client;

/**
* @brief Prints the usage information for the program.
* @details
* This function prints the usage information for the program, including the necessary command line options and their descriptions.
*/
void print_usage() {
    std::cout << "Usage: ./Client.exe [OPTION]...\n"
              << "You must specify the -s, -p, -u and -w flags.\n"
              << "Options:\n"
              << "  -s SERVER_IP     Specifies the IP of the server for connection. Example: -s 192.168.1.100.\n"
              << "  -p PORT          Defines the port number to connect on the server. Example: -p 9000.\n"
              << "  -u USERNAME      Uses a specific username when connecting to the server. Example: -u john.\n"
              << "  -w PASSWORD      Uses a specific password when connecting to the server. Example: -w password123.\n"
              << "  -h               Prints this usage message.\n"
              << "Example:\n"
              << "  ./Client.exe -s 192.168.1.100 -p 9000 -u john -w password123\n";
}

std::string ip, port, username, password;

/**
 * @brief Parses command line arguments to get the flags and their values.
 * @param argc The number of command line arguments.
 * @param argv An array of command line arguments.
 * @throws std::runtime_error If incorrect usage is detected.
 */
void get_flags(int argc, char **argv) {
    bool check[4];

    if(argc < 9)  { print_usage(); throw std::runtime_error("Incorrect usage"); }

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-s") == 0) {
            if(i + 1 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }

            check[0] = true;
            ip = argv[i + 1];
            i++;
        }
        if(strcmp(argv[i], "-p") == 0) {
            if(i + 1 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }

            check[1] = true;
            port = argv[i + 1];
            i++;
        }
        if(strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }

            check[2] = true;
            username = argv[i + 1];
            i++;
        }
        if(strcmp(argv[i], "-w") == 0) {
            if (i + 1 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }

            check[3] = true;
            password = argv[i + 1];
            i++;
        }
    }

    for(int i = 0, size = sizeof(check) / sizeof(bool); i < size; i++) {
        if(!check[i]) {
            print_usage();
            throw std::runtime_error("Incorrect usage");
        }
    }
}

void onClose() {
    if (client) {
        client->closeConnection();
    }
}

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
    try {
        get_flags(argc, argv);
        client = std::make_unique<Client>(ip, port, username, password);
        std::atexit(onClose);
        client->run();
        return EXIT_SUCCESS;
    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        std::ofstream log("log.txt", std::ios::app);
        log << e.what() << std::endl;
        log.close();
        return EXIT_FAILURE;
    }
}