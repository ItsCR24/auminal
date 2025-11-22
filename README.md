# auminal
#### 🎶 Terminal audio player with responsive UI

## Features
- Play audio files from a specified directory
- Responsive terminal user interface
- Supports common audio formats
- Displays audio metadata (title, artist, album, duration, album details, etc.)
- Mouse support for navigation and control
- Resizable split view for library browsing and audio file preview/information
- Preferences get saved in a config file

## Usage
```bash
Usage: auminal [option]
Options:
        --directory, -d <path>  Specify the audio library path
        --help, -h              Show this help message
		--version, -v           Show version number
```

## Supported Formats
- FLAC
- MP3
- OGG (including OGA, OGX, OPUS)
- WAV

## Libraries Used
- [SFML Audio](https://www.sfml-dev.org/) for audio playback
- [taglib](https://taglib.org/) for audio metadata extraction
- [FTXUI](https://github.com/ArthurSonzogni/FTXUI) for terminal user interface

---

## Download
You can download the latest release from the [releases page](https://github.com/ItsCR24/auminal/releases).
