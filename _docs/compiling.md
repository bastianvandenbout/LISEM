---
title: Compiling LISEM yourself
permalink: /docs/compiling/
redirect_from: /docs/index.html
---

We welcome anyone to compile LISEM yourself, and contribute to the development. All of the required software and dependencies can be obtained for free and are open-source.
Despite this, compiling all of the external depencies can take a lot of work, so we provide a full set of pre-compiled dependencies to which you can link during compilation.

When you want to compile LISEM yourself, the following steps must usually be taken:
1.	Install MSYS2
2.  Install the required library packages
3.  Get the LISEM Source code using Git
4.  Download the pre-compiled external dependencies
5.  Create build files using CMAKE
6.  Compilation

### 1. Install MSYS2
MSYS2 is a highly usefull development environment for c++ code that uses a package manager and features a large number of pre-built libraries as packages.
It comes with the most recent version of the gcc compiler, but other compilers such as clang can also be used.
We will use gcc to ease compilation. It is typically easier to add the msys64/mingw64/bin directory to your environment variable PATH.
Download [MSYS2](https://www.msys2.org/)

### 2. Install the required library packages
Using the MSYS2 package maneger, install the required library packages. We provide a list that can be automatically installed using the MSYS2 Mingw64 shell.
[package script](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Compile/packages_windows.txt/download).
Run the script using the command 'sh packages_windows.txt'

### 3. Get the LISEM Source code using Git
Use git to download the source code from the reposity [https://github.com/bastianvandenbout/LISEM](https://github.com/bastianvandenbout/LISEM).
There are multiple branches in the repository. The "main" branch is the currently published version. The "develop" branch contains the recent developments after the latest release.
Finally, the "website" branch contains the files for this website.

![GitPage](/assets/img/gitpage.png)

### 4. Download the pre-compiled external dependencies
The pre-compiled dependencies are available for windows and available as a multi-part 7zip file on sourceforge. Download them all the extract them.
[Compiled dependencies zip files](https://sourceforge.net/projects/lisem/files/LISEM%20%28beta%20release%20new%20software%29/Compile/).
One additional external dependency must be installed seperately, which is the [Microsoft MPI framework](https://docs.microsoft.com/en-us/message-passing-interface/microsoft-mpi). 

### 5. Create build files using CMAKE
Install [CMAKE GUI](https://cmake.org/) (or optionally use cmake from the command line). Open the source directory, and specify the build directory.
By defeault, LISEM will try to find the pre-compiled dependencies and search for those in the folder LISEM_EXTERNAL_DIR. Set this variable to the right directory to continue.
If you dont want to use the LISEM_EXTERNAL_DIR, disable LISEM_USE_EXTERNAL_DIR.

![CMAKE](/assets/img/cmake.png)

### 6. Compilation
Once the build files have been generated, open qt creator or another IDE. Qt creator should be installed by msys2, so check the mingw64/bin folder for qtcreator.exe.

![qt](/assets/img/qtopen.png)

Let qt run cmake (or qmake) as well, and finally, press the compile button to start the compilation.

During compilation, several minor fixes might need to be made. 
One of the external libraries present in the compiled dependencies puts the installation folder itself within the file. Go to armadillo_bits/config.hpp and correct the paths in some defines in line 154, 309 and 310.
Qt does not allways realize the location of compiled ui-files. When prompted with an error about "findandreplacedialog.h", replace this include with "widgets/findandreplacedialog.h".

After compilation, make sure to copy all of the required dependencies (for windows, dll files, the gdal data files, image and font assets, shaders and computational shaders) to the build folder.
You might easily take all these files from the latest released version of LISEM.


