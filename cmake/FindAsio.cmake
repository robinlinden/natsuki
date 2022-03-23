# Asio_FOUND
# Asio::Asio

find_path(Asio_INCLUDE_DIR NAMES asio.hpp)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Asio
    DEFAULT_MSG
    Asio_INCLUDE_DIR
)

mark_as_advanced(Asio_INCLUDE_DIR)

if(Asio_FOUND)
    find_package(Threads QUIET REQUIRED)

    add_library(Asio::Asio INTERFACE IMPORTED)
    target_include_directories(Asio::Asio INTERFACE ${Asio_INCLUDE_DIR})
    target_compile_definitions(Asio::Asio INTERFACE ASIO_STANDALONE)
    target_link_libraries(Asio::Asio INTERFACE Threads::Threads)
endif()
