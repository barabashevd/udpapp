# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\barab\CLionProjects\udpapp

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\barab\CLionProjects\udpapp\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles\udpapp.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\udpapp.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\udpapp.dir\flags.make

CMakeFiles\udpapp.dir\main.cpp.obj: CMakeFiles\udpapp.dir\flags.make
CMakeFiles\udpapp.dir\main.cpp.obj: ..\main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\barab\CLionProjects\udpapp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/udpapp.dir/main.cpp.obj"
	C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\udpapp.dir\main.cpp.obj /FdCMakeFiles\udpapp.dir\ /FS -c C:\Users\barab\CLionProjects\udpapp\main.cpp
<<

CMakeFiles\udpapp.dir\main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/udpapp.dir/main.cpp.i"
	C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\udpapp.dir\main.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\barab\CLionProjects\udpapp\main.cpp
<<

CMakeFiles\udpapp.dir\main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/udpapp.dir/main.cpp.s"
	C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\udpapp.dir\main.cpp.s /c C:\Users\barab\CLionProjects\udpapp\main.cpp
<<

CMakeFiles\udpapp.dir\stdafx.cpp.obj: CMakeFiles\udpapp.dir\flags.make
CMakeFiles\udpapp.dir\stdafx.cpp.obj: ..\stdafx.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\barab\CLionProjects\udpapp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/udpapp.dir/stdafx.cpp.obj"
	C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\udpapp.dir\stdafx.cpp.obj /FdCMakeFiles\udpapp.dir\ /FS -c C:\Users\barab\CLionProjects\udpapp\stdafx.cpp
<<

CMakeFiles\udpapp.dir\stdafx.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/udpapp.dir/stdafx.cpp.i"
	C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe > CMakeFiles\udpapp.dir\stdafx.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\barab\CLionProjects\udpapp\stdafx.cpp
<<

CMakeFiles\udpapp.dir\stdafx.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/udpapp.dir/stdafx.cpp.s"
	C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\udpapp.dir\stdafx.cpp.s /c C:\Users\barab\CLionProjects\udpapp\stdafx.cpp
<<

# Object files for target udpapp
udpapp_OBJECTS = \
"CMakeFiles\udpapp.dir\main.cpp.obj" \
"CMakeFiles\udpapp.dir\stdafx.cpp.obj"

# External object files for target udpapp
udpapp_EXTERNAL_OBJECTS =

udpapp.exe: CMakeFiles\udpapp.dir\main.cpp.obj
udpapp.exe: CMakeFiles\udpapp.dir\stdafx.cpp.obj
udpapp.exe: CMakeFiles\udpapp.dir\build.make
udpapp.exe: CMakeFiles\udpapp.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\barab\CLionProjects\udpapp\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable udpapp.exe"
	"C:\Program Files\JetBrains\CLion 2020.3.2\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\udpapp.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100183~1.0\x86\mt.exe --manifests  -- C:\PROGRA~2\MICROS~2\2019\PROFES~1\VC\Tools\MSVC\1428~1.293\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\udpapp.dir\objects1.rsp @<<
 /out:udpapp.exe /implib:udpapp.lib /pdb:C:\Users\barab\CLionProjects\udpapp\cmake-build-debug\udpapp.pdb /version:0.0  /machine:X86 /debug /INCREMENTAL /subsystem:console  kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\udpapp.dir\build: udpapp.exe

.PHONY : CMakeFiles\udpapp.dir\build

CMakeFiles\udpapp.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\udpapp.dir\cmake_clean.cmake
.PHONY : CMakeFiles\udpapp.dir\clean

CMakeFiles\udpapp.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\Users\barab\CLionProjects\udpapp C:\Users\barab\CLionProjects\udpapp C:\Users\barab\CLionProjects\udpapp\cmake-build-debug C:\Users\barab\CLionProjects\udpapp\cmake-build-debug C:\Users\barab\CLionProjects\udpapp\cmake-build-debug\CMakeFiles\udpapp.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\udpapp.dir\depend

