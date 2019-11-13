#include "SenderSocket.h"
#include "pch.h"

SenderSocket::SenderSocket()
{
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	RTO = 1;
	tlast = clock();
	seq_num = 0;
}

int SenderSocket::Open(string targetHost, int port, int windowSize, LinkProperties linkProperties)
{
	//dont try again if connection has been established
	if (connected)
		return ALREADY_CONNECTED;

	//SYN header creation
	SenderSynHeader synHeader;
	synHeader.linkProperties = linkProperties;
	synHeader.senderDataHeader.flags.SYN = 1;
	synHeader.senderDataHeader.sequence = 0;

	//create socket
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		printf("Socket creation failed with status: %d\n", WSAGetLastError());
		return SOCK_ERROR;
	}

	//fill host infromation 
	host.sin_family = AF_INET;
	host.sin_addr.s_addr = INADDR_ANY;
	host.sin_port = htons(0);

	//bind socket
	if (bind(sock, (struct sockaddr*) & host, sizeof(host)) == SOCKET_ERROR)
	{
		printf("Bind failed with status: %d\n", WSAGetLastError());
		return SOCK_ERROR;
	}

	struct hostent* remote;

	DWORD IP = inet_addr(targetHost.c_str());
	//start clock
	clock_t tlast = clock();
	//find IP for server
	if (IP = INADDR_NONE)
	{
		if ((remote = gethostbyname(targetHost.c_str())) == NULL) //not valid Ip do dns lookup
		{
			/*printf("[%0.3f] --> target %s is invalid\n", 
				(clock() - tlast) / CLOCKS_PER_SEC, targetHost);*/
			return INVALID_NAME;
		}
		else //we have found IP
		{
			memcpy((char*) & (server.sin_addr), remote->h_addr, remote->h_length);
		}
	}
	else
	{
		//given IP
		server.sin_addr.s_addr = IP;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	//define retransmission timeout
	struct timeval tv;
	tv.tv_sec = (DWORD)RTO;
	tv.tv_usec = (DWORD)((RTO * (double)1000000) - (tv.tv_sec * (double)1000000));
	
	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	//start sending "open" packets
	int attemptCount = 0;
	tlast = clock();
	while (attemptCount < MAX_SYN_ATTEMPTS)
	{
		/*printf("[ %.3f] --> SYN %d (attempt %d of %d, RTO %.3f) to %s\n", 
			(double)(clock() - tlast) / CLOCKS_PER_SEC, synHeader.senderDataHeader.sequence,
			attemptCount + 1, MAX_SYN_ATTEMPTS, RTO, inet_ntoa(server.sin_addr));*/

		//send 
		if (sendto(sock, (char*)& synHeader, sizeof(synHeader), 0, 
			(struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
		{
			/*printf("[ %.3f] --> failed sendto with : %d\n", 
				(double)(clock() - tlast) / CLOCKS_PER_SEC, WSAGetLastError());*/
			return FAILED_SEND;
		}

		//we have response create receiver header
		ReceiverHeader receiveHeader;

		//from HW 1
		if (select(sock, &fdset, NULL, NULL, &tv)>0)
		{
			int ansLen = sizeof(server);
			int bytes = recvfrom(sock, (char*) &receiveHeader, sizeof(receiveHeader), 0,
				(struct sockaddr*)&server, &ansLen);
			if (bytes < 0)
			{
				/*printf("[ %.3f] <-- failed recvfrom() with: %d\n", 
					(double)(clock() - tlast) / CLOCKS_PER_SEC, WSAGetLastError());*/
				return FAILED_RECV;
			}
			connected = true;
			//change RTO to RTT
			RTO = ((double)(clock() - tlast)) / CLOCKS_PER_SEC;
			RTO = RTO * 3;//RTO has to be 3 RTT due to host -> server -> host
			/*printf("[ %.3f] <-- SYN-ACK %d window %d, setting initial RTO to %.3f\n",
				((double)(clock() - tlast)) / CLOCKS_PER_SEC, receiveHeader.ackSequence, 
				receiveHeader.recieverWindow, RTO);*/
			return STATUS_OK;
		}
		//end of HW 1 fragment
		attemptCount++;
	}
	return TIMEOUT;
}

int SenderSocket::Close()
{
	if (!connected)
		return NOT_CONNECTED;

	//create FIN packet
	SenderDataHeader finHeader;
	finHeader.flags.FIN = 1;
	finHeader.sequence = 0;


	//define retransmission timeout
	struct timeval tv;
	tv.tv_sec = (DWORD)RTO;
	tv.tv_usec = (DWORD)((RTO * (double)1000000) - (tv.tv_sec * (double)1000000));

	fd_set fdset;
	FD_ZERO(&fdset);
	FD_SET(sock, &fdset);
	//start sending "open" packets
	int attemptCount = 0;
	while (attemptCount < MAX_ATTEMPTS)
	{
		tlast = clock();
		/*printf("[ %.3f] --> SYN %d (attempt %d of %d, RTO %.3f)\n",
			(double)(clock() - tlast) / CLOCKS_PER_SEC, finHeader.sequence,
			attemptCount + 1, MAX_ATTEMPTS, RTO);*/

		//send 
		if (sendto(sock, (char*)& finHeader, sizeof(finHeader), 0,
			(struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
		{
			/*printf("[ %.3f] --> failed sendto with : %d\n",
				(double)(clock() - tlast) / CLOCKS_PER_SEC, WSAGetLastError());*/
			return FAILED_SEND;
		}

		//we have response create receiver header
		ReceiverHeader receiveHeader;

		//from HW 1
		if (select(sock, &fdset, NULL, NULL, &tv)>0)
		{
			int ansLen = sizeof(server);
			int bytes = recvfrom(sock, (char*)& receiveHeader, sizeof(receiveHeader), 0,
				(struct sockaddr*) & server, &ansLen);
			if (bytes < 0)
			{
				/*printf("[ %.3f] <-- failed recvfrom() with: %d\n",
					(double)(clock() - tlast) / CLOCKS_PER_SEC, WSAGetLastError());*/
				return FAILED_RECV;
			}
			/*printf("[ %.3f] <-- FIN-ACK %d window %d\n",
				((double)(clock() - tlast)) / CLOCKS_PER_SEC, receiveHeader.ackSequence,
				receiveHeader.recieverWindow);*/
			
			connected = false;

			if (closesocket(sock) == SOCKET_ERROR)
			{
				/*printf("[ %.3f] <-- failed closesocket() with: %d\n",
					(double)(clock() - tlast) / CLOCKS_PER_SEC, WSAGetLastError());*/
				return SOCK_ERROR;
			}
			return STATUS_OK;
		}
		//end of HW 1 fragment
		attemptCount++;
	}
	return TIMEOUT;
}

int SenderSocket::Send(char* buf, int bytes)
{
	if (!connected)
		return NOT_CONNECTED;

	//buiild header
	SenderDataHeader sendHeader;
	sendHeader.sequence = seq_num;

	//start the buffer that will include the bytes + the size of the header
	char* sendBuf = new char[bytes + sizeof(SenderDataHeader)];
	sendBuf = buf;
	
	int attempts = 0;

}