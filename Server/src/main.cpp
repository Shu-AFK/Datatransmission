#define WIN32_LEAN_AND_MEAN

#include "server.h"

static constexpr const char* DEFAULT_PORT = "27015";

/**
 * @brief Prints the usage message for the HostExec program.
 *
 * @details
 * This function prints the usage message which explains the command line options for the HostExec program.
 * It provides information on how to specify the server port number, add users, remove users, and display the usage message.
 * An example usage is also provided.
 */
void print_usage() {
    std::cout << "Usage: ./HostExec.exe [OPTION]...\n"
              << "Options:\n"
              << "  -p PORT          specifies the port number for the server.\n"
              << "  -n NAME PASSWORD adds a user with the given name and password.\n"
              << "  -r NAME          removes a user with the given name.\n"
              << "  -h               prints this usage message.\n"
              << "  --set-startup    Boots the executable on server startup.\n"
              << "Example:\n"
              << "  ./HostExec.exe -p 9000 -n john password -r mary\n";
}

bool add = false;
std::string add_name;
std::string add_pass;

bool rem = false;
std::string rem_name;

bool set_startup = false;

/**
 * @brief Handles the command line arguments and assigns values to corresponding variables.
 *
 * This function parses the command line arguments and assigns values to the port, add, add_name,
 * add_pass, rem, and rem_name variables accordingly.
 *
 * @param[in] argc The number of command line arguments.
 * @param[in] argv An array of C-style strings containing the command line arguments.
 * @param[out] port A reference to a string where the port number will be stored.
 *
 * @throws std::runtime_error If incorrect usage is detected.
 * @see print_usage()
 */
void handle_args(int argc, char **argv, std::string &port) {
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-p") == 0) {
            if(i + 1 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }
            port = argv[i + 1];
            i++;
        }

        else if(strcmp(argv[i], "-n") == 0) {
            if(i + 2 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }
            add = true;
            add_name = argv[i + 1];
            add_pass = argv[i + 2];
            i += 2;
        }

        else if(strcmp(argv[i], "-h") == 0)
            print_usage();

        else if(strcmp(argv[i], "-r") == 0) {
            if(i + 1 >= argc) { print_usage(); throw std::runtime_error("Incorrect usage"); }
            rem = true;
            rem_name = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i], "--set-startup") == 0)
            set_startup = true;
    }
}

/**
 * @brief The main function of the program.
 *
 * It initializes a Server object and runs it.
 * If an error occurs during the server's execution, it is logged to a file and displayed on the console.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return The exit status of the program.
 */
int main(int argc, char **argv) {
    std::string port = DEFAULT_PORT;
    try {
        handle_args(argc, argv, port);
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    Server server(port);

    if(add) {
        if(server.addUser(add_name, add_pass) == -1)
            return EXIT_FAILURE;
    }

    if(rem) {
        if(server.remUser(rem_name) == -1)
            return EXIT_FAILURE;
    }

    if(set_startup) {
        int res = server.addStartup();

        if(res == -1)
            return EXIT_FAILURE;
        else if(res == -2)
            std::cerr << "Executable already in startup!" << std::endl;
    }

    try {
        int res = server.run();

        if(res == 1) {
            std::ofstream log("log.txt", std::ios::app);
            log << "Error in run function" << std::endl;
            log.close();
            std::cerr << "Error in run function" << std::endl;
        }
    }

    catch (const std::runtime_error &e) {
        std::ofstream log("log.txt", std::ios::app);
        log << e.what() << std::endl;
        log.close();
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
