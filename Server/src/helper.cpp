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
	
	std::string path_to_bat_1;
	std::string path_to_bat_2;
	
	if(auto p = find_path(std::filesystem::current_path(), "scripts")) {
		path_to_bat_1 = std::filesystem::path(*p / "OpenFirewallPort.bat").string();
		path_to_bat_2 = std::filesystem::path(*p / "CheckIfOpen.bat").string();
	}
	else {
		std::cerr << "Can't find scripts folder" << std::endl;
		return 1;
	}
	
    std::string OpenScript = std::format("{} {}", path_to_bat_1, port);
    std::string CheckIfOpenScript = std::format("{} {}", path_to_bat_2, port);

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
 * The start script is written to the relative path ..\..\Server\src\scripts\run_exec.bat.
 *
 * @param cwd The current working directory for the start script.
 * @param port The port parameter for the start script.
 *
 * @return 0 if the start script is successfully created, otherwise return 1.
 */
int create_start_script(const std::string& cwd, const std::string& port) {
	
	std::string path_to_bat;
	
	if(auto p = find_path(std::filesystem::current_path(), "scripts")) {
		path_to_bat = std::filesystem::path(*p / "run_exec.bat").string();
	}
	else {
		std::cerr << "Can't find scripts folder" << std::endl;
		return 1;
	}
	
    std::ofstream run_script(path_to_bat.c_str());
    if(!run_script)
        return 1;

    run_script << std::format(R"(start "" "{}\Server.exe" "-p {}")", cwd, port);
    run_script.close();
    return 0;
}

/**
 * @brief Recursively finds the folder or file.
 *
 * @details
 * This function recursively finds the path to specified folder or file using std::filesystem.
 *
 * @param start_path The path, where starts the recursive search for the file/folder
 * @param target The name of target file/folder
 *
 * @return std::optional<std::filesystem::path> a container for value that may or may not be present. 
 * If found the target returns path, that can be accessed through operator *. (Implicitly converts to true)
 * If not returns std::nullopt. (Implicitly converts to false)
 */
std::optional<std::filesystem::path> find_path(std::filesystem::path start_path, std::string target) {
	try {
		while (start_path != start_path.root_directory()) {
			for (auto const& dir_entry : std::filesystem::recursive_directory_iterator(start_path))
			{
				if (exists(dir_entry.path() / target)) {
					auto p_ret = dir_entry.path();
					return p_ret / target;
				}
			}
			start_path = start_path.parent_path();
		}
		return std::nullopt;
	}
	catch (const std::filesystem::filesystem_error& ex) {
		std::cerr << "Exception: " << ex.what() << '\n';
	}
}
