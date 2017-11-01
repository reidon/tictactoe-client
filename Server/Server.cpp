// Server.cpp : Defines the entry point for the console application.
//qwertyuiopå
//aa
//FOR THE EMPRAH
//8==D
//Make a comment whatever m8

#include "stdafx.h"
#include <iostream>

#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <process.h>
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 1024
#define DEFAULT_PORT "27020"

int num_of_players[2];
int whichTurn[2];

bool can_join(int roomNo)
{
	if (num_of_players[roomNo] < 2)
	{
		return true;
	}
	return false;
}

DWORD WINAPI ProcessClient(LPVOID lpParameter)
{
	SOCKET ClientSocket = (SOCKET)lpParameter;
	int iResult;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	//Lobby stuff
	int roomNo = 0;

	//Game stuff
	int playerNo;
	bool ourTurn;
	// iSendResult = send(ClientSocket, msg, strlen(msg), 0);	// Send
	// iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);	// Receive
	// sprintf_s(a_char_array, "%d", an_integer);				// Convert int to char array
	// int input_int = atoi(input);								// Convert from char* to int
	// const char *char = a_string.c_str();						// Convert from string to char*

	do { // Lobby loop
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); //waits until something is received
		if (iResult > 0) {
			int recvbufInt = atoi(recvbuf);



			if (can_join(recvbufInt - 1))
			{
				roomNo = recvbufInt - 1;
				iSendResult = send(ClientSocket, "1", (int)strlen("1"), 0);
				break; //Exits the lobby loop
			}
			else
			{
				iSendResult = send(ClientSocket, "0", (int)strlen("0"), 0);
				continue;
			}
			//iSendResult = send(ClientSocket, recvbuf, strlen(recvbuf), 0);

		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

		playerNo = ++num_of_players[roomNo]; //Assigns the player number and increments the number of players
		ourTurn = (playerNo == 1 ? true : false);
		do // Game loop
		{

			//Our turn

			while (whichTurn[roomNo] != playerNo) //Waits for the player's turn
			{

			}

			if (whichTurn[roomNo] == playerNo)
			{
				iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); //waits until something is received
			}

		} while (true);
	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
}

int __cdecl main(void)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;



	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	//// Accept a client socket
	//ClientSocket = accept(ListenSocket, NULL, NULL);
	//if (ClientSocket == INVALID_SOCKET) {
	//	printf("accept failed with error: %d\n", WSAGetLastError());
	//	closesocket(ListenSocket);
	//	WSACleanup();
	//	return 1;
	//}

	while (1)
	{
		ClientSocket = SOCKET_ERROR;

		while (ClientSocket == SOCKET_ERROR)
		{
			ClientSocket = accept(ListenSocket, NULL, NULL);
		}

		printf("Client Connected.\n");

		DWORD dwThreadId;
		CreateThread(NULL, 0, ProcessClient, (LPVOID)ClientSocket, 0, &dwThreadId);
	}

	// No longer need server socket
	closesocket(ListenSocket);

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}



