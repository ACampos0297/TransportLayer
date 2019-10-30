#pragma once
#include "pch.h"

class SenderSocket
{
};

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