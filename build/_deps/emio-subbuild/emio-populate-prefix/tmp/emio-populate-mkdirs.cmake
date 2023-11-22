# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-src"
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-build"
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix"
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix/tmp"
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix/src/emio-populate-stamp"
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix/src"
  "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix/src/emio-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix/src/emio-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/gumpa/PCB Design Projects/OpenDAS/Firmware/build/_deps/emio-subbuild/emio-populate-prefix/src/emio-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
