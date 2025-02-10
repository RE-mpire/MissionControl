import argparse
import sys
import yaml
from datetime import datetime

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Host server configuration")
    parser.add_argument("-c", "--config", type=str, default="./config.yaml", help="Path to the configuration file")
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

def log(message: str, silent: bool = False):
    if not silent:
        print(message)
    try:
        with open("log.txt", "a") as f:
            f.write(f"{datetime.now()} -- {message}\n")
    except Exception as e:
        print(f"[WARNING] Could not write to log file: {e}")