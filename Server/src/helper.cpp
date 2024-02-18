#include "helper.h"
#include <string>
#include <cstdlib>
#include <format>
#include <fstream>

/**
 * @brief Runs the initialization process for opening a firewall port.
 *
 * @details
 * This function runs the necessary scripts to open a firewall port on the host machine.
 * It first checks if the port is already open using the CheckIfOpen script, and if not,
 * it runs the OpenFirewallPort script to open the specified port.
 *
 * @param port The port number to be opened.
 * @return 0 if the port is successfully opened, 1 otherwise.
 */
int run_init(std::basic_string<char> port) {
    std::string OpenScript = std::format(R"(..\..\Server\src\scripts\OpenFirewallPort.bat {})", port);
    std::string CheckIfOpenScript = std::format(R"(..\..\Server\src\scripts\CheckIfOpen.bat {})", port);

    if(system(CheckIfOpenScript.c_str()) == 0) {
        if(system(OpenScript.c_str()) == 1) {
            fprintf(stderr, "Couldn't open firewall port\n");
            return 1;
        }
    }

    return 0;
}


/**
 * @brief Creates a start script.
 *
 * @details
 * This function creates a start script with the specified current working directory (cwd) and port.
 * The start script is written to the relative path ..\..\Host\src\scripts\run_exec.bat.
 *
 * @param cwd The current working directory for the start script.
 * @param port The port parameter for the start script.
 *
 * @return 0 if the start script is successfully created, otherwise return 1.
 */
int create_start_script(const std::string& cwd, const std::string& port) {
    std::ofstream run_script(R"(..\..\Server\src\scripts\run_exec.bat)");
    if(!run_script)
        return 1;

    run_script << std::format(R"(start "" "{}\Server.exe" "{}")", cwd, port);
    run_script.close();
    return 0;
}