cmake_minimum_required(VERSION 3.2)


# source files
set(JPEG2RGB_SOURCE_FILES
        jpeg2rgb.c
        )

set(RGB2JPEG_SOURCE_FILES
        rgb2jpeg.c
        )

find_library(JPEG_LIB NAMES libjpeg jpeg)

include_directories(/usr/include /usr/local/include /opt/local/include)


add_executable(jpeg2rgb ${JPEG2RGB_SOURCE_FILES})
target_link_libraries(jpeg2rgb ${JPEG_LIB})
install(TARGETS jpeg2rgb DESTINATION bin)

add_executable(rgb2jpeg ${RGB2JPEG_SOURCE_FILES})
target_link_libraries(rgb2jpeg ${JPEG_LIB})
install(TARGETS rgb2jpeg DESTINATION bin)


