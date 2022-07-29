from idl import *

@endpoint
class Echo:
    def send_message(m: str) -> str: pass