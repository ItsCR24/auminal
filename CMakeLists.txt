
cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

include(FetchContent)
FetchContent_Declare(
	ftxui
	GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
	GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

find_package(taglib CONFIG REQUIRED)

add_executable(
	auminal
	auminal.cpp
	minimalButton.cpp
	Song.cpp
)

target_compile_features(auminal PRIVATE cxx_std_20)
target_link_libraries(auminal
	PRIVATE ftxui::screen
	PRIVATE ftxui::dom
	PRIVATE ftxui::component
	PRIVATE SFML::Audio
	TagLib::tag
)
