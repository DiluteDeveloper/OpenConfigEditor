# OpenConfigEditor

This is an IDE made in C++ with Qt, designed to make it much easier
to edit configuration files, particularly on GNU/Linux.

For me, It's frustrating going between many different configuration
files, all in different languages, with different documentation, and
most of them dont have an LSP/code completion. This project aims to
solve that.

Planned features for this project include:
- Integration with language servers for code completion, formatting,
  and syntax highlighting, along with facilitating the creation of
  language servers that don't already exist for config files.
- Embedded official or unofficial documentation of config files
- Project and filesystem management

## Compiling from source

Currently, as this is very much a personal project that is very early
on, the build system is very much catered to my own system, which uses
the Nix package manager to install and link the Qt packages.

### Using Nix 

If you have the Nix package manager installed, you can simply run the
`nix-shell` command inside of the root directory to create a nix shell
that installs the required Qt packages automatically and puts them on
the system path. Inside of the nix shell, run the `build` command in
the root directory, which will run the build script and put the
intermediate files and the executable in the `build/` folder. 

### Not using Nix

If you are not using Nix, the Qt base package needs to be installed on
your system and on the system path. With Qt base installed, run the
`qmake qt.pro` command inside of the root directory, then run the
`make` command inside of the root directory.

