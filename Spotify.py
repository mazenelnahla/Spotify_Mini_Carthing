import socket
import spotipy
import json
from spotipy.oauth2 import SpotifyOAuth
import time

# Spotify authentication
client_id = '3fb73f276ec048b78eff8151cee5563c'
client_secret = 'e8d229b5cc704e4d9c29bbd62957f93d'
redirect_uri = 'http://localhost:8888'
scope = "user-read-currently-playing user-modify-playback-state"

# UDP server address and port
udp_host = '127.0.0.1'  # Change to your UDP server's address
udp_port = 12356        # Change to your UDP server's port number

# Function to update the currently playing track information
def update_current_track():
    while True:
        try:
            sp = spotipy.SpotifyOAuth(client_id=client_id,
                                      client_secret=client_secret,
                                      redirect_uri=redirect_uri,
                                      scope=scope)
            token_dict = sp.get_access_token()
            token = token_dict['access_token']

            spotify_object = spotipy.Spotify(auth=token)
            current_track = spotify_object.current_user_playing_track()

            if current_track is not None:
                track_info = current_track['item']
                duration_ms = track_info['duration_ms']
                duration_sec = duration_ms / 1000
                progress_ms = current_track['progress_ms']
                progress_sec = progress_ms / 1000
                json_data = {
                    "trackName": current_track['item']['name'],
                    "artistName": current_track['item']['artists'][0]['name'],
                    "albumName": current_track['item']['album']['name'],
                    "albumURL": current_track['item']['album']['images'][0]['url'],
                    "isPlaying": current_track['is_playing'],
                    "currentTime": progress_sec,
                    "duration": duration_sec
                }
            else:
                json_data = {
                    "trackName": "",
                    "artistName": "",
                    "albumName": "",
                    "albumURL": "",
                    "isPlaying": False,
                    "currentTime": 0,
                    "duration": 0
                }
            send_song_data(json_data)

        except (socket.error, spotipy.SpotifyException) as e:
            # Handle specific exceptions (like socket errors or Spotify API errors)
            print(f"Error occurred: {str(e)}")
            # Wait before retrying
            time.sleep(10)
        except Exception as e:
            # Handle unexpected exceptions
            print(f"Unexpected error occurred: {str(e)}")
            # Wait before retrying
            time.sleep(10)


# Function to send song data over UDP
def send_song_data(json_data):
    try:
        # Create a UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        json_bytes = json.dumps(json_data, ensure_ascii=False).encode('utf-8')
        sock.sendto(json_bytes, (udp_host, udp_port))
        # Close the socket
        sock.close()
    except socket.error as e:
        print(f"Socket error occurred while sending data: {str(e)}")

# Function to handle play, pause, forward, and reverse commands
def handle_play_pause(command):
    try:
        sp = spotipy.SpotifyOAuth(client_id=client_id,
                                  client_secret=client_secret,
                                  redirect_uri=redirect_uri,
                                  scope=scope)
        token_dict = sp.get_access_token()
        token = token_dict['access_token']

        spotify_object = spotipy.Spotify(auth=token)

        if "play" in command:
            spotify_object.start_playback()
        elif "pause" in command:
            spotify_object.pause_playback()
        elif "forward" in command:
            spotify_object.next_track()
        elif "reverse" in command:
            spotify_object.previous_track()
    except (socket.error, spotipy.SpotifyException) as e:
        print(f"Error occurred while handling command: {str(e)}")
    except Exception as e:
        print(f"Unexpected error occurred while handling command: {str(e)}")

# Function to receive commands from the QML application
def receive_commands():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((udp_host, 12456))

    while True:
        data, addr = sock.recvfrom(1024)  # buffer size is 1024 bytes
        try:
            command = json.loads(data.decode('utf-8'))
            print(f"Received command: {command}")
            handle_play_pause(command)
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON: {str(e)}")
        except Exception as e:
            print(f"Error occurred while handling command: {str(e)}")

# Start the thread to receive commands
import threading
threading.Thread(target=receive_commands, daemon=True).start()

# Start updating the currently playing track information
update_current_track()
