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
./client <server_ip> <port> <mode> <topic>

# Example
./client 127.0.0.1 3000 PUBLISHER hello_topic
```

This connects the client to the server running on IP 127.0.0.1 and port 3000 as a publisher who can publish to the topic hello_topic

Two types of modes:

- PUBLISHER
- SUBSCRIBER

Make sure to run these within the root directory. 

## 💬 Features

- Threaded TCP server using pthread
- Multiple client support
- Message loop: send/receive until client types exit
- Topic-based message infrastructure (WIP/future scope)
- Clean modular design

## 🛠️ Technical Implementation Details

This Pub/Sub system is built in pure C using low-level system programming concepts. Below are the key technical highlights:

### 🔌 Socket Programming

- Implemented using POSIX sockets (<sys/socket.h>) to create full-duplex TCP connections.
- Uses AF_INET (IPv4) with SOCK_STREAM for TCP communication.
- Configures SO_REUSEADDR using setsockopt() to allow fast socket rebinding during rapid testing.

### 📡 Client-Server Architecture

The server listens for incoming client connections using listen() and accept(), operating in a loop to handle multiple clients.

Each client must specify:

- Mode: PUBLISHER or SUBSCRIBER
- Topic: a string to route messages by interest

 ### 🧵 Multithreading (Concurrency)
 
Server Side:

- Uses pthread_create() to spawn a new thread (handle_client) for each client.
- Uses pthread_detach() to avoid memory leaks and free thread resources automatically.

Client Side:

- Creates two dedicated threads:
- One to handle user input and send messages (send_thread_func)
- One to receive messages from the server and print them (recv_thread_func)
- Ensures asynchronous data flow.

 ### 🔄 Graceful Termination
 
- Clients can type terminate to gracefully shut down:
  - Uses shutdown(sock, SHUT_RDWR) to signal both threads to exit.
  - Prevents dangling socket reads on the receiver thread after sender exit.
- Proper close() calls on all sockets to free system resources.

### 📨 Topic-Based Publish/Subscribe Logic

- Clients are stored in a global Client array with their: socket_fd, mode, and topic.
- A publisher's message is only sent to subscribers of the same topic.
- **Subscribers never receive messages from:**
  - Other subscribers
  - Unrelated publishers

 ### 🔄 Full Duplex I/O
 
- Client and server maintain bidirectional, simultaneous message flow using recv() and send().
