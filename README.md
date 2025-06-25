# 📡 PubSubRio – Simple TCP Pub/Sub System in C

This project implements a basic **publish-subscribe** model using low-level **C sockets and threads**. It features a server that manages topic creation, message routing, and multiple client connections.

---

## 🛠️ Build Instructions

This project uses a simple `Makefile` for compilation.

### 🔧 Requirements

- GCC compiler
- POSIX-compliant system (Linux/macOS/WSL)
- `make` utility

### 🏗️ Build Targets

| Target       | Description                        |
|--------------|------------------------------------|
| `make`       | Builds both `server` and `client` |
| `make server`| Builds only the `server` binary   |
| `make client`| Builds only the `client` binary   |
| `make clean` | Removes all compiled binaries     |

### 📦 Build Example

```bash
# To build both server and client
make

# To build just the server
make server

# To build just the client
make client

# To clean up compiled binaries
make clean
```

---

## 🚀 Run Instructions

After building the binaries, you can run the server and clients as follows.

### 🖥️ Start the Server

```bash
./server <port>

# Example
./server 3000
```

### 🧑‍💻 Start the Client

```bash
./client <server_ip> <port>

# Example
./client 127.0.0.1 3000
```

This connects the client to the server running on IP 127.0.0.1 and port 3000.

## 💬 Features

- Threaded TCP server using pthread
- Multiple client support
- Message loop: send/receive until client types exit
- Topic-based message infrastructure (WIP/future scope)
- Clean modular design
