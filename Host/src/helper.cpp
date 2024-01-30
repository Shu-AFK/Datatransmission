#include "helper.h"
#include <fstream>
#include <string>
#include <algorithm>
#include <filesystem>

int create_scripts(char *port) {
    std::ofstream OpenPortScript(R"(..\..\Host\src\scripts\OpenFirewallPort.bat)");
    std::ofstream  CheckIfOpenScript(R"(..\..\Host\src\scripts\CheckIfOpen.bat)");

    if(!OpenPortScript.is_open() || !CheckIfOpenScript.is_open()) {
        fprintf(stderr, "Couldn't write to files!");
        return 1;
    }

    OpenPortScript << "@echo off\n"
                      "SET PORT_NUMBER=" << port << "\n"
                                                            "SET RULE_NAME=TCP Port %PORT_NUMBER%\n"
                                                            "\n"
                                                            "netsh advfirewall firewall add rule name=\"%RULE_NAME%\" dir=in action=allow protocol=TCP localport=%PORT_NUMBER%\n"
                                                            "if %ERRORLEVEL% equ 0 (\n" // If it worked return 0, else return 1
                                                            "   exit /b 0"
                                                            ") else (\n"
                                                            "   exit /b 1\n"
                                                            ")";

    CheckIfOpenScript << "@echo off\n"
                         "SET PORT_NUMBER=" << port << "\n"
                                                               "SET RULE_NAME=TCP Port %PORT_NUMBER%\n"
                                                               "\n"
                                                               "netsh advfirewall firewall show rule name=all | findstr /C:\"LocalPort:%PORT_NUMBER% \" /C:\"Enabled:Yes\" /C:\"Action:Allow\" /C:\"Direction:In\"\n"
                                                               "\n"
                                                               "IF %ERRORLEVEL% equ 1 (\n" // If port is not open return 0, else return 1
                                                               "    exit /b 0\n"
                                                               ") ELSE (\n"
                                                               "    exit /b 1\n"
                                                               ")";


    OpenPortScript.close();
    CheckIfOpenScript.close();

    return 0;
}

int run_init() {
    const char *OpenScript = R"(..\..\Host\src\scripts\OpenFirewallPort.bat)";
    const char *CheckIfOpenScript = R"(..\..\Host\src\scripts\CheckIfOpen.bat)";

    // Execute the script
    FILE* pipe = _popen(CheckIfOpenScript, "r");

    if(!pipe) {
        fprintf(stderr, "Couldn't run %s\n", CheckIfOpenScript);
        return 1;
    }

    auto returnCodeCheck = _pclose(pipe);
    if(returnCodeCheck == 0) {
        FILE* open = _popen(OpenScript, "r");
        if(!open)
        {
            fprintf(stderr, "Couldn't run %s\n", OpenScript);
            return 1;
        }

        auto returnCodeOpen = _pclose(open);
        if(returnCodeOpen == 1) {
            fprintf(stderr, "Couldn't open firewall port\n");
            return 1;
        }

        return 0;
    }

    return 0;
}

std::string buff;
std::string newPath;

int run_ls(std::string path)
{
    buff.clear();

    // Remove the white spaces from the path
    newPath = path;
    newPath.erase(std::remove_if(newPath.begin(), newPath.end(), ::isspace), newPath.end());

    // Create the command that needs to be executed
    buff = "ls \"" + newPath + R"(" > ..\..\Host\src\scripts\out.txt)";

    // Execute the command
    return system(buff.c_str());
}