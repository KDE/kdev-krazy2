# Find the krazy2 executable
#
# Defines the following variables
#  Krazy2_EXECUTABLE - path of the krazy2 executable

#=============================================================================
# Copyright 2017 Friedrich W. H. Kossebau <kossebau@kde.org>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================

find_program(Krazy2_EXECUTABLE NAMES krazy2)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Krazy2 DEFAULT_MSG Cppcheck_EXECUTABLE)

mark_as_advanced(Krazy2_EXECUTABLE)
