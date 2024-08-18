# Data Transmission Project

## 1. Project Overview

This project implements a simple socket connection between a client and a server. It provides basic shell commands that allow clients to navigate the Server PC, including the ability to transfer files from the server to the client.

## 2. Dependencies

This project relies on the following libraries and tools:

- [CMake](https://cmake.org/) (version 3.12 or higher) for the build system.
- [SQLite3](https://www.sqlite.org/) for users' database handling.
- [Winsock Library](https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-start-page-2) for TCP/IP networking.
- [LZ4](https://lz4.github.io/lz4/) for efficient and fast data compression.
- [libsodium](https://libsodium.gitbook.io/doc) for cryptogrphic operations, including hashing and verifying passwords.
- [Vcpkg](https://github.com/microsoft/vcpkg) for managing C++ libraries on Windows, macOS, and Linux.
- [Dear ImGui](https://github.com/ocornut/imgui) for creating client graphical user interfaces.
- A C++ Compiler with support for C++23 standard.

### Package install script

To use, open a command prompt **as an administrator** and navigate to the directory containing install_packages.bat. Run the script using the following command

```shell
install_packages.bat <path_to_vcpkg_directory> [-install]
```

The -install flag is optional and indicates that the script should clone and build vcpkg if it's not already installed.

## 3. Full Unix Commands and Extensions implemented

### 3.1 Unix Commands

| Command | Description                                                | Example Usage                  |
|---------|------------------------------------------------------------|--------------------------------|
| `ls`    | Lists all files and directories in the current directory.  | `ls /path/to/directory`        |
| `cd`    | Changes the current directory to the specified one.        | `cd /path/to/directory`        |
| `pwd`   | Prints the absolute path of the current working directory. | `pwd`                          |
| `cat`   | Concatenates and displays the contents of files.           | `cat my_file.txt`              |
| `echo`  | Outputs the input string.                                  | `echo Hello, World!`           |
| `mkdir` | Creates a new directory.                                   | `mkdir new_directory`          |
| `rmdir` | Removes a directory if it is empty.                        | `rmdir /path/to/directory`     |
| `rm`    | Removes files or directories.                              | `rm my_file.txt`               |
| `touch` | Creates a new empty file.                                  | `touch new_file.txt`           |
| `mv`    | Moves or renames files or directories.                     | `mv old_name.txt new_name.txt` |
| `cp`    | Copies files or directories.                               | `cp source_file target_file`   |
| `cut`   | Cuts the file on the server and moves it to the client.    | `cut abc.txt`                  |
| `find`  | Searches for files in a directory hierarchy.               | `find . -name my_file.txt`     |
| `grep`  | Searches text using patterns.                              | `grep "my pattern" file.txt`   |
| `exit`  | Exits the shell.                                           | `exit`                         |

### 3.2 Extension Commands

| Command          | Description                                              | Example Usage                |
|------------------|----------------------------------------------------------|------------------------------|
| `copy_to`        | Copies a file to the client's PC.                        | `copy_to file.txt`           |
| `copy_from`      | Copies a file from the client's PC to the server.        | `copy_from file.txt`         |
| `get-processes`  | Retrieves all currently running processes on the server. | `get-processes`              |
| `move_startup`   | Runs the Server executable on startup.                   | `move_startup`               |
| `remove_startup` | Cancels the `move_startup` command.                      | `remove_startup`             |
| `check_startup`  | Checks whether the executable file starts on startup.    | `check_startup`              |
| `run`            | Runs executables and .bat scripts.                       | `run script.bat`             |
| `add_user`       | Adds a user to the database                              | `add_user username password` |
| `remove_user`    | Removes a user from the database                         | `remove_user username`       |

## 4. Usage

Both the server (`Server.exe`) and the client (`Client.exe`) interact over a TCP socket connection. Once the connection is established, the client runs a basic shell, facilitating communication between both ends.

### 4.1 Server Usage: `Server.exe`

Here's how you can call `Server.exe` with several command-line flags:
```shell
.\Server.exe -p PORT -n NAME PASSWORD -r NAME --set-startup
```

Read more about these flags [here](FLAGS_FOR_SERVER).

### 4.2 Client Usage: `CLient.exe`

`CLient.exe` can be called similarly, but with different flags (basic user is username: root, password: root):
```shell
.\CLient.exe -s SERVER_IP -p PORT -u USERNAME -w PASSWORD
```

Read more about these flags [here](FLAGS_FOR_CLIENT).

## 5. Installation & Setup

To set up this project on your machine, follow the steps outlined below:

### 5.1 Clone the repository

Clone the repository onto your local machine using the following command in your command line:
```shell
git clone https://github.com/Shu-AFK/Datatransmission.git
```

### 5.2 Navigate to the project directory

Using the command line, navigate into the newly created project directory:
```shell
cd Datatransmission
```

### 5.3 Build the project 

Next, you'll need to build the project. You can do this by running the provided `build.bat` script. In the command prompt, execute the script like this:
```shell
.\build.bat "C:\path\to\vcpkg"
```

The batch script will create a `build` directory if one doesn't exist, configure the project using CMake with the provided vcpkg path, and then build the project in release mode.

### 5.4 Run the project

After building the project, you can run the server and client applications. These applications are `Server.exe` and `Client.exe`, with the flags specified earlier.

## 6. Contributing

We welcome all contributions! Please read our [CONTRIBUTING.md](CONTRIBUTING) for detailed information on how you can contribute.

## 7. License

This project is distributed under the MIT License. See our [LICENSE](LICENSE.txt) for more details.

## 8. Code of Conduct

To ensure we all stay on the same page about the behavior that crafts a more productive and enjoyable space,
a Code of Conduct is outlined for this project. Find the Code of Conduct 
[here](CODE_OF_CONDUCT.md), and keep spreading positivity!
