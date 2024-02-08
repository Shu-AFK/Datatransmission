# Data transmission

## Description
This project implements a simple socket connection between a client and a server. As the client you can 
use basic shell commands to traverse the host pc, even transferring files from the server to the client.

## Unix( + extra) Command Descriptions

Below are descriptions of several commonly-used Unix commands:

- `ls`: This command lists all files and directories in the current directory. Example usage: `ls` or to list the files and directories in some other directory: `ls /path/to/directory`.

- `cd`: This command changes the current directory to the one specified. Example usage: `cd /path/to/directory`.

- `pwd`: This command prints the working directory, i.e., the directory you are currently within. Example usage: `pwd`.

- `cat`: This command concatenates and displays the content of files. Example usage: `cat my_file.txt`.

- `echo`: This command outputs the inputs. Example usage: `echo Hello, World!`.

- `mkdir`: This command creates a new directory. Example usage: `mkdir new_directory`.

- `rmdir`: This command removes a directory. However, it only removes empty directories. Example usage: `rmdir /path/to/directory`.

- `rm`: This command removes files or directories. Example usage: `rm my_file.txt`.

- `touch`: This command creates an empty file. Example usage: `touch new_file.txt`.

- `mv`: This command moves or renames files or directories. Example usage: `mv old_name.txt new_name.txt` or `mv file /path/to/new/location`.

- `cp`: This command copies files or directories. Example usage: `cp source_file target_file`.

- `find`: This command searches for files in a directory hierarchy. Example usage: `find . -name my_file.txt`.

- `grep`: This command searches text using patterns. Example usage: `grep "my pattern" file.txt`.

- `exit`: This command exits the shell. Simply type `exit` and hit enter.

- `copy_to`: This command copies a file to the client's PC. Example usage: `copy_to file.txt`.

- `copy_from`: this command copies a file from the client's PC to the server. Example usage: `copy_from file.txt`.

- `move_startup`: This command runs the host executable on startup. Usage: `move_startup`.

- `remove_startup`: This command reverts the previous command. Usage: `remove_startup`.

- `check_startup`: This command checks if the exe file gets started on startup. Usage: `check_startup`.

- `run`: This command runs executables and .bat scripts. Usage: `run script.bat`.

## How it works:
The server and the client engage in a tcp socket connection. Once the connection is established, a basic shell is 
started on the client pc, allowing for communication between the client and the server. 

## Installation: 

Follow the steps below to install the project:

1. **Clone the Repository**

    First, clone the repository to your local machine using git:
   ```bash
    git clone https://github.com/Shu-AFK/Datatransmission
   ```

2. **Navigate to the project directory**

   Go to the project directory via the command line:

    ```bash
    cd Datatransmission
    ```
   
3. **Make the project**

   The project uses CMake as its build system, you can download it as follows:
   - Open a web browser and navigate to the official CMake download page: https://cmake.org/download/
   - Under "Current Release", find the "Binary distributions" section.
   - Download the Windows win64-x64 Installer, or Windows x86 installer for 32-bit Windows.
   - After the download is complete, you can install CMake by running the installer and following the prompts.
   - During installation, you will be asked whether to add CMake to the system PATH. It's recommended to do so to be able to run CMake from the command line from anywhere.

   Now you can build the project as follows:

    ```bash
    mkdir build && cd build
    cmake -G "MinGW Makefiles" ..
    make
    ```
   
4. **Move the files**

    Building the project results in two executable files.
     - Move the HostExec.exe file to the PC you want to access.
     - Move the ReceiverExec.exe file to the PC you want to excess the Host PC from.

5. **Run both files**

    Lastly you can run the HostExec.exe file using (please make sure to run with admin privileges):

    ```bash
    HostExec.exe [optional port]
   ```
   
    Now you can run the ReceiverExec.exe file using:
    
    ```bash
    HostExec.exe [server IP] [optional port]
   ```
   
    And the connection should be established.

## Contributing
Any contributions you make are **greatly appreciated**.

1. **Fork** the Project
2. Create your **Feature Branch** (git checkout -b feature/AmazingFeature)
3. **Commit** your Changes (git commit -m 'Add some AmazingFeature')
4. **Push** to the Branch (git push origin feature/AmazingFeature)
5. Open a **Pull Request**

Please explain any changes to the codebase in detail in your pull request and make sure
you resolve any conflicts with the main branch before submitting the pull request.
Also, please make sure to write good documentation for any feature added.


## License
Distributed under the MIT License. See [LICENSE](LICENSE.txt) for more information.
