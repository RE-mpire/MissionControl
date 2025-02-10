import socket
import threading
import sys

from utils import log

HEADER = 64
FORMAT = 'UTF-8'

class Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port      
        self.accepting = True
        self.is_running = False
        self._server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  

    def start_server(self):
        try:
            self._server.bind((self.host, self.port))
        except OSError:
            log(f"[Error] Could not bind to {self.host}:{self.port}")
            sys.exit(1)

        log(f"[Startup] Starting server on {self.host}:{self.port}")

    def listen(self):
        self.is_running = True
        self._server.listen(5)
        log("[Listening] Waiting for connections...")

    def accept_connection(self):
        if self.accepting:
            conn, addr = self._server.accept()
            log(f"[Connection] New connection from {addr}")

            if conn.recv(HEADER).decode(FORMAT) == "controller":
                threading.Thread(target=self._handle_controller, args=(conn, addr)).start()
            elif conn.recv(HEADER).decode(FORMAT) == "observer":
                threading.Thread(target=self._handle_observer, args=(conn, addr)).start()
            else:
                log(f"[Error] Unknown connection type from {addr}")
                conn.close()
        else:
            log("[Error] Server is not accepting connections")

    def stop_server(self):
        self.is_running = False
        self._server.close()
        log("[Shutdown] Server stopped")

    def _handle_controller(self, conn: socket.socket, addr: tuple):
        connection_active = True
        while self.is_running and connection_active:
            try:
                data = conn.recv(HEADER).decode(FORMAT)
                if not data:
                    connection_active = False
                    continue
                log(f"[{addr}] {data}")
            except ConnectionResetError:
                connection_active = False
                continue

        conn.close()
        log(f"[Controller] Connection to controller {addr} closed")
            

    def _handle_observer(self):
        pass


