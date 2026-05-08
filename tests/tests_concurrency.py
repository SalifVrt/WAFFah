import threading
import requests
import random
import time
import sys

#WAFFah is running on this URL by default
PROXY_URL = "http://localhost:8888"

def send_request(client_id):
    #small random delay to simulate real-world traffic patterns
    time.sleep(random.uniform(0, 0.5))
    try:
        response = requests.get(PROXY_URL + "/", timeout=10)
        print(f"[Client {client_id}] Status: {response.status_code}")
    except Exception as e:
        print(f"[Client {client_id}] Error: {e}")

#testing maximum capacity defined in proxy.h (MAX_CLIENTS = 10)
NB_CLIENTS = 10 
print(f"Starting WAF saturation test ({NB_CLIENTS} simultaneous clients)...")
print("-" * 60)

threads = []
for i in range(NB_CLIENTS):
    t = threading.Thread(target=send_request, args=(i,))
    threads.append(t)
    t.start()

for t in threads:
    t.join()

print("-" * 60)
print("Load test complete.")