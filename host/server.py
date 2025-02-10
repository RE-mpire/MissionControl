import socket
import sys
import selectors
from typing import Optional

from utils import log
from workqueue import TaskExecutor

HEADER = 64
FORMAT = 'UTF-8'

class Server:
    def __init__(self, host, port):
        self.host = host
        self.port = port      
        self.accepting = False
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

        self._server.listen(5)
        self.selector.register(self._server, selectors.EVENT_READ, self._accept)
        self._command_thread.start()
        self._observer_thread.start()

        log(f"[Startup] Starting server on {self.host}:{self.port}")

    def listen(self):
        self.is_running = True
        self.accepting = True
        log("[Listening] Waiting for connections...")
        while self.is_running:
            events = self.selector.select(timeout=None)
            for key, _ in events:
                callback = key.data
                callback(key.fileobj)

    def _accept(self, server_socket):
        if self.accepting:
            conn, addr = server_socket.accept()
            conn.setblocking(False)
            log(f"[Connection] New connection from {addr[0]}:{addr[1]}")
            self.selector.register(conn, selectors.EVENT_READ, self._handle_client)
        else:
            log("[Error] Server is not accepting connections right now")

    def _handle_client(self, conn):
        message = self._recv_message(conn)
        if message == "controller":
            self._command_thread.add_command((conn, conn.getpeername()))
            log(f"[Info] Connection to controller {conn.getpeername()[0]}:{conn.getpeername()[1]} established")
        elif message == "observer":
            self._observer_thread.add_command((conn, conn.getpeername()))
            log(f"[Info] Connection to observer {conn.getpeername()[0]}:{conn.getpeername()[1]} established")
        else:
            log(f"[Error] Unknown connection type from {conn.getpeername()[0]}:{conn.getpeername()[1]}")
            self.selector.unregister(conn)
            conn.close()

    def stop_server(self):
        self.is_running = False
        self.selector.close()
        self._server.close()
        self._command_thread.notify_shutdown()
        self._observer_thread.notify_shutdown()
        self._command_thread.join()
        self._observer_thread.join()
        log("[Shutdown] Server stopped")

    def _recv_message(self, conn: socket.socket) -> Optional[str]:
        try:
            message_length = conn.recv(HEADER).decode(FORMAT)
            if message_length:
                message = conn.recv(int(message_length)).decode(FORMAT)
                conn.send("Msg received".encode(FORMAT))
                log(f"[Message] {message}", silent=True)
                return message
        except BlockingIOError:
            return None
        return None
