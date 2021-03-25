// PC-Lint GUI
// Copyright (C) 2021  Ayymooose

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

// TODO: Need more macros defined to support other platforms

#if _WIN64
    // 64 bit build
    #define BUILD_ARCHITECTURE "(64-bit)"
#else
    #define BUILD_ARCHITECTURE "(32-bit)"
#endif


#ifdef __GNUC__
    #define COMPILER_NAME "g++"
    #define COMPILER_VERSION __VERSION__
#else
    #define COMPILER_NAME "Unknown compiler"
    #define COMPILER_VERSION "Unknown compiler version"
#endif
