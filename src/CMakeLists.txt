add_executable(crash
  util/uniform_unit_random_source.cpp
  renderer/bitmap_renderer.cpp
  renderer/video_file_output.cpp
  renderer/renderer.cpp
  crash.cpp
)

find_library( AVCODEC avcodec )
find_library( AVUTIL avutil )
find_library( SWSCALE swscale )
find_library( CAIRO cairo )
target_link_libraries( crash "${AVCODEC}" "${AVUTIL}" "${SWSCALE}" "${CAIRO}")
