/** Library for RehaMove3 and RehaIngest devices Connection.
  * Including UDP Client and Server definitions.
  *
*/

#ifndef SASTEST_H_ // Include guards
#define SASTEST_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#undef UNICODE

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
// Standard C++ and C libraries
#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <ctime>
#include <iostream>
#include <vector>

//Connection settings
#pragma warning(disable : 5208)
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512
using namespace std;

typedef struct {
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	fd_set fds;
	int n;
	struct timeval tv;
	bool error = false;
	uint32_t PORTn = 30007;            // Robert's background task port
	bool display = true;
	char SERVERc[15] = "172.31.1.147"; // Robert's IP address
	// Robot variables
	bool isMoving = false;
	bool Reached = false;
	// Message delimiters:
	char start_msg[7] = "ROBERT";
	char field1[5] = "Move";
	char field2[5] = "Stop";
	char delim_msg = 59;  // ASCII Semicolon ; code
	char end_msg = 33;    // ASCII Exclamation ! code
	bool valid_msg = false;
	//Functions
	void start() {
		//Initialise winsock
		printf("UPD Connection - starting...\n");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			error = true;
		}
		//create socket
		if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			printf("socket() failed with error code : %d", WSAGetLastError());
			error = true;
		}
		//setup address structure
		memset((char*)&si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(PORTn);
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVERc);
		// Set up the struct timeval for the timeout.
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(s, &fds);
		char* ip = inet_ntoa(si_other.sin_addr);
		std::cout << "UDP Client running on IP " << ip << ", port " << PORTn << endl;
		//send a "hello!" so the server can start sending stuff
		strcpy(message, "RobotStateInformer;STATUS;1");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			error = true;
		}
		// For afterwards:
		//strcpy(message, "RobotStateInformer;STATUS;1");
	};
	void get() {
		memset(buf, '\0', BUFLEN);
		valid_msg = false;
		//if(display){printf("UPD: Requesting status...\n");}
		//strcpy(message, "RobotStateInformer;Ping;1");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			error = true;
		}
		// Wait until timeout or data received.
		n = select(s, &fds, NULL, NULL, &tv);
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0) || (n == -1))
		{
			//if(n==0){printf("Timeout\n");}
			error = true;
			Sleep(3000);
		}
		if (!error) {
			int length = sizeof(SERVERc);
			recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);
			if (display) { std::cout << "UDP Received: " << buf << "  --->  "; }// endl; }// puts(buf);
	  // Decode message
			int comp = 0, pos = 0, pos_cont = 0, j = 0;
			bool comp_b = false, valid1 = false, valid2 = false;
			//Looking for beginning of the message:
			while ((j < BUFLEN) && !valid_msg) {
				// Beginning of the string found
				if ((buf[j] == 'R') && (pos_cont <= 1)) { //start_msg[0]){
					comp_b = true;
					pos_cont = 0;
					comp = 0;
					pos = j;
					//  printf("Beginning found on pos %d.\n", pos);
				}
				// If the beginning is found, start storing the "ROBERT" identifier
				if ((comp_b) && (pos_cont < 7)) {
					comp = comp + (buf[j] - start_msg[j - pos]);
					pos_cont++;
				}
				// After 6 iteractions, verify that the identifier was found
				if ((pos_cont >= 6) && (comp == 0)) {
					//std::cout<<"\nMessage found. valid 1 = "<<buf[pos+7]<<", valid 2 = "<<buf[pos+9]<<endl;
					valid1 = (buf[pos + 7] == '1') || (buf[pos + 7] == '0');
					valid2 = (buf[pos + 9] == '1') || (buf[pos + 9] == '0');
					valid_msg = (comp == 0) && valid1 && valid2;
					//printf("RESULT: valid1 = %d, valid2 = %d, valid_msg = %d. ", valid1, valid2, valid_msg);
				}
				else if (pos_cont >= 6) {
					comp_b = false; // If not found_set up back
				}
				j++;
			}
			// Booleans from Robert: only true if valid
			//std::cout<<"Found: "<<buf[pos+7]<<", reached "<<buf[pos+9]<<endl;
			isMoving = valid_msg && (buf[pos + 7] == '1');
			Reached = valid_msg && (buf[pos + 9] == '1');
			if (valid_msg && display) {
				printf("UDP interpretation: isMoving = %d, Reached = %d\n", isMoving, Reached);
			}
			else if (display) {
				printf("UDP message not valid\n");
			}

		}
	};
	void end() {
		closesocket(s);
		WSACleanup();
		if (display) { printf("UDP Connection closed\n"); }
	};
}UDPClient;

