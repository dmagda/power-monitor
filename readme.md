# Create Executable

Download build tools:

* Download and Install the [Visual Studio Build Tools](https://visualstudio.microsoft.com/visual-cpp-build-tools/).
* Select the "Desktop development with C++" option during installation. This will include required compiler, linker and other tools.

Set Up Environment for `cl` compiler:

* Open "Developer Command Prompt" by going to the Start Menu and searching for "x64 Native Tools Command Prompt for VS 2022" or a similar name based on your Visual Studio version.
* Run `cl` to make sure the compiler is ready.

Create executable:
* Go to the project root with the `power-monitor.c` file.
* Compile and link the project `cl power-monitor.c /link user32.lib wtsapi32.lib`

Execute the `power-monitor.exe` to start receiving the events!