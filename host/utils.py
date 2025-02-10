import argparse
import sys
import yaml
from datetime import datetime

logger = None

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Host server configuration")
    parser.add_argument("-c", "--config", type=str, default="config.yaml", help="Path to the configuration file")
    parser.add_argument("-l", "--log-path", type=str, default="log.txt", help="Path to the log file")
    parser.add_argument("-H", "--host", type=str, help="Host address (takes precedence over config file)")
    parser.add_argument("-P", "--port", type=int, help="Port number (takes precedence over config file)")

    try:
        args = parser.parse_args()
    except argparse.ArgumentError as e:
        print(f"Argument error: {e}")
        parser.print_help()
        sys.exit(2)
    
    return args

def load_config(path: str) -> dict:
    config = {}
    print(f"[Startup] Loading config from {path}")
    try:
        with open(path, "r") as f:
            config = yaml.safe_load(f)
    except FileNotFoundError:
        print(f"[Error] Could not find config file {path}")
        sys.exit(1)
    except yaml.YAMLError as e:
        print(f"[Error] Error parsing config file: {e}")
        sys.exit(1)
    return config

class Logger:
    def __init__(self, log_path: str = "log.txt"):
        self.log_path = log_path
        try:
            self.log_file = open(self.log_path, "a")
            print(f"[Startup] Logging file will be saved to {self.log_path}")
        except OSError as e:
            print(f"[Warning] Failed to open file. Logs will NOT be saved: {e}")
            self.log_file = None

    def log(self, message: str, silent: bool = False):
        if not silent:
            print(message)
        if self.log_file:
            try:
                self.log_file.write(f"{datetime.now()} -- {message}\n")
                self.log_file.flush()
            except (IOError, ValueError) as e:
                print(f"[WARNING] Could not write to log file: {e}")

    def close(self):
        self.log_file.close()

def get_logger(log_path: str = "log.txt") -> Logger:
    global logger
    if logger is None:
        logger = Logger(log_path)
    return logger

def log(message: str, silent: bool = False):
    get_logger().log(message, silent)