typedef struct {
	// Communication variables
	int iResult;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
	int recvbuflen = BUFLEN;
	char sendbuf[BUFLEN];// = "Client: sending data test";
	char recvbuf[BUFLEN];// = "";
	char SERVERc[15] = "127.0.0.1";
	//uint32_t PORTn = 702;
	int PORTn = 702;
	// Control variables
	bool error = false, display = true;
	fd_set fds;
	int n;
	struct timeval tv;
	// Functions
	void start() {
		printf("TCP Connection - starting...\n");
		memset(recvbuf, '\0', BUFLEN);
		memset(sendbuf, '\0', BUFLEN);
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR) {
			printf("TCP WSAStartup failed with error: %d\n", iResult);
		}
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("TCP socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			error = true;
		}
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr("127.0.0.1");//inet_addr(SERVERc);
		clientService.sin_port = htons(PORTn);//htons(DEFAULT_PORT);
		// Connect to server.
		iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR) {
			printf("TCP connect failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			error = true;
			WSACleanup();
		}
		else {
			char* ip = inet_ntoa(clientService.sin_addr);
			int porti = PORTn;
			printf("TCP Client running on IP %s, port %d\n", ip, porti);
			//Set message for sending
			strcpy(sendbuf, "SCREEN;STATUS;1");
		}
		// Set up the struct timeval for the timeout.
		tv.tv_sec = 3;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(ConnectSocket, &fds);
	};//void start

	void get() {
		// Send data
		if (display) { printf("TCP Requesting status...\n"); }
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			printf("TCP send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			error = true;
		}
		// Wait until timeout or data received.
		if (display) { printf("TCP Waiting message\n"); }
		n = select(ConnectSocket, &fds, NULL, NULL, &tv);
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0) || (n == -1))
		{
			if (n == 0) { printf("TCP Timeout\n"); }
			error = true;
			Sleep(3000);
		}
		if (!error) {
			// Receive data
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0 && display) { printf("TCP Received: %s\n", recvbuf); }
			else if (iResult == 0 && display) { printf("TCP Connection closed\n"); }
			else if (display) {
				printf("TCP recv failed with error: %d\n", WSAGetLastError());
				error = true;
			}
		}

	}; // void get

	void end() {
		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, 1); // 1 = SD_SEND
		if (iResult == SOCKET_ERROR) {
			printf("TCP shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
		}
		// close the socket
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR) {
			printf("TCP close failed with error: %d\n", WSAGetLastError());
			WSACleanup();
		}
		// Receive until the peer closes the connection
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		do {
			if (iResult == 0) { printf("TCP Connection closed\n"); }
			//else if (iResult > 0) {printf("Bytes received: %d\n", iResult);}
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		} while (iResult > 0);

		WSACleanup();
	}; // void end

}TCPClient;

void printHello();

typedef struct {
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[BUFLEN];
	int recvbuflen = BUFLEN;
	char senbuf[BUFLEN];
	int senbuflen = BUFLEN;

	// New
	char PORTc[15] = "30002";
	bool error = false;

	void start() {
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			error = true;
		}
		else {
			printf("TCP Winsock initialised.\n");
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, PORTc, &hints, &result);
		if (iResult != 0) {
			printf("TCP getaddrinfo failed with error: %d\n", iResult);
			WSACleanup();
			error = true;
		}

		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("TCP socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			error = true;
		}
		else {
			printf("TCP Socket created.\n");
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("TCP bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else if (!error) {
			printf("TCP Start-up finished.\n");
		}
		fflush(stdout);
		//clear the buffer by filling null, it might have previously received data
		memset(recvbuf, '\0', BUFLEN);
		memset(senbuf, '\0', BUFLEN);
	} // void start

	void waiting() {
		// Listening until a client connects
		freeaddrinfo(result);
		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else {
			printf("TCP Listening started on port %s. Waiting for a client.\n", PORTc);
		}
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("TCP client accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else if (!error) {
			printf("A TCP client has been accepted.\n");
		}
		// No longer need server socket
		closesocket(ListenSocket);
	} // void waiting

	void stream() {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//printf("TCP Bytes received: %d\n", iResult);
			printf("TCP Data received: %s\n", recvbuf);
			// Echo the buffer back to the sender
			iSendResult = 0;
			//if(i_loop > 2){
			strcpy(senbuf, "SAS;10.4;3;2.5;"); // New format
			printf("TCP sending: %s\n", senbuf);
			// Screen: Variable 1 = 0, variable 2 = 1, variable 3 = 3
			iSendResult = send(ClientSocket, senbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("TCP send failed with error: %d\n", WSAGetLastError());
			}
		}
		else if (iResult == 0)
			printf("TCP Connection closing...\n");
		else {
			printf("TCPrecv failed with error: %d\n", WSAGetLastError());
		}
	} // void stream

	void end() {
		// shutdown the connection since we're done
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR) {
			printf("TCP shutdown failed with error: %d\n", WSAGetLastError());
		}
		else {
			printf("TCP Connection closed.\n");
		}
		// cleanup
		closesocket(ClientSocket);
		WSACleanup();
	} // void end
}TCPServer;

#endif // SASTEST_H_
