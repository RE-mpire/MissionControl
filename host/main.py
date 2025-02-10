import sys

from utils import parse_args, load_config
from server import Server

def main():
    args = parse_args()
    config = load_config(args.config)
    
    # Command line arguments take precedence over config file
    if args.host:
        config["host"] = args.host
    if args.port:
        config["port"] = args.port

    s = Server(config["host"], config["port"])
    s.start_server()
    s.listen()
    try:
        while s.is_running:
            s.accept_connection()
    except KeyboardInterrupt:
        pass
    finally:
        s.stop_server()

if __name__ == "__main__":
    sys.exit(main())