import threading
import queue
import time

class TaskExecutor(threading.Thread):
    def __init__(self):
        super().__init__()
        self.command_queue = queue.Queue()
        self.shutdown_flag = threading.Event()
        self.condition = threading.Condition()

    def run(self):
        while not self.shutdown_flag.is_set():
            with self.condition:
                while self.command_queue.empty() and not self.shutdown_flag.is_set():
                    self.condition.wait()
                if self.shutdown_flag.is_set():
                    break
                command = self.command_queue.get()
            self.process_command(command)

    def process_command(self, command):
        print(f"Processing command: {command}")
        time.sleep(1)  # Simulate time-consuming task

    def add_command(self, command):
        with self.condition:
            self.command_queue.put(command)
            self.condition.notify()

    def notify_shutdown(self):
        with self.condition:
            self.shutdown_flag.set()
            self.condition.notify()