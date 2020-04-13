add_executable(crash
  crash.cpp
  bitmap_renderer.cpp
  video_file_output.cpp
  renderer.cpp
)

find_library( AVCODEC avcodec )
find_library( AVUTIL avutil )
find_library( SWSCALE swscale )
find_library( CAIRO cairo )
target_link_libraries( crash "${AVCODEC}" "${AVUTIL}" "${SWSCALE}" "${CAIRO}")
