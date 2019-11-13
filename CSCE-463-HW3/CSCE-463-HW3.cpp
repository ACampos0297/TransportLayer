/*
	Abdul Campos
	325001471
	HW2 P1
	CSCE 463 - 500
	Fall 2019
*/

#include "pch.h"
#include "SenderSocket.h"

class Parameters {
public: 
	HANDLE eventQuit;
	HANDLE	mutex;
	SenderSocket* sock;
	clock_t init;
};

UINT statsThread(LPVOID pParam)
{
	Parameters* p = ((Parameters*)pParam);

	while (WaitForSingleObject(p->eventQuit, 2000) == WAIT_TIMEOUT)
	{
		WaitForSingleObject(p->mutex, INFINITE);					// lock mutex
		//printf("[%3.0f] B %5d ( %.1f MB ) N %5d T %2d F 0 W 1 S %0.3f Mbps RTT %.3f\n");
		printf("[% 3.0f]\n", floor((clock() - p->init) / CLOCKS_PER_SEC));

		ReleaseMutex(p->mutex);										// unlock mutex
	}
	printf("[% 3.0f]\n", floor((clock() - p->init) / CLOCKS_PER_SEC));

	//last print goes here

	return 0;
}

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
	int bufferSize, senderWindow, linkSpeed; //buffer size Sender window in packets, link speed
	float RTT, fwLossRate, rtLossRate; //RTT in seconds, loss rates

	bufferSize = atoi(argv[2]);
	senderWindow = atoi(argv[3]);
	linkSpeed = atoi(argv[7]);
	RTT = strtof(argv[4],NULL);
	fwLossRate = strtof(argv[5], NULL);
	rtLossRate = strtof(argv[6], NULL);

	//Main information
	printf("Main:\tsender W = %d, RTT %.3f sec, loss %g / %g, link %d Mbps\n",
		senderWindow, RTT, fwLossRate, rtLossRate, linkSpeed);
	printf("Main:\tInitializing DWORD array with 2^%d elements... ", bufferSize);

	//start clock
	clock_t tlast = clock();

	//initialize buffer
	UINT64  dwordBufSize = (UINT64) 1 << bufferSize;
	DWORD* dwordBuf = new DWORD[dwordBufSize]; // user-requested buffer
	for (UINT64 i = 0; i < dwordBufSize; i++) // required initialization
		dwordBuf[i] = i;
	
	printf("done in %d ms\n", (double)((clock()-tlast)/CLOCKS_PER_SEC));

	//Initialize WinSock; once per program run
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0) {
		printf("Main:\tWSAStartup error %d\n", WSAGetLastError());
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
	tlast = clock();

	if ((status = ss.Open(server, MAGIC_PORT, senderWindow, lp)) != STATUS_OK)
	{
		printf("Main:\tconnect failed with code: %d\n", status);
		return -1;
	}

	//connected successfully
	printf("Main:\tconnected to %s in %.3f sec, packet size %d\n", server.c_str(),
		(double)(clock() - tlast) / CLOCKS_PER_SEC, MAX_PKT_SIZE);

	//start stats thread
	//Prepared shared parameters based on sample code 
	Parameters p;
	// create a quit event for stat thread
	p.eventQuit = CreateEvent(NULL, true, false, NULL);
	// create a mutex for accessing critical sections (including printf); initial state = not locked
	p.mutex = CreateMutex(NULL, 0, NULL);
	//start thread clock
	p.init = clock();
	//send socket to shared params
	p.sock = &ss;

	//to implement in next part
	
	char* charBuf = (char*)dwordBuf; // this buffer goes into socket
	UINT64 byteBufferSize = dwordBufSize << 2; // convert to bytes
	UINT64 off = 0; // current position in buffer

	HANDLE statThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)statsThread, &p, 0, NULL);
	
	//start transfer clock for main
	tlast = clock();

	
	while (off < byteBufferSize)
	{
		// decide the size of next chunk
		int bytes = min(byteBufferSize - off, MAX_PKT_SIZE - sizeof(SenderDataHeader));
		//Next chunk cannot be less than 0
		if (bytes <= 0)
		{
			break;
		}
		// send chunk into socket
		status = 

		off += bytes;
	}

	


	//close connection
	int closestatus;
	if ((closestatus = ss.Close()) != STATUS_OK)
	{
		printf("Main:\tclose failed with code:%d\n", closestatus);
		return -1;
	}
	printf("Main:\ttransfer finished in %.3f sec\n", 
		(double)(clock() - tlast) / CLOCKS_PER_SEC);

	

	//exit stats thread
	SetEvent(p.eventQuit);
	WaitForSingleObject(statThread, INFINITE);
	CloseHandle(statThread);

	return 0;
}