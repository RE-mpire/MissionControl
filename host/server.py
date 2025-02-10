import selectors
import socket
import sys
from typing import Optional

from utils import log
from workqueue import TaskExecutor

HEADER = 64
FORMAT = 'UTF-8'

class Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port      
        self.accepting = True
        self.is_running = False
        self._command_thread = TaskExecutor()
        self._observer_thread = TaskExecutor()
        self._server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self._server.setblocking(False)
        self.selector = selectors.DefaultSelector()  

    def start_server(self):
        try:
            self._server.bind((self.host, self.port))
        except OSError:
            log(f"[Error] Could not bind to {self.host}:{self.port}")
            sys.exit(1)

        # create background thread to handle incoming connections
        self._command_thread.start()
        self._observer_thread.start()

        log(f"[Startup] Starting server on {self.host}:{self.port}")

    def listen(self):
        self.is_running = True
        self._server.listen(5)
        log("[Listening] Waiting for connections...")

    def accept_connection(self):
        if self.accepting:
            conn, addr = self._server.accept()
            log(f"[Connection] New connection from {addr[0]}:{addr[1]}")

            message = self._recv_message(conn)
            if message == "controller":
                self._command_thread.add_command((conn, addr))
                log(f"[Info] Connection to controller {addr[0]}:{addr[1]} established")
            elif message == "observer":
                self._observer_thread.add_command((conn, addr))
                log(f"[Info] Connection to observer {addr[0]}:{addr[1]} established")
            else:
                log(f"[Error] Unknown connection type from {addr[0]}:{addr[1]}")
                conn.close()
        else:
            log("[Error] Server is not accepting connections")

    def stop_server(self):
        self.is_running = False
        self._server.close()
        self._command_thread.notify_shutdown()
        self._observer_thread.notify_shutdown()
        self._command_thread.join()
        self._observer_thread.join()
        log("[Shutdown] Server stopped")

    def _recv_message(self, conn: socket.socket) -> Optional[str]:
        message_length = conn.recv(HEADER).decode(FORMAT)
        if message_length:
            message = conn.recv(int(message_length)).decode(FORMAT)
            conn.send("Msg received".encode(FORMAT))
            log(f"[Message] {message}", silent=True)
            return message
        return None

    def _handle_controller(self, conn: socket.socket, addr: tuple):
        connection_active = True
        while self.is_running and connection_active:
            try:
                data = self._recv_message(conn)
                if data:
                    log(f"[{addr}] {data}")
            except ConnectionResetError:
                connection_active = False
                continue

        conn.close()
        log(f"[Controller] Connection to controller {addr[0]}:{addr[1]} closed")
            

    def _handle_observer(self):
        pass


