cmake_minimum_required(VERSION 3.2)
project(jpeg2rb C)


# source files
set(JPEG2RGB_SOURCE_FILES
        jpeg2rgb.c
        )

set(RGB2JPEG_SOURCE_FILES
        rgb2jpeg.c
        )

set(PNG2RGB_SOURCE_FILES
        png2rgb.c
        )

set(RGB2PNG_SOURCE_FILES
    rgb2png.c
    )

set(PNG2PNG_SOURCE_FILES
    png2png.c
    )




find_library(JPEG_LIB NAMES libjpeg jpeg)
find_library(PNG_LIB NAMES libpng png)

include_directories(/usr/include /usr/local/include /opt/local/include)

add_executable(jpeg2rgb ${JPEG2RGB_SOURCE_FILES})
target_link_libraries(jpeg2rgb ${JPEG_LIB})
install(TARGETS jpeg2rgb DESTINATION bin)

add_executable(rgb2jpeg ${RGB2JPEG_SOURCE_FILES})
target_link_libraries(rgb2jpeg ${JPEG_LIB})
install(TARGETS rgb2jpeg DESTINATION bin)

add_executable(png2rgb ${PNG2RGB_SOURCE_FILES})
target_link_libraries(png2rgb ${PNG_LIB})
install(TARGETS png2rgb DESTINATION bin)

add_executable(rgb2png ${RGB2PNG_SOURCE_FILES})
target_link_libraries(rgb2png ${PNG_LIB})
install(TARGETS rgb2png DESTINATION bin)

add_executable(png2png ${PNG2PNG_SOURCE_FILES})
target_link_libraries(png2png ${PNG_LIB})
install(TARGETS png2png DESTINATION bin)



