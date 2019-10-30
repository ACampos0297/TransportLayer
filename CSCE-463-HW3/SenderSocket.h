#pragma once
#include "pch.h"
// SenderSocket.h
#define MAGIC_PORT 22345 // receiver listens on this port
#define MAX_PKT_SIZE (1500-28) // maximum UDP packet size accepted by receiver

//sender socket errors
#define STATUS_OK 0 // no error
#define ALREADY_CONNECTED 1 // second call to ss.Open() without closing connection
#define NOT_CONNECTED 2 // call to ss.Send()/Close() without ss.Open()
#define INVALID_NAME 3 // ss.Open() with targetHost that has no DNS entry
#define FAILED_SEND 4 // sendto() failed in kernel
#define TIMEOUT 5 // timeout after all retx attempts are exhausted
#define FAILED_RECV 6 // recvfrom() failed in kernel
#define SOCK_ERROR 7

//Flag header
#define MAGIC_PROTOCOL 0x8311AA 

//LinkProperties
#define FORWARD_PATH 0
#define RETURN_PATH 1 

//max attempts
#define MAX_ATTEMPTS 5 //try a maximum 3 for syn and 5 for all others 
#define MAX_SYN_ATTEMPTS 3

#pragma pack(push,1)
class Flags {
public:
	DWORD reserved : 5; // must be zero
	DWORD SYN : 1;
	DWORD ACK : 1;
	DWORD FIN : 1;
	DWORD magic : 24;
	Flags() { memset(this, 0, sizeof(*this)); magic = MAGIC_PROTOCOL; }
};

class ReceiverHeader {
public:
	Flags flags;
	DWORD recieverWindow; // receiver window for flow control (in pkts)
	DWORD ackSequence; // ack value = next expected sequence
};

class SenderDataHeader {
public:
	Flags flags;
	DWORD sequence;
};

class LinkProperties {
public:
	// transfer parameters
	float RTT; // propagation RTT (in sec)
	float linkSpeed; // bottleneck bandwidth (in bits/sec)
	float lossProbability[2]; // probability of loss in each direction
	DWORD bufferSize; // buffer size of emulated routers (in packets)
	LinkProperties() { memset(this, 0, sizeof(*this)); }
};

class SenderSynHeader {
public:
	SenderDataHeader senderDataHeader;
	LinkProperties linkProperties;
};
#pragma pack(pop)

class SenderSocket
{
	private: 
		SOCKET sock;
		double RTO;
		struct sockaddr_in host;
		struct sockaddr_in server;
		bool connected = false;
		clock_t tlast;
	public:
		SenderSocket();
		~SenderSocket();
		int Open(string targetHost, int port, int windowSize, LinkProperties lp);
};