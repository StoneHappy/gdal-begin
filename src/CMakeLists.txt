find_package(GDAL REQUIRED)

########################## QT ######################################
find_package(Qt5 COMPONENTS Widgets OpenGL)

find_package(qtadvanceddocking CONFIG REQUIRED)

add_subdirectory(Vendors)
add_subdirectory(Runtime)
add_subdirectory(Tutorials)