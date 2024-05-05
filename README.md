## x11-calc - Another RPN (Reverse Polish) calculator.

Emulators  for  the HP 35, HP 80, HP 45, HP 70, HP 21, HP 22, HP25, HP 25C,
HP 27,  HP 29C,  HP 31E, HP 32E, HP 33E, HP 33C,  HP 34C,  HP 37E,  HP 38C,
HP 38E, HP 67, HP 10C, HP 11C, HP 12C, HP 15C, and HP 16C.

All written in C using X11.

Use  of any language extensions or non standard language features has  been
avoided in order to try to make the code as portable as possible.

The  aim  is to have the same source code compile without  modification  on
as  many systems and architectures as possible including Linux, VAX/VMS and
Tru64 Unix.

![HP10](./img/x11-calc-10c.png) ![HP11](./img/x11-calc-11c.png)

More [screenshots](./img/#top)


### Latest News

05 May 24

   - Fixed a bug that affected some simulators when compiled on MacOS.
   - Updated this README with more details of the keyboard shortcuts.
   - Added some much needed acknowledgements.

14 Apr 24

   - Allows the size of the window to be adjusted using the `--zoom n` option
     where n can be between 0 and 4.
   - Added an install option to the makefile (by default the installer will
     use `$HOME/.local` if it exists, but it is possible to specify another
     directory by setting the directory `prefix`).
   - Embedded missing firmware.

24 Feb 24
   - By default the application will attempt to use the X11 base fonts. But
     if these are not available it will try to select an suitable alternate
     font instead from a predefined list.

     For all the fonts to be rendered as intended users should ensure  that
     the X11 base fonts are installed (see prerequisites).

16 Feb 24
   - For UNIX based systems the default location used to store the state of
     the machine when it is powered off has changed.
     If the data file already exists in `$HOME` then this will always be used
     by default, but if it does not exist then if `$XDG_DATA_HOME` is defined
     the program will create a sub directory in this location for the  data
     files or in or `$HOME/.local/share/` if `$XDG_DATA_HOME` is not defined or
     does not exist. If `$HOME/.local/share/` does not exist then the program
     will use `$HOME` as before.

01 Nov 23
   - Updated DCL make script.

14 Oct 23
   - Updated build instructions.
   - Uses a simpler display on ARM (excluding Apple).

22 Sep 23
   - Added ability to build using make on MacOS.

### How to get it

   - [Compile and install from source](#Compiling)

   - [Install a binary package](#Using-a-precompiled-package)

### Compiling

To  build the simulator check that you have all the [prerequisites](#Prerequisites) installed
then download the source code from github and unzip it (this will created a
new directory automatically).

e.g:
```
$ wget https://github.com/mike632t/x11-calc/archive/refs/heads/stable.zip
$ unzip x11-calc-stable.zip
```
Then change directory to the new folder.
```
$ cd x11-calc-stable
```
The to compile all the emulators you just need to invoke make.
```
$ make clean; make all
```
It is also possible to compile a single emulator by specifying the model on
the command line
```
$ make hp29c
```
By default the executable files will be created in the `bin` directory, and
can run either a particular emulator directly, or invoke the launcher which
will allow you to select which enulator to use by default the next time you
launch it.
```
$ ./bin/x11-calc-29c
x11-calc-29c: Version 0.10 [Commit ID: 399d546] 01 Nov 23 23:53:00 (Build: 0114)
ROM Size : 4096 words
```
OR
```
$ bin/x11-calc
```
If more than one C compiler is installed or if gcc is not available you can
specify which one to use from the command line.
```
$ make CC=clang hp11c

$ make CC=tcc
```
#### VMS
On VMS unzip the source code archive, change the default directory and then
run `make.com`.

e.g:
```
$ unzip x11-calc-stable.zip
$ set def [.x11-calc-stable.src]
$ @make all

$ mc [-.bin]x11-calc-29c
x11-calc-29c: Version 0.10 [Commit ID: 399d546] 02 Nov 23 23:52:11 (Build: 0114)
ROM Size : 4096 words
```

### Installing

On Linux systems after the compilation is complete you can use the makefile
to install the emulator locally.

By default the installer will use `$HOME/.local` if it exists, but it is
possible to specify another directory by setting the directory `prefix`.
```
$ make install

OR

$ make install prefix=/usr
```
Installer also supports staged installs in a custom directory defined using
DESTDIR.
```
make DESTDIR=/tmp/staging install
```

### Using a precompiled package

If you don't want to download an compile the sources your self the emulator
is also  available on [Flathub](https://flathub.org/apps/io.github.mike632t.x11-calc) and can be installed using Flatpak.

### Tested

The emulators have been tested on the following systems:

   - Alpine 3.19, gcc 13.2.1, x64 + arm64

   - Debian 12 (Bookworm), clang 14.0.6, x64 + arm64

   - Debian 12 (Bookworm), tcc 0.9.27, x64 + arm64

   - Debian 12 (Bookworm), gcc 12.2.0, x64 + arm64

   - Debian 11 (Bullseye), clang 11.0.1-2, x64

   - Debian 11 (Bullseye), gcc 10.2.1, x64

   - Debian 11 (Bullseye), tcc 0.9.27, x64

   - Debian 10 (Buster), gcc 8.3.0, x64 + arm

   - Debian 10 (Buster), clang 7.0.1, x64

   - Debian 9 (Stretch), gcc 6.3.0, arm

   - Debian 5 (Lenny), gcc 4.2.4, alpha

   - Fedora 34, gcc 11.2.1, x64

   - Fedora 35, gcc 11.3.1, x64

   - Fedora 35, clang 13.0.1, x64

   - Fedora 39, gcc 13.2.1, x64

   - FreeBSD 14.0, clang 16.0.6, x64

   - Gentoo, gcc 11.2.0, x64

   - MacOS 10 (Catalina), clang 12.0.0, x64

   - MacOS 13.4.1 (Venture), clang 14.0.3, arm64

   - NetBSD 9.2, gcc 7.5.0, x64

   - SUSE 15.4, clang 13. 0.1, x64

   - SUSE 15.4, gcc 7.5.0, x64

   - Ubuntu 20.04, gcc 9.4.0, x64

   - Ubuntu 20.04, clang 10.0.0, x64

   - Ubuntu 20.04, tcc 0.9.27, x64

   - VAX/VMS 5.4-3, VAX C 3.2, VAX (simh)

   - OpenVMS 9.2-1, VSI C 7.4-726, x64

   - Windows 11 + WSL2, gcc 12.2.0, x64 + arm64

### Prerequisites

The following packages are required to build and/or run the simulator.

   - Debian : gcc | clang | tcc  make libc6-dev libx11-dev xfonts-base

   - Fedora : gcc | clang  make glibc-devel libX11-devel xorg-x11-fonts-base | xorg-x11-fonts-misc

   - Gentoo : gcc make libc6-dev libx11-dev font-misc-misc

   - MacOS  : clang make [xquartz](https://www.xquartz.org/)

   - SUSE   : gcc | clang make libX11-devel

   - Ubuntu : gcc make libc6-dev libx11-dev xfonts-base

   - Windows 11 + WSL2 : gcc make libc6-dev libx11-dev xfonts-base

When all the prerequisites are available, you should be able to [compile](#Compiling) the
program using make if it is in the list of tested platforms.

Note - If you are using a newer version of the operating system or compiler
then you may encounter some minor issues.

### Starting the simulator

If you install the simulator on most modern desktops it should create a new
menu entry that will start the launcher script by default. When invoked for
the first time this will prompt you to select the default simulator as well
as any additional command line options.  These selections will be saved and
used in future.

On some desktop environments it is also possible to right click on the menu
icon which will display a sub-menu that will allow you to select a specific
model directly as well as change the default model and options.

### Keyboard Shortcuts

The following keyboard shortcuts should work on Linux:

'0' - '9', '+'. '-'. '*'. '/' and 'Enter' should do what you expect them to
(when using numeric key pad you need to use numlock as usual).

'f' and where applicable 'g' and 'h' correspond to the shift keys.

'Esc' or 'Backspace' corresponds to 'Clx', 'c' to CHS, 'e' to 'EEX', and on
financial models 'n' and 'i' correspond to 'n' and 'i' if not shifted.

'A' - 'E' , 'A' - 'F' , or 'A' - 'B' correspond to program labels  (whether
shifted or not).

'Space' maps to 'SST' (if not shifted.

The following control keys can also be used.

'Ctrl-Z' Exists the simulator. For models  with  continuous memory 'Ctrl-Z'
saves the current memory contents.

'Ctrl-C' Resets the simulator to its initial state.

'Ctrl-T' Toggles tracing of the simulator code execution.

'Ctrl-S' Enable tracing and executes a single instruction.

'Ctrl-Q' Resumes execution (does not disable trace).

### Loading and saving

For  models with continuous memory the contents of program memory and  data
registers  are saved automatically when the calculator is switched  off  or
the  window is closed.  The current state of the simulator will be saved in
either `$HOME/.local/share/x11-calc/` or in a hidden file in the user's  HOME
directory if `$HOME/.local/` does not exist.

Resetting the simulator using 'Ctrl-C' will reload the saved state.
```
~/.x11-calc-nn.dat
```
When  starting the simulator the name of the data file used to restore  the
saved state can be specified on the command line allowing previously  saved
copies of programs to be loaded automatically when the simulator starts  or
the  simulator is reset using 'Ctrl-C'.  However, any changes will be
saved in the hidden data file.

### Exiting

For  models with a 'sliding' On/Off switch clicking on the switch will turn
the simulator on or off, but if when switching off you hold down the switch
down for two seconds the program will exit.

### Debugging

You  can  start the simulation in trace mode using '-t', or in single  step
mode using '-s', and set a breakpoint using '-b &lt;octal address&gt;'.

'Ctrl-T'  also toggles trace mode when running, 'Ctrl-S' executes the  next
instruction, 'Ctrl-Q' resumes execution, and 'Ctrl-R' displays the contents
of the CPU registers.

When in trace mode a jump to the same instruction produces no output.

### ROM Images

The '-r <filename>' command line option provides the ability to use the ROM
contents from a separate file.  The contents of the ROM are stored as pairs
values seperated by a colon containing the memory address and the opcode.

Anything appearing after a semi colon on each line is ignored.

For the HP10C, HP11C, HP12C, HP15C and HP16C the ROM file contains pairs of
_hexadecimal_ values.
```
0000:107
0001:04e
0002:270
0003:238
0004:2ee
0005:13f
```
For other models the ROM file contains pairs of _octal_ values.
```
00000:00255
00001:01420
00002:00451
00003:01456
00004:01746
00005:00472
```
When loading a ROM from file any gaps between the memory addresses will not
be filled with zeros, and the existing ROM contents will be left unchanged.

ROM files can therefore be used to load alternative version of the firmware
for a particular model or apply a patch to the existing firmware.

### Known Issues

- Keyboard shortcuts only work on Linux and NetBSD.
- A 24 bit colour display is required.
- For best results you need to have the X windows core fonts installed.

- Parallel make only works on Linux, NetBSD and FreeBSD.

##### HP 11C + HP 12C + HP 15C + HP 16C

- Keyboard test is successful but these models do not pass the self-test.

##### HP 29C

- All 30 registers have continuous memory.

##### HP 37E

- Fails self-test.

##### HP 67

- Cannot read or write to magnetic cards.
- Has  continuous memory (which allows saved programs to be loaded from the
command line).

### Raspberry Pi Specific Issues

- The code uses a simplified display on Arm based systems (except Apple) to
avoid the display refresh issues seen on the Raspberry Pi if either FKMS or
KMS overlays are enabled. (Note- Do not disable KMS on the latest Raspberry
Pi OS release).

### Wayland specific Issues

- Performance under Xwayland is generally poor, the emulator will work well
on a Raspberry Pi3 using Xwindows but a Raspberry Pi4 using Xwayland cannot
redraw the display quickly enough to allow background shading to be used to
draw the display digits. As a result the emulator's LED/LCD display doesn't
use any background shading on ARM based systems.
* The emulator window is supposed to be a fixed size.  However wnen running
on Xwayland the window manager does not handle this correctly.

### VMS Specific Issues

- Colour palette assumes a black and white display.
- Not all text is visible due to the limited colour palette and the DEC fonts
are missing some characters (for example the Pi symbol).


### Acknowledgements

There are almost certainly some names I've missed off this list but without
the help and encouragement from a multiple it is unlikely that this project
would have happened at all or that it would got as far as it has.

- Teenix for convincing me that it was possible in the first place and help
since (at least I think I glad he did).

- Teenix/Panamatik for their excellent simulators from which I was able to
figure out most of what should happen when each instruction is executed.

- Macmpi for completely rewriting the makefiles and packaging the simulator
for flatpak and for packaging and testing the simulators on Alpine Linux.

- Agarza/Martin HEPPERLE for translating the help text

- Thomas KLEMM for his testing and debugging on MacOS.

- Mark SHIN for testing on MacOS.

- Jonakeys for testing on FreeBSD.

### Problem Reports

If you find problems or have suggestions relating to these simulators, then
please create a new [issue](https://github.com/mike632t/x11-calc/issues).

Your problem report should contain:

- Architecture (and VM host if applicable);

- Operating System and version;

- Desktop Environment and version;

- Window Manager and version;

- Compiler and version used;

- Commit ID;

- A description of the problem.

Thank you.
