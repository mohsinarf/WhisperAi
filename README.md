# Speech-to-Text Server

## Overview
This project implements a simple speech-to-text server using Python's socket programming and the Whisper library for speech recognition. The server receives audio files in WAV format, transcribes them into text using the specified pre-trained model, and sends back the transcribed text to the client.

## Requirements
- Python 3.x
- Whisper library (installed via pip: `pip install whisper`)

## Supported Models
The server supports the following pre-trained models for speech recognition:
- **Tiny**: 39 million parameters, English-only model, tiny.en
- **Base**: 74 million parameters, English-only model, base.en
- **Small**: 244 million parameters, English-only model, small.en
- **Medium**: 769 million parameters, English-only model, medium.en
- **Large**: 1550 million parameters, Large model (multilingual), large.en

## Dependencies
- `socket`: Used for creating server-client communication via TCP/IP.
- `whisper`: A library for speech recognition and synthesis.

## Usage
1. Run the server script on the machine where you want to receive the audio files and perform speech-to-text transcription.
2. Clients can connect to the server and send audio files in WAV format.
3. The server transcribes the audio using the specified model and sends back the transcribed text.

## Usage Example
```bash
python server.py
```

## Configuration
- TEXT_HOST: IP address of the server.
- TEXT_PORT: Port number used by the server.
- model: Specifies the pre-trained model to be used for speech recognition.
## Workflow
1) The server listens for incoming connections.
2) Upon connection, the server receives the audio file.
3) The server transcribes the audio into text using the specified model.
4) The transcribed text is sent back to the client.
