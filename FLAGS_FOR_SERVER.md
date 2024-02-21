# Detailed Command-Line Flags for `Server.exe`

The executable for the server, `Server.exe`, can be called with various command-line flags that determine its behavior.

Here's a detailed breakdown:

- `-p PORT` – Specifies the port number for the server to listen on.
  
  Replace "PORT" with your desired port number. The application will use a default port if this flag is not provided. For example: `-p 9000`.

- `-n NAME PASSWORD` – Adds a user with a specified "NAME" and "PASSWORD".
  
  This feature enables the server to manage multiple users with different credentials. For example: `-n john password123`.

- `-r NAME` – Removes a user with a given "NAME".
  
  Use this flag to revoke access from a specific user. For example: `-r john`.

- `-h` – Provides a usage message that lists these flags and explains how to utilize them.
- `--set-startup` - Enables the executable to start upon booting up.
- `--set-cwd` - Sets the current working directory. For example: `--set-cwd C:\`.