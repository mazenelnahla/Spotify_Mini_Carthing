# Spotify Mini Carthing

![Spotify Mini Carthing](https://github.com/mazenelnahla/Spotify_Mini_Carthing/blob/main/screenshots/2.png)

A minimal Spotify media control application designed for car instrument clusters, developed using Qt and C++. This project enables you to control Spotify playback, view track information, and manage Spotify authentication directly from your car's interface.

## Features

- **Spotify Playback Control:** Play, pause, skip, and rewind tracks directly from the car interface.
- **Track Information Display:** View the currently playing track, artist, and album information.
- **Dynamic Authentication:** Log in to Spotify using a mobile device with automatic token handling.
- **Responsive UI:** Designed to be responsive and lightweight, perfect for embedded systems like car clusters.
- **Support for Arabic Text:** Properly handles and displays Arabic song titles and artist names.

## Screenshots

![Spotify Mini Carthing](https://github.com/mazenelnahla/Spotify_Mini_Carthing/blob/main/screenshots/2.png)
![Spotify Mini Carthing](https://github.com/mazenelnahla/Spotify_Mini_Carthing/blob/main/screenshots/1.png)



## Installation

### Prerequisites

- Qt 5.15 or later
- C++17 or later
- Spotify Developer Account for API access
- Raspberry Pi or any other embedded system (optional for car installation)

### Building the Project

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/mazenelnahla/spotify-mini-carthing.git
   git clone https://github.com/yourusername/spotify-mini-carthing.git
   cd spotify-mini-carthing
   ```

2. **Configure the Project:**
   ```bash
   qmake
   ```

3. **Build the Project:**
   ```bash
   make
   ```

4. **Run the Application:**
   ```bash
   ./spotify-mini-carthing
   ```

### Running on Raspberry Pi

For deployment on a Raspberry Pi or similar device:

1. **Cross-compile the project** for ARM if developing on a different architecture.
2. **Transfer the executable** to your Raspberry Pi.
3. **Execute the application** on the device:
   ```bash
   ./spotify-mini-carthing
   ```

## Usage

1. **Connect your device** to a network with internet access.
2. **Launch the application** on your car's instrument cluster or embedded system.
3. **Authenticate with Spotify** using your mobile device by scanning the displayed QR code or entering the 6-digit pairing code.
4. **Control your music** using the interface.

## Configuration

You can configure the application by editing the `config.json` file located in the build directory. This file includes settings for:

- **Spotify API credentials**
- **Display options**
- **Authentication timeout**

## Contributing

If you'd like to contribute to this project, feel free to fork the repository and submit a pull request. Please ensure your code follows the project's coding standards and includes proper documentation.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Thanks to the Qt community for the fantastic framework.
- Spotify for providing a robust API to work with.
