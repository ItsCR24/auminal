#include <SFML/System/Time.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_options.hpp>
#include <ftxui/screen/color.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fileref.h>
#include <tpropertymap.h>
#include <tstringlist.h>
#include <ftxui/component/screen_interactive.hpp>
#include "ftxui/dom/elements.hpp"
#include "minimalButton.h"
#include "Song.h"

namespace fs = std::filesystem;
using namespace ftxui;
//Icons: ♫         

struct ListEntry {
	std::wstring path;
	std::string listTitle;
};

std::filesystem::path getConfigPath() {
#ifdef _WIN32
	const char* localAppData = std::getenv("LOCALAPPDATA");
	std::filesystem::path path = localAppData ? localAppData : std::filesystem::current_path();
	path /= "CR24";
	path /= "auminal";
#else
	const char* home = std::getenv("HOME");
	std::filesystem::path path = home ? home : std::filesystem::current_path();
	path /= ".config";
	path /= "auminal";
#endif
	std::filesystem::create_directories(path);
	return path;
}

void playButtonCurrent(bool& playing, std::string& playIcon, std::vector<std::unique_ptr<Song>>& songs, int& currentlyPlaying, sf::Clock& pd) {

	// No audio playing
	if (currentlyPlaying == -1) return;

	// Audio playing
	Song* s = songs[currentlyPlaying].get();
	auto status = s->getStatus();

	// Pause
	if (status == sf::SoundSource::Status::Playing) {
		s->pause();
		pd.stop();
		playing = false;
		playIcon = "";
	}
	// Unpause
	else {
		s->play();
		pd.start();
		playing = true;
		playIcon = "";
	}
}

void playSelected(int index, bool& playing, std::string& playIcon, std::vector<std::unique_ptr<Song>>& songs, int& currentlyPlaying, sf::Clock& pd) {
	Song* s = songs[index].get();
	auto status = s->getStatus();

	// SAME song clicked
	if (currentlyPlaying == index)
	{
		if (status == sf::SoundSource::Status::Playing) {
			pd.stop();
			s->pause();
			playing = false;
			playIcon = "";
		}
		else {
			s->play();
			pd.start();
			playing = true;
			playIcon = "";
		}
		return;
	}

	// DIFFERENT song clicked
	if (currentlyPlaying != -1)
		songs[currentlyPlaying]->stop();

	currentlyPlaying = index;
	pd.restart();
	s->play();
	playing = true;
	playIcon = "";
}

float sliderMax(int currentlyPlaying, std::vector<std::unique_ptr<Song>>& songs) {
	if (currentlyPlaying == -1)
		return 0;

	Song* s = songs[currentlyPlaying].get();
	auto status = s->getStatus();

	if (status == sf::SoundSource::Status::Playing) return songs[currentlyPlaying]->getDuration();
	else return 0;
}

Element showDetail(int opt, int& selected, std::vector<std::unique_ptr<Song>>& songs) {

	Song* s = songs[selected].get();
	std::string output;

	if (opt == 0 && !s->getTitle().empty()) {
		output = s->getTitle();
	}
	if (opt == 1 && !s->getArtist().empty()) {
		output = s->getArtist();
	}
	if (opt == 2 && !s->getAlbum().empty() && s->getAlbum() != s->getTitle()) {
		output = s->getAlbum();
	}
	if (opt == 3 && !s->getAlbum().empty() && s->getAlbum() != s->getTitle()) {
		output = "Track: " + std::to_string(s->getTrack());
	}
	if (opt == 4 && s->getDisc() != 0 && s->getAlbum() != s->getTitle()) {
		output = "Disc: " + std::to_string(s->getDisc());
	}
	if (opt == 5 && s->getYear() != 0) {
		output = "Year: " + std::to_string(s->getYear());
	}
	if (opt == 6 && !s->getGenre().empty()) {
		output = s->getGenre();
	}
	if (opt == 7 && !s->getComment().empty()) {
		output = s->getComment();
	}
	if (!output.empty()) {
		return text(output);
	}
	else {
		return emptyElement();
	}
}

