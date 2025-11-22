#pragma once
#include <SFML/Audio/Music.hpp>
#include <SFML/System/Time.hpp>
#include <string>
#include <SFML/Audio.hpp>

class Song {
public:
	Song();
	void play();
	void pause();
	void stop();
	sf::Music::Status getStatus();
	void setVolume(float volume);
	void setLoop(bool loop);
	void setPlayingOffset(sf::Time offset);
	void setFilePath(const std::wstring& path);
	void setTitle(const std::string& t);
	std::string getTitle() const;
	void setArtist(const std::string& a);
	std::string getArtist() const;
	void setAlbum(const std::string& a);
	std::string getAlbum() const;
	void setGenre(const std::string& g);
	std::string getGenre() const;
	void setLyrics(const std::string& l);
	std::string getLyrics() const;
	void setComment(const std::string& c);
	std::string getComment() const;
	void setTrack(int t);
	int getTrack() const;
	void setDisc(int d);
	int getDisc() const;
	void setYear(int y);
	int getYear() const;
	float getDuration() const;
	~Song();

private:
	bool playing;
	std::wstring filePath;
	sf::Music audio;
	std::string title;
	std::string artist;
	std::string album;
	std::string genre;
	std::string lyrics;
	std::string comment;
	int track;
	int disc;
	int year;
	float duration;
	float progress;
};
