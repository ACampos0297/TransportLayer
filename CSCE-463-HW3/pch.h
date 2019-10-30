/*
	Abdul Campos
	325001471
	HW3 P1
	CSCE 463 - 500
	Fall 2019
*/

// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#pragma once
#pragma comment(lib, "ws2_32.lib")


#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here

#include <stdio.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <ctime>
#include <fstream>

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

//Flag header
#define MAGIC_PROTOCOL 0x8311AA 

//LinkProperties
#define FORWARD_PATH 0
#define RETURN_PATH 1 

using namespace std;
#endif //PCH_H
