import socket
import threading

HOST = '127.0.0.1'  # Change to your server's IP if needed
PORT = 58670        # Change to your read socket's port

def connect_and_send(idx):
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            with socket.create_connection((HOST, PORT)) as s:
                msg = f"Hello from client {idx}"
                s.sendall(msg.encode())
                print(f"Client {idx} sent message")
        except Exception as e:
            print(f"Client {idx} error: {e}")

threads = []
for i in range(20):
    t = threading.Thread(target=connect_and_send, args=(i,))
    t.start()
    threads.append(t)

for t in threads:
    t.join()