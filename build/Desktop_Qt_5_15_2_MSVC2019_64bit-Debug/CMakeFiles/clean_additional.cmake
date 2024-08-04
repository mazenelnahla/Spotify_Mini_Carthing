# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\spotifyMini_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\spotifyMini_autogen.dir\\ParseCache.txt"
  "spotifyMini_autogen"
  )
endif()
