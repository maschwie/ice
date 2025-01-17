# Building Ice for C++ on Windows

This file describes how to build Ice for C++ from sources on Windows, and how
to test the resulting build.

ZeroC provides [Ice binary distributions][1] for various platforms and compilers,
including Windows and Visual Studio, so building Ice from sources is usually
unnecessary.

## Build Requirements

### Operating Systems and Compilers

Ice was extensively tested using the operating systems and compiler versions
listed for our [supported platforms][2].

### Third-Party Libraries

Ice has dependencies on a number of third-party libraries:

 - [bzip][3] 1.0
 - [expat][4] 2.1
 - [LMDB][5] 0.9.16
 - [mcpp][6] 2.7.2 (with patches)

You do not need to build these packages yourself, as ZeroC supplies
[Nuget][7] packages for all these third party dependencies.

The Ice build system for Windows downloads and installs Nuget and these
Nuget packages when you build Ice for C++. The third-party packages
are installed in the ``ice/cpp/third-party-packages`` folder.

## Building Ice

Open a command prompt that is configured for your target architecture. For
example, when using Visual Studio 2013, you have several alternatives:

- Developer Command Prompt
- VS2013 x86 Native Tools Command Prompt
- VS2013 x64 Native Tools Command Prompt
- VS2013 x64 Cross Tools Command Prompt

Using the first two configurations produces 32-bit binaries, while the third and
fourth configurations produce 64-bit binaries.

In the command window, change to the `cpp` subdirectory:

    cd cpp

Edit `config\Make.rules.mak` to establish your build configuration. The comments
in the file provide more information.

Now you're ready to build Ice:

    nmake /f Makefile.mak

This will build the Ice core libraries, services, and tests.

### x64 Platform

Building Ice for x64 with Visual Studio is like building Ice for x86. You just need to
perform the build in an "x64 Command Prompt", and not in a regular "Developer Command Prompt".

### C++11 mapping

The C++ source tree supports two different language mappings (C++98 and C++11), 
the default build uses the C++98 map. The C++11 mapping is a new mapping that
uses the new language features.

To build the new C++11 mapping, set the environment variable `CPP11_MAPPING` to
yes, as shown below:

    set CPP11_MAPPING=yes

## Installing a C++ Source Build

Simply run `nmake /f Makefile.mak install`. This will install Ice in the
directory specified by the `prefix` variable in `config\Make.rules.mak`.

If you built a 64-bit version of Ice, the binaries are installed in the
`bin\x64` directory and the libraries are installed in the `lib\x64` directory.

## Running the Test Suite

Python is required to run the test suite. Additionally, the Glacier2 tests
require the Python module `passlib`, which you can install with the command:

    pip install passlib

After a successful source build, you can run the tests as follows:

    python allTests.py
    
For C++11 mapping you need to use the`--c++11` argument:

    $ python allTests.py --c++11

If everything worked out, you should see lots of `ok` messages. In case of a
failure, the tests abort with `failed`.

[1]: https://zeroc.com/download.html
[2]: https://doc.zeroc.com/display/Ice37/Supported+Platforms+for+Ice+3.7.0
[3]: http://bzip.org
[4]: http://expat.sourceforge.net
[5]: http://symas.com/mdb/
[6]: https://github.com/zeroc-ice/mcpp
[7]: https://www.nuget.org