Element nowPlaying(int& currentlyPlaying, std::vector<std::unique_ptr<Song>>& songs, int option) {
	if (option == 1 && currentlyPlaying != -1) {
		return text(songs[currentlyPlaying]->getTitle());
	}
	else if (option == 2 && currentlyPlaying != -1) {
		return text(songs[currentlyPlaying]->getArtist()) | dim;
	}
	else if (option == 2 && currentlyPlaying == -1) {
		return text(" Nothing playing");
	}
	return text("");
}

sf::Time elapsed(sf::Clock songPlaybackDuration, int currentlyPlaying) {
	if (currentlyPlaying != -1) {
		return songPlaybackDuration.getElapsedTime();
	}
	else {
		return sf::seconds(0.f);
	}
}

float currentSongDuration(int currentlyPlaying, std::vector<std::unique_ptr<Song>>& songs) {
	if (currentlyPlaying != -1) {
		return songs[currentlyPlaying]->getDuration();
	}
	else {
		return 0;
	}
}

int main(int argc, char *argv[]) {

	ScreenInteractive screen = ScreenInteractive::Fullscreen();
	int selected = 0;
	int currentlyPlaying = -1;
	bool playing = false;
	int playerSize = 4;
	int spinner_index = 0;
	int fileCount = 0;
	std::string reset_position;
	std::string playIconDetails = "";
	std::string playIcon = "";

	sf::Clock songPlaybackDuration;
	std::FILE* configFile;
	std::filesystem::path configPath = getConfigPath() / "config.ini";

	std::string path;
	std::vector<ListEntry> files;
	std::vector<std::string> listTitles;
	std::vector<std::unique_ptr<Song>> songs;

	if (argc > 1) {
		if (std::string(argv[1]) == "--directory" || std::string(argv[1]) == "-d") {
			if (argc > 2) {
				if (fs::exists(argv[2]) && fs::is_directory(argv[2])) {
					path = argv[2];
				}
				else {
					std::cerr << "The provided path is not a valid directory." << std::endl;
					return 1;
				}
			}
			else {
				std::cout << "Please provide a directory path after --directory or -d" << std::endl;
				return 1;
			}
		}
		if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
			std::cout << "auminal - Terminal audio player with responsive ui\n\n";
			std::cout << "Usage: auminal [option]\n";
			std::cout << "Options:\n";
			std::cout << "\t--directory, -d <path> \tSpecify the audio library directory path\n";
			std::cout << "\t--help, -h \t\tShow this help message\n";
			std::cout << "\nConfig file is stored at:\n";
			std::cout << configPath << "\n";
			return 1;
		}
		else {
			std::cout << "Unknown option. Use --help or -h for usage information." << std::endl;
			return 1;
		}
	}

	std::string tempPath;

	if (fopen_s(&configFile, configPath.string().c_str(), "r") != NULL) {
		// Create default config
		std::cout << "\nWelcome to auminal! :)\n";
		std::cout << "Do you want to specify a custom audio library directory? (Y/n): ";
		std::string response;
		std::getline(std::cin, response);
		if (response == "Y" || response == "y" || response.empty()) {
			std::cout << "Please enter the full path to your audio library directory: ";
			std::getline(std::cin, tempPath);
			while (!fs::exists(tempPath) && !fs::is_directory(tempPath)) {
				std::cout << "The provided path is not a valid directory. Please enter a valid path: ";
				std::getline(std::cin, tempPath);
			}

			if ((fopen_s(&configFile, configPath.string().c_str(), "w")) != NULL) {
				std::cerr << "Error: Could not open file 'config.ini' for writing.\n";
				return -1;
			}

			// Save to config file and set path
			fprintf(configFile, "PATH=%s\n", tempPath.c_str());
			path = tempPath;
			fclose(configFile);
		}
		else {
			// Enter temporary path for this session
			std::cout << "Enter a temporary path for this session: ";
			std::getline(std::cin, tempPath);
			while (!fs::exists(tempPath) && !fs::is_directory(tempPath)) {
				std::cout << "The provided path is not a valid directory. Please enter a valid path: ";
				std::getline(std::cin, tempPath);
			}
			path = tempPath;
		}
	}
	else {
		char buffer[260];
		fscanf_s(configFile, "PATH=%s\n", &buffer, 260);
		tempPath = buffer;
		fclose(configFile);

		if (!fs::exists(tempPath) || !fs::is_directory(tempPath)) {
			std::cerr << "The path specified in 'config.ini' is not a valid directory.\n";
			std::cout << "Do you want to specify a different audio library directory? (y/N): ";
			std::string response;
			std::getline(std::cin, response);
			if (response == "Y" || response == "y") {
				std::cout << "Please enter the full path to your audio library directory: ";
				std::getline(std::cin, tempPath);
				while (!fs::exists(tempPath) && !fs::is_directory(tempPath)) {
					std::cout << "The provided path is not a valid directory. Please enter a valid path: ";
					std::getline(std::cin, tempPath);
				}

				// Update config file
				if ((fopen_s(&configFile, configPath.string().c_str(), "w")) != 0) {
					std::cerr << "Error: Could not open file 'config.ini' for writing.\n";
					return -1;
				}
				fprintf(configFile, "PATH=%s\n", tempPath.c_str());
				path = tempPath;
				fclose(configFile);
			}
			else {
				std::cout << "\nExiting auminal. Please fix the 'config.ini' file to point to a valid directory.\n";
				std::cout << "Your config is located at: " << configPath << "\n";
				return -1;
			}
		}
		else {
			path = tempPath;
		}
	}

	// Load library
	for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
		auto ext = entry.path().extension().string();
		if (ext == ".flac" || ext == ".mp3" || ext == ".ogg" || ext == ".oga" ||
			ext == ".ogx" || ext == ".opus" || ext == ".wav" || ext == ".wave") {

			std::wstring fp = entry.path().wstring();
			TagLib::FileRef file(fp.c_str());
			TagLib::PropertyMap props = file.properties();
			sf::Music audio;
			auto songData = std::make_unique<Song>();

			std::string itemTitle = "Unknown - Unknown";
			if (!file.isNull() && file.tag()) {
				itemTitle = file.tag()->artist().to8Bit(true) + " - " + file.tag()->title().to8Bit(true);
				songData->setFilePath(fp);
				songData->setTitle(file.tag()->title().to8Bit(true));
				songData->setArtist(file.tag()->artist().to8Bit(true));
				songData->setAlbum(file.tag()->album().to8Bit(true));
				songData->setGenre(file.tag()->genre().to8Bit(true));
				songData->setComment(file.tag()->comment().to8Bit(true));
				songData->setTrack(file.tag()->track());
				songData->setYear(file.tag()->year());
				songData->setDisc(props["DISCNUMBER"].toString().toInt());
				songData->setLyrics(props["LYRICS"].toString().to8Bit(true));
			}

			files.push_back({ fp, itemTitle });
			listTitles.push_back(itemTitle);
			songs.push_back(std::move(songData));
			fileCount++;
		}
		auto loadingOutput = vbox({
			text("  auminal") | color(Color::Green),
			hbox({
				text("  Loading library "),
				spinner(15, spinner_index++)
			}),
			text("  Processed files: " + std::to_string(fileCount))
		});

		auto screen = Screen::Create(Dimension::Full(), Dimension::Fit(loadingOutput));
		Render(screen, loadingOutput);
		std::cout << reset_position;
		screen.Print();
		reset_position = screen.ResetPosition();
	}

	// If library empty
	if (songs.empty()) {
		std::cout << "\nNo audio files found in the specified directory: " << path << "\n";
		std::cout << "Supported formats: .flac, .mp3, .ogg, .oga, .ogx, .opus, .wav, .wave\n";
		return 1;
	}

	// Components
	Component menu = Menu(&listTitles, &selected);

	Component songsList = Renderer(menu, [&] {
		return vbox({
			text("auminal") | bold | center,
			separator(),
			menu->Render() | yframe,
		}) | border | xflex_grow | yflex_grow;
	});

	Component playButtonDetails = minimalButton(playIconDetails, [&]() { playSelected(selected, playing, playIcon, songs, currentlyPlaying, songPlaybackDuration); });

	Component preview = Renderer(playButtonDetails, [&] {
		if (songs[selected].get()->getStatus() == sf::SoundSource::Status::Playing) {
			playIconDetails = "";
		}
		else {
			playIconDetails = "";
		}

		Element playElement = playButtonDetails->Render();

		return vbox({
			text(listTitles[selected]) | center,
			separator(),
			filler(),
			vbox({
				showDetail(0, selected, songs) | center,
				showDetail(1, selected, songs) | center,
				showDetail(2, selected, songs) | center,
				separatorEmpty(),
				showDetail(3, selected, songs) | center,
				showDetail(4, selected, songs) | center,
				showDetail(5, selected, songs) | center,
				showDetail(6, selected, songs) | center,
				separatorEmpty(),
				showDetail(7, selected, songs) | center,
				text(files[selected].path) | center,
				separatorEmpty(),
				playElement | center,
			}) | vcenter | center,
			filler()
		}) | xflex_grow | yflex_grow | border;
	});

	ResizableSplitOption leftSplitOpts;

	int left_size = Terminal::Size().dimx * 25 / 100;
	leftSplitOpts.main = songsList;
	leftSplitOpts.back = preview;
	leftSplitOpts.main_size = &left_size;
	leftSplitOpts.direction = Direction::Left;
	leftSplitOpts.separator_func = [] { return separatorEmpty(); };


	float progress = 0.f;
	float currentSongPlaybackDuration = elapsed(songPlaybackDuration, currentlyPlaying).asSeconds();
	float songDuration = currentSongDuration(currentlyPlaying, songs);

	Component splits = ResizableSplit(leftSplitOpts);

	Component playButton = minimalButton(playIcon, [&]() { playButtonCurrent(playing, playIcon, songs, currentlyPlaying, songPlaybackDuration); });

	Component player = Renderer(playButton, [&] {
		currentSongPlaybackDuration = elapsed(songPlaybackDuration, currentlyPlaying).asSeconds();
		songDuration = currentSongDuration(currentlyPlaying, songs);
		
		progress = currentSongPlaybackDuration / songDuration;

		if (currentSongPlaybackDuration > songDuration && songDuration != 0) {
			// Song ended
			songPlaybackDuration.restart();
			songPlaybackDuration.stop();
		}

		return vbox({
			nowPlaying(currentlyPlaying, songs, 1) | size(WIDTH, EQUAL, Terminal::Size().dimx * 30/100),
			dbox({
				hbox({
					nowPlaying(currentlyPlaying, songs, 2) | size(WIDTH, EQUAL, Terminal::Size().dimx * 30/100),
				}),
				playButton->Render() | center,
			}),
			text(""),
			hbox({
				text(std::to_string(static_cast<int>(currentSongPlaybackDuration)) + " ") | vcenter,
				gauge(progress) | bgcolor(Color::SpringGreen4) | color(Color::CadetBlue),
				text(" " + std::to_string(static_cast<int>(songDuration))) | vcenter
			})
		});
	});

	ResizableSplitOption btmSplitOpts;
	btmSplitOpts.main = player;
	btmSplitOpts.back = splits;
	btmSplitOpts.main_size = &playerSize;
	btmSplitOpts.direction = Direction::Down;
	btmSplitOpts.separator_func = [] { return separatorEmpty(); };

	// Main layout
	Component layout = ResizableSplit(btmSplitOpts);

	// Main renderer
	Component main_renderer = Renderer(layout, [&] {
		int term_width = Terminal::Size().dimx;
		playerSize = 4;

		if (left_size > term_width * 40 / 100) // 40% max
			left_size = term_width * 40 / 100;
		return layout->Render();
	});

	std::thread([&] {
		while (true) {
			std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 5 FPS
			screen.PostEvent(Event::Custom); // triggers a redraw
		}
	}).detach();

	screen.Loop(main_renderer);
}
