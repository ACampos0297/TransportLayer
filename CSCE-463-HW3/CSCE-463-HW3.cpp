/*
	Abdul Campos
	325001471
	HW2 P1
	CSCE 463 - 500
	Fall 2019
*/

#include "pch.h"
#include "SenderSocket.h"
#include "CSCE-463-HW3.h"

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

	//prepare linker info
	LinkProperties lp;
	lp.RTT = RTT;
	lp.linkSpeed = (float)1000000 * linkSpeed;
	lp.lossProbability[FORWARD_PATH] = fwLossRate;
	lp.lossProbability[RETURN_PATH] = rtLossRate;
	lp.bufferSize = senderWindow + MAX_ATTEMPTS;

	SenderSocket ss;
	int status;
	if ((status = ss.Open(server, MAGIC_PORT, senderWindow, lp)) != STATUS_OK)
		return 0;
}