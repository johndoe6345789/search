## @brief nextra-search — full-text search + indexer.
cmake_minimum_required(VERSION 3.20)
project(nextra-search VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/service.cmake"
)

nextra_service(
    NAME nextra-search
    DIRS
        search/backend
        search/backend/commands
        search/controllers
        search/events
        elasticsearch/backend
        alerts/backend
)
