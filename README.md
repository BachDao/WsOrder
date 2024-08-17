# WsExchange

## Purpose

Provides a C++ solution for setting up a WebSocket WsClient to connect with a crypto exchange's Order module, with the following requirements:

- **WebSocket connection setup**: Establish a secure connection to the Exchange.
- **Handling unexpected disconnects**: Automatic reconnect in case of disconnections.
- **Minimizing impact on orders**: Minimal disruption to active orders, maintaining data consistency, even connection is interrupted.

## High level design

Our module will be structured in layers, each focusing on specific aspect of the problem
and provide clean, simple API to upper-layer, hiding unnecessary complex details.

From lowest

- **WsConnection**:
  - Established WebSocket secure connection to the Exchange.
  - Allows sending and receiving entire messages rather than just raw byte sequences. 
  - Notifies the upper layer if the WebSocket connection is interrupted.
  - Hiding the details about WebSocket, in fact, we can swap this class with any other
  component that supports byte stream transfer without affecting the application.
  
- **Sender**
  - Use WsConnection to send Message
  - Implements timeout and retry mechanisms, also processes acknowledgment (ACK) packets
  - Provides reliable sending capabilities to the upper layer: eventually our Message will 
  reach to otherside.
  
- **Receiver**
  - Utilizes WsConnection to receive messages.
  - Eliminates duplicate messages and ensures exactly-once delivery to the upper layer.
  
- **WsClient**
  - Acts as the coordinator between Sender, Receiver, and WsConnection.
  - Dispatch message among these components.

- **Order**
  - An immutable object that contains information about the Order.
    - Order type
    - Amount
    - Status
    - .....
    
- **ExchangeClient**
  - Utilize WsClient, provide end-user API for create Order, query Orders status... 
  
Still, we're missing something like OrderManagement to manage and sync-up Order's information bw Client
and Server, but it's enough for demonstration

## Exchange protocol
_Assumption: The protocol is implemented on both sides, WsClient and Exchange server._

The protocol comprises two layers:
- **Connection-Level protocol** (_implemented by first four objects above_)
  - Manages connection states like open, close, and SessionId.
  - Maintains the connection state even if the underlying WebSocket disconnects.
  - These state is tracked by WsClient.

- **Application protocol** (_done by Order and ExchangeClient_)
  - Handles business logic, including order definitions, states, necessary transformations, 
  and synchronization of order information between client and server.


### **Connection level protocol**

Each message contains a header and the payload, header includes control information:
- Length: total length of a message, in byte, including header.
- SessionId: used to identify active connection session
- MessageType: Whether this is Control or Normal message, control message including open, close, ack.
Normal message is application level message, encoded by protobuf
- SequenceNumber: for deduplication purpose

```
+------------------+-----------------------+----------------------+--------------------------+-----------+
| Length (4 bytes) |   SessionId (4 byte)  | MessageType (1 byte) | SequenceNumber (8 bytes) | Payload   |
+------------------+-----------------------+----------------------+--------------------------+-----------+
```

- **WsConnection**:
  - Whenever WebSocket is interrupted, notify the upper layer.
  - And, any data received so far is discarded
  
- **Sender**
  - Sending a message, if:
    - Receive ack, mark that message as completed
    - Time-out without ACK, retransmit
  - Connection interrupted, retransmit any message that has pending Ack
    
- **Receiver**
  - Receive a message, if:
    - That is normal message, response with ACK
    - If that message has outdated SeqNo, discard
    - Otherwise, deliver it to the upper layer

### **Application level protocol**

I'm not familiar with the protocol used by a popular Exchange, so I just laid out a basic
one that I think is enough for demonstration purpose.

Protobuf is used to define our application protocol due to its simplicity in documentation and
build support

The protocol should be simple and easy to follow.
- Exchange server has authority information about the state of each Order.
- WsClient be able to create Order, fill enough information and send to Server
- Server will respond with Order's status: Match or Fail.
- WsClient be able to get information about all the Orders (We don't care about privacy ^^)

For details information, please check out "proto/exchange.proto"

## Connection Recovery

Our protocol maintains data consistency with Interrupt Notification, Retry, and Timeout mechanisms:
- Sender track the latest message that isn't acked yet.
- Receiver sends ACKs for received messages.
- On Disconnection:
  - Sender: Retransmits failed messages.
  - Receiver: Discards incomplete messages, waits for retransmission.
- Receiver rejects outdated messages.

## Implementation notes

**boost-beast** is used for WebSocket handling, with SSL to secure the connection.

**boost-signal2** is used in some places for management and triggering of callbacks.

Each connection object maintains a queue of messages and is associated with a 
unique SessionId, separate from the underlying WebSocket connection.

Each side implements both Sender and Receiver role.

## Build

**Dependency**
- Boost 1.84 or higher
- CMake 3.12 or higher
- lib-protobuf and protoc compiler
- lib-openssl
- C++17 compatible compiler

On a *nix system with the required dependencies, execute **build.sh** to start the build.

Otherwise, run **docker_build.sh** to create a Docker container with the necessary dependencies and build inside it.

## Limitations

- **Message Copies**: Currently, there are some cases where Message objects are copied
between different components. These copies should be optimized to enhance performance.

- **Timeout Calculation**: The timeout for retransmissions is set to a fixed value.
It should be dynamically calculated and update based on network behavior.