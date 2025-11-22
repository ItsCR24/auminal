#include "Song.h"
#include <SFML/System/Time.hpp>
#include <sys/stat.h>

Song::Song() {
	title = "Unknown";
	artist = "Unknown";
	album = "Unknown";
	genre = "Unknown";
	lyrics = "Unknown";
	comment = "Unknown";
	track = 0;
	disc = 0;
	year = 0;
	duration = 0;
	progress = 0.f;
}

void Song::play() {
	// Load only ONCE
	if (audio.getStatus() == sf::SoundSource::Status::Stopped && duration == 0) {
		if (!audio.openFromFile(filePath))
			return;

		duration = audio.getDuration().asSeconds();
	}

	audio.play();
}

void Song::pause() {
	audio.pause();
}

void Song::stop() {
	audio.stop();
}

sf::Music::Status Song::getStatus() {
	return audio.getStatus();
}

void Song::setVolume(float volume) {
	audio.setVolume(volume);
}

void Song::setLoop(bool loop) {
	audio.setLooping(loop);
}

void Song::setPlayingOffset(sf::Time offset) {
	audio.setPlayingOffset(offset);
}

void Song::setFilePath(const std::wstring& path) {
	filePath = path;
}

void Song::setTitle(const std::string& t) {
	title = t;
}

std::string Song::getTitle() const {
	return title;
}

void Song::setArtist(const std::string& a) {
	artist = a;
}

std::string Song::getArtist() const {
	return artist;
}

void Song::setAlbum(const std::string& a) {
	album = a;
}

std::string Song::getAlbum() const {
	return album;
}

void Song::setGenre(const std::string& g) {
	genre = g;
}

std::string Song::getGenre() const {
	return genre;
}

void Song::setLyrics(const std::string& l) {
	lyrics = l;
}

std::string Song::getLyrics() const {
	return lyrics;
}

void Song::setComment(const std::string& c) {
	comment = c;
}

std::string Song::getComment() const {
	return comment;
}

void Song::setTrack(int t) {
	track = t;
}

int Song::getTrack() const {
	return track;
}

void Song::setDisc(int d) {
	disc = d;
}

int Song::getDisc() const {
	return disc;
}

void Song::setYear(int y) {
	year = y;
}

int Song::getYear() const {
	return year;
}

float Song::getDuration() const {
	return duration;
}

Song::~Song() {
	if (playing) {
		audio.stop();
	}
}
