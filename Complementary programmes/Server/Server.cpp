/*
	Simple UDP//TCP Server
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#undef UNICODE

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <thread>

// ----------------------------- Variables -----------------------------------
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define BUFLEN 512 //Max length of buffer
//#define PORT 8888	//The port on which to listen for incoming data
#define PORT 30009 //Robert port number
// char ROBERT[15] = "172.31.1.147";
char ROBERT[15] = "127.0.0.1";
#define DEFAULT_PORT "30002" //Screen port number
int one_to_escape = 0;
bool main_finish = false;
bool display_msg = true;
char message[BUFLEN];
using namespace std;
// ----------------------------- Structures ----------------------------------
struct TCPServer
{
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo *result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[BUFLEN];
	int recvbuflen = BUFLEN;
	char senbuf[BUFLEN];
	int senbuflen = BUFLEN;

	// New
	char PORTc[15] = "30002";
	bool error = false;

	void start()
	{
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			printf("WSAStartup failed with error: %d\n", iResult);
			error = true;
		}
		else
		{
			printf("TCP Winsock initialised.\n");
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, PORTc, &hints, &result);
		if (iResult != 0)
		{
			printf("TCP getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			error = true;
		}

		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET)
		{
			printf("TCP socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			error = true;
		}
		else
		{
			printf("TCP Socket created.\n");
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			printf("TCP bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else if (!error)
		{
			printf("TCP Start-up finished.\n");
		}
		fflush(stdout);
		//clear the buffer by filling null, it might have previously received data
		memset(recvbuf, '\0', BUFLEN);
		memset(senbuf, '\0', BUFLEN);
	} // void start

	void waiting()
	{
		// Listening until a client connects
		freeaddrinfo(result);
		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR)
		{
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else
		{
			printf("TCP Listening started on port %s. Waiting for a client.\n", PORTc);
		}
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("TCP client accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else if (!error)
		{
			printf("A TCP client has been accepted.\n");
		}
		// No longer need server socket
		closesocket(ListenSocket);
	} // void waiting

	void stream()
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			//printf("TCP Bytes received: %d\n", iResult);
			printf("TCP Data received: %s\n", recvbuf);
			// Echo the buffer back to the sender
			iSendResult = 0;
			//if(i_loop > 2){
			strcpy(senbuf, "SAS;10.4;3;2.5;"); // New format
			printf("TCP sending: %s\n", senbuf);
			// Screen: Variable 1 = 0, variable 2 = 1, variable 3 = 3
			iSendResult = send(ClientSocket, senbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR)
			{
				printf("TCP send failed with error: %d\n", WSAGetLastError());
			}
		}
		else if (iResult == 0)
			printf("TCP Connection closing...\n");
		else
		{
			printf("TCPrecv failed with error: %d\n", WSAGetLastError());
		}
	} // void stream

	void end()
	{
		// shutdown the connection since we're done
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("TCP shutdown failed with error: %d\n", WSAGetLastError());
		}
		else
		{
			printf("TCP Connection closed.\n");
		}
		// cleanup
		closesocket(ClientSocket);
		WSACleanup();
	} // void end
} SCREEN;

// ----------------------------- Functions -----------------------------------
void thread_ml_t1()
{
	std::cout << "Thread doing stuff\n";
	int task1 = 0;

	while (!main_finish)
	{
		task1++;
		Sleep(1000);
		if (task1 >= 10)
		{
			std::cout << "RehaMove3 task 1\n";
			task1 = 0;
		}
	}
}

void UDPServer()
{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	slen = sizeof(si_other);

	//Initialise winsock
	printf("Initialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	// Original:
	//	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	// Modified:
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_addr.S_un.S_addr = inet_addr(ROBERT);
	server.sin_port = htons(PORT);
	char *ip = inet_ntoa(server.sin_addr);
	std::cout << "Running UDP server on IP " << ip << ", port " << PORT << endl;
	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d\n", WSAGetLastError());
		//exit(EXIT_FAILURE);
	}
	puts("UDP Bind done");
	//-------------------------------------------------
	fflush(stdout);
	//printf("Waiting for a client connection...");
	//fflush(stdout);
	//clear the buffer by filling null, it might have previously received data
	memset(buf, '\0', BUFLEN);

	printf("\n================ ROBERT emulation ===============\n");

	//-------------------------------------------------
	int i = 0;
	//keep listening for data
	while (!main_finish) //(one_to_escape == 0)
	{
		//* ORIGINAL CODE
		if (display_msg)
		{
			printf("Waiting for status request...");
		}
		fflush(stdout);

		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == SOCKET_ERROR)
		{
			if (display_msg)
			{
				printf("recvfrom() failed with error code : %d", WSAGetLastError());
			}
			//exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
		if (display_msg)
		{
			printf("UDP Received data: %s\n", buf);
		}

		//-------------------------------------------------
		//memset(buf, '\0', BUFLEN);
		strcpy(buf, message);
		if (display_msg)
		{
			printf("UDP Sending: %s\n", buf);
		}
		if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR)
		{
			if (display_msg)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
			}
			//exit(EXIT_FAILURE);
		}
		memset(buf, '\0', BUFLEN);
		Sleep(10);
		i++;
	}

	closesocket(s);
	WSACleanup();
}

void TCPRunning()
{
	SCREEN.start();
	printf("1st part done.");
	SCREEN.waiting();
	printf("2nd part done.");
	// Receive until the peer shuts down the connection
	int i_loop = 0;
	do
	{
		SCREEN.stream();
		i_loop++;
	} while (SCREEN.iResult > 0);

	SCREEN.end();
}

void keyboard(int &command)
{
	int temp = 0;
	int ch;
	ch = _getch();
	ch = toupper(ch);
	printf("\n---> Key pressed: %c: ", ch);
	if (ch >= '0' && ch <= '9')
	{
		temp = ch - '0';
	}
	if ((ch == 'D') || (ch == 'U'))
	{
		display_msg = !display_msg;
	}
	command = temp;
}

void UDPInterface()
{
	int one_to_escape;
	int command = 0;
	do
	{
		one_to_escape = _kbhit();
		if (one_to_escape != 0)
		{
			keyboard(command);
			switch (command)
			{
			case 1:
				printf("Reset signals <--- \n");
				strcpy(message, "0.0;false;");
				break;
			case 2:
				printf("Only Is Moving <---\n");
				strcpy(message, "5.123;false;");
				break;
			case 3:
				printf("Only End Of Point <---\n");
				strcpy(message, "0.0;true;");
				break;
			case 4:
				printf("Set signals <---\n");
				strcpy(message, "94.012345678;true;");
				break;
			}
		}
		Sleep(10);
	} while (!main_finish);
}

// -----------------------------------------------------------------------------

int main()
{
	//UDPServer();
	//TCPRunning();
	memset(message, '\0', BUFLEN);
	strcpy(message, "0.0;false;");

	printf("=========== ROBERT emulation controllers ===========\n");
	printf("-1: Is Moving and End Of Point Reached (EOPR) both reset to 0.\n");
	printf("-2: Is Moving set to 1 and EOPR reset to 0.\n");
	printf("-3: Is Moving reset to 0 and EOPR set to 1.\n");
	printf("-4: Is Moving and EOPR both set to 1.\n");
	printf("===================================================\n");

	std::thread to_SAS(UDPServer);
	std::thread interface(UDPInterface);
	//
	to_SAS.join();
	interface.join();
	// main_finish = true;
	// task1.join();

	return 0;
}
