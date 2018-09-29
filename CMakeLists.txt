cmake_minimum_required(VERSION 3.2)
project(jpeg2rb C)


# source files
set(JPEG2RGB_SOURCE_FILES
        jpeg2rgb.c
        )

set(RGB2JPEG_SOURCE_FILES
        rgb2jpeg.c
        )

set(JPEGSCRAMBLE_SOURCE_FILES
        jpegscramble.c
        )

set(PNG2RGB_SOURCE_FILES
        png2rgb.c
        )

set(RGB2PNG_SOURCE_FILES
    rgb2png.c
    )

set(PNGSCRAMBLE_SOURCE_FILES
    pngscramble.c
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

add_executable(jpegscramble ${JPEGSCRAMBLE_SOURCE_FILES})
target_link_libraries(jpegscramble ${JPEG_LIB})
install(TARGETS jpegscramble DESTINATION bin)

add_executable(png2rgb ${PNG2RGB_SOURCE_FILES})
target_link_libraries(png2rgb ${PNG_LIB})
install(TARGETS png2rgb DESTINATION bin)

add_executable(rgb2png ${RGB2PNG_SOURCE_FILES})
target_link_libraries(rgb2png ${PNG_LIB})
install(TARGETS rgb2png DESTINATION bin)

add_executable(pngscramble ${PNGSCRAMBLE_SOURCE_FILES})
target_link_libraries(pngscramble ${PNG_LIB})
install(TARGETS pngscramble DESTINATION bin)



