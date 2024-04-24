import socket
import whisper

TEXT_HOST = '127.0.0.1'  # The server's IP address
TEXT_PORT = 8080  # The port used by the server

# Define the table headers
# | Size   | Parameters | English-only model | Multilingual model | Required VRAM | Relative speed |
# ----------------------------------------------------------------------------------------------
# | tiny   | 39 M       | tiny.en            | tiny               | ~1 GB         | ~32x           |
# | base   | 74 M       | base.en            | base               | ~1 GB         | ~16x           |
# | small  | 244 M      | small.en           | small              | ~2 GB         | ~6x            |
# | medium | 769 M      | medium.en          | medium             | ~5 GB         | ~2x            |
# | large  | 1550 M     | N/A                | large              | ~10 GB        | 1x             |
# ----------------------------------------------------------------------------------------------

model = whisper.load_model("medium")   # tiny.en, base.en, small.en, medium.en, large.en


def receive_wav_file(host, port, filename):
    # Create a socket object
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)

    print(f"Server listening on {host}:{port}")

    # Accept a connection
    client_socket, client_address = server_socket.accept()
    print(f"Connection from: {client_address}")

    # Open a file to write received data
    f = open(filename, 'wb')
    reply_message = "File received successfully."
    client_socket.send(reply_message.encode('utf-8'))

    connected_state = True
    while True:
        data = client_socket.recv(1024 * 10)
        if not data:
            print("Client disconnected")
            break
        # Write received data to the file
        f.write(data)

    print("File received successfully.")

    # Send a reply to the client
    # reply_message = "File received successfully."
    # client_socket.send(reply_message.encode('utf-8'))
    # print("reply_message sent successfully.")
    # client_socket.sendmsg()

    # Close the client socket
    client_socket.close()

    # Close the server socket
    server_socket.close()

    f.close()

    result = model.transcribe(FILENAME, fp16=False)
    print(result["text"])

    send_long_message(client_address[0], TEXT_PORT, result["text"])


def send_long_message(HOST, PORT, long_message):
    try:
        # Create a socket object
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((HOST, PORT))

            # Send the message in fixed-size chunks
            chunk_size = 64
            bytes_sent = 0
            while bytes_sent < len(long_message):
                chunk = long_message[bytes_sent:bytes_sent + chunk_size]
                s.sendall(chunk.encode())
                bytes_sent += len(chunk)

            print("Message sent successfully!")

    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    # HOST = '127.0.0.1'  # Use your server's IP address
    HOST = '192.168.0.33'  # Use your server's IP address
    PORT = 5000  # Choose any available port
    FILENAME = 'received_file.mp3'
    while True:
        receive_wav_file(HOST, PORT, FILENAME)

