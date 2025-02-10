import sys

from utils import parse_args, load_config, get_logger
from server import Server

def main():
    args = parse_args()
    config = load_config(args.config)
    
    # Command line arguments take precedence over config file
    if args.host:
        config["host"] = args.host
    if args.port:
        config["port"] = args.port

    logger = get_logger(args.log_path)
    server = Server(config["host"], config["port"])
    server.start_server()
    try:
        server.listen()
    except KeyboardInterrupt:
        pass
    finally:
        server.stop_server()
        logger.close()


if __name__ == "__main__":
    sys.exit(main())