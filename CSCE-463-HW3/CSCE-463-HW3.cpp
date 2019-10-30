/*
	Abdul Campos
	325001471
	HW2 P1
	CSCE 463 - 500
	Fall 2019
*/

#include "pch.h"

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

int main(int argc, char* argv[])
{
	if (argc < 8) //wrong command line arguments
	{
		printf("usage: [destination server] [buffer size] [sender window]"
			"[RTT propagation delay] [forward loss rate] "
			"[return loss rate] [link speed]\n");
		return -1;
	}

	string server = argv[1]; //destination server
	int bufferSize, senderWindow, linkSpeed; //buffer size (power of 2) Sender window in packets, link speed
	float RTT, fwLossRate, rtLossRate; //RTT in seconds, loss rates

	bufferSize = atoi(argv[2]);
	senderWindow = atoi(argv[3]);
	linkSpeed = atoi(argv[7]);

	//check if power of 2
	if (!(bufferSize && (!(bufferSize & (bufferSize - 1)))))
	{
		printf("usage: [destination server] [buffer size] [sender window]"
			"[RTT propagation delay] [forward loss rate] "
			"[return loss rate] [link speed]\n");
		return -1;
	}

	RTT = strtof(argv[4],NULL);
	fwLossRate = strtof(argv[5], NULL);
	rtLossRate = strtof(argv[6], NULL);

	//Main information
	printf("Main:\tsender W = %d, RTT %.3f sec, loss %.0e / %.0e, link %d Mbps\n",
		senderWindow, RTT, fwLossRate, rtLossRate, linkSpeed);
	printf("Main:\tInitializing DWORD array with 2^%d elements... ", bufferSize);

	//start clock
	clock_t tlast = clock();

	//initialize buffer
	UINT64  dwordBufSize = (UINT64) 1 << bufferSize;
	DWORD* dwordBuf = new DWORD[dwordBufSize]; // user-requested buffer
	for (UINT64 i = 0; i < dwordBufSize; i++) // required initialization
		dwordBuf[i] = i;
	
	printf("done in %d ms\n", (clock()-tlast)/CLOCKS_PER_SEC);

	//Initialize WinSock; once per program run
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("WSAStartup error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

}