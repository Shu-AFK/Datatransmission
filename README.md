# Data transmission

## Description
This project implements a simple socket connection between a client and a server. As the client you can 
use basic shell commands to traverse the host pc, even transferring files from the server to the client.

## Shell commands implemented:
    ls - List all files and directories in the current directory. 
    cd - Change current directory. 
    pwd - Print working directory. 
    cat - Concatenate and display the content of files.
    echo - Output the inputs.
    mkdir - Create a new directory. 
    rmdir - Removes a directory. 
    rm - Remove files or empty directories. 
    touch - Create an empty file. 
    mv - Move or rename files or directories.
    cp - Copy files or directories.
    find - Search for files in a directory hierarchy.
    grep - Search text using patterns.
    exit - Exit the shell. 
    copy_pc - copy's a file to the client pc

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
    mkdir build
    cd build
    cmake ..
    make
    ```
   
4. **Move the files**

    Building the project results in two executable files.
     - Move the HostExec.exe file to the PC you want to access.
     - Move the ReceiverExec.exe file to the PC you want to excess the Host PC from.

5. **Run both files**

    Lastly you can run the HostExec.exe file using:

    ```bash
    HostExec.exe
   ```
   
    Now you can run the ReceiverExec.exe file using:
    
    ```bash
    HostExec.exe
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


## License
Distributed under the MIT License. See [LICENSE](LICENSE.txt) for more information.
