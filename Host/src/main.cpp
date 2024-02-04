#define WIN32_LEAN_AND_MEAN

#include "server.h"

static constexpr const char* DEFAULT_PORT = "27015";

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
    if(argc == 1 || argc == 2) {
        std::string port;
        if(argc == 2)
            port = argv[1];
        else
            port = DEFAULT_PORT;

        Server server(port);

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
    else {
        std::cout << "Usage: " << argv[0] << " [optional port]" << std::endl;
        return EXIT_FAILURE;
    }
}