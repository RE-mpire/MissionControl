import argparse
import socket
import sys

HEADER = 64
SERVER = "localhost"
FORMAT = 'UTF-8'

def parse_args():
    parser = argparse.ArgumentParser(description="Client for remote server")
    parser.add_argument("-c", "--controller", action="store_true", help="Connect as controller, default is observer")
    parser.add_argument("-s", "--server", type=str, required=True, help="Server address")
    parser.add_argument("-p", "--port", type=int, required=True, help="Port number")
    return parser.parse_args()

def send(client: socket.socket, msg: str) -> str:
    message = msg.encode(FORMAT)
    msg_length = len(message)
    send_length = str(msg_length).encode(FORMAT)
    send_length += b' ' * (HEADER - len(send_length))
    client.send(send_length)
    client.send(message)
    ack = client.recv(2048).decode(FORMAT)
    print(f"[Server] {ack}")
    return ack


def main():
    args = parse_args()
    ADDR = (args.server, args.port)

    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(ADDR)

    role = "controller" if args.controller else "observer"
    print(f"[Info] Connecting as {role}")
    send(client, role)

    try:
        while True:
            msg = input("> ")
            if msg == "stop":
                break
            send(client, msg)
    except KeyboardInterrupt:
        pass
    except BrokenPipeError:
        print("[Error] Connection to server lost")
    client.close()

if __name__ == "__main__":
    sys.exit(main())