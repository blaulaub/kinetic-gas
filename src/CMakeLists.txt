add_executable(crash
  crash.cpp
)

find_library( AVCODEC avcodec )
find_library( AVUTIL avutil )
find_library( SWSCALE swscale )
target_link_libraries( crash "${AVCODEC}" "${AVUTIL}" "${SWSCALE}")
