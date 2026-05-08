# WAFFah

A lightweight, asynchronous HTTP proxy and Web Application Firewall (WAF) implemented in C. 

WAFFah sits between a client and a backend server, intercepting traffic to detect and block common web vulnerabilities in real-time. Designed with a focus on memory safety and non-blocking I/O.

## Features

* **Asynchronous I/O:** Utilizes `select()` to multiplex multiple simultaneous client connections without the overhead of threading.
* **Dynamic Memory Allocation:** Safely handles exceptionally large HTTP requests and headers through dynamic buffer reallocation (`realloc`).
* **Traffic Inspection:** Decodes URL-encoded payloads and scans for malicious signatures:
  * Cross-Site Scripting (XSS)
  * SQL Injection (SQLi)
  * Local File Inclusion (LFI)
* **Audit & Logging:** Records connection attempts, real client IPs, timestamps, and WAF decisions to a local `waf.log` file.
* **Graceful Shutdown:** Intercepts `SIGINT` to safely close active sockets and release ports upon termination.

## Prerequisites

* `gcc` or `clang`
* `make`
* `python3` and `requests` library (for the test suite)

## Build Instructions

Clone the repository and compile the binary using the provided Makefile:

```bash
git clone [https://github.com/SalifVrt/waffah.git](https://github.com/SalifVrt/waffah.git)
cd waffah
make
```

## Usage

Start the proxy by specifying the local port to listen on, alongside the target backend server's IP and port:

```bash
./waffah_bin <local_port> <remote_ip> <remote_port>
```

**Example setup with a dummy backend:**
```bash
# 1. Start a simple python backend in the background
mkdir -p fake_serv && echo "OK" > fake_serv/index.html
python3 -m http.server 8080 --directory fake_serv &

# 2. Start WAFFah
./waffah_bin 8888 127.0.0.1 8080
```

## Running the Test Suite

The repository includes an automated Python test suite to verify routing, dynamic buffer elasticity, and WAF blocking rules.

```bash
python3 tests/test_suite.py
```

## License

This project is licensed under the MIT License - see the LICENSE file for details.