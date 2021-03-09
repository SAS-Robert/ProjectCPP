#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <conio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iomanip>
#include <complex>
#include <stdio.h>
#include <thread>
#include <ctime>
#include <sstream>
#include <string>
#include <complex>

#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
using namespace std;

#pragma comment(lib, "ws2_32.lib") //Winsock Library
#pragma warning disable

//#define SERVER "127.0.0.1"	//ip address of udp server
#define SERVER "127.0.0.1"
#define BUFLEN 512 //Max length of buffer

int rep_nr = 1; // Repetition number



//---------------- Variables field ----------------------------
enum RehaMove3_Req_Type
{
	Move3_none = 0,		 // Nothing to do
	Move3_incr = 1,		 // Increment current
	Move3_decr = 2,		 // Reduce current
	Move3_ramp_more = 3, // Increase ramp
	Move3_ramp_less = 4, // Reduce ramp
} stimulator_code;

enum RehaIngest_Req_Type
{
	Inge_none = 0, // Nothing to do
	Inge_incr = 1, // Increase threshold gain
	Inge_decr = 2, // Decrease threshold gain
} recorder_code;

enum tcp_msg_Type
{
	exDone = 0,     // Finish everything
	start = 1,       // Exercise has started
	repeat = 2,     // Repeat exercise
	pause = 3,      // Play/Pause button = in pause
	play = 4,     // Play/Pause button = in play 
	setDone = 5,    // Repetitions-set finished
	repStart = 6,   // Start repetition
	repEnd = 7,     // End repetition
	finish = 8,     // Close socket
	msg_none = 9,   // Nothing to do
	msg_invalid = 10, // You messed up somewhere
} ;

struct tcp_msg_struct
{
public:
	const int size = 11;
	tcp_msg_Type status;
	string messages[11];
	// Constructor
	tcp_msg_struct()
	{
		messages[exDone] = "EXERCISE_DONE";
		messages[start] = "PLAY";
		messages[repeat] = "REPEAT";
		messages[pause] = "PAUSE";
		messages[play] = "RESUME";
		messages[setDone] = "SET_DONE";
		messages[repStart] = "CYCLE_START";
		messages[repEnd] = "CYCLE_DONE";
		messages[finish] = "ENDTCP";
		messages[msg_invalid] = " ";
		messages[msg_none] = " ";
		status = msg_none;
	}

}msgList;
//----------------- Objects definition -------------------------

struct UdpClient
{
private:
	struct sockaddr_in si_other;
	int s, slen;
	WSADATA wsa;
	fd_set fds;
	int n;
	uint32_t PORTn;
	char SERVERc[15]; // IP address

public:
	char buf[BUFLEN];
	char message[BUFLEN];
	struct timeval timeout;
	bool error, error_lim;
	int error_cnt;
	bool display;
	// Robot variables
	bool isMoving;
	bool Reached;
	bool valid_msg;

	// Constructor 
	UdpClient(char* S_address, uint32_t port) {
		slen = sizeof(si_other);
		error = false;
		error_lim = false;
		error_cnt = 0;
		display = true;
		// Robot variables
		isMoving = false;
		Reached = false;
		valid_msg = false;

		strcpy(SERVERc, S_address);
		PORTn = port;
	}
	//Functions
	void start()
	{
		//Initialise winsock
		if (display)
		{
			printf("UPD Connection - starting...\n");
		}
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			error = true;
		}
		//create socket
		if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			if (display)
			{
				printf("socket() failed with error code : %d", WSAGetLastError());
			}
			error = true;
		}
		//setup address structure
		memset((char*)&si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		si_other.sin_port = htons(PORTn);
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVERc);
		// Set up the struct timeval for the timeout.
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(s, &fds);
		char* ip = inet_ntoa(si_other.sin_addr);
		if (!error)
		{
			std::cout << "UDP Client running on IP " << ip << ", port " << PORTn << endl;
		}
		//send a start message so the server can start sending stuff
		strcpy(message, " ");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			error = true;
		}
	};
	void get()
	{
		error = false;
		memset(buf, '\0', BUFLEN);
		valid_msg = false;
		if (display)
		{
			printf("UPD: Sending = %s\n", message);
		}
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			if (display)
			{
				printf("sendto() failed with error code : %d", WSAGetLastError());
			}
			error = true;
			error_cnt++;
		}

		// Receive process has been taken out
		error_lim = error_cnt >= 10;
	};
	void end()
	{
		closesocket(s);
		WSACleanup();
		if (display)
		{
			printf("UDP Client connection closed\n");
		}
	};
};

struct TCPClient
{
	// Communication variables
	int iResult;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
	int recvbuflen = BUFLEN;
	char sendbuf[BUFLEN]; // = "Client: sending data test";
	char recvbuf[BUFLEN]; // = "";
	char SERVERc[15] = "172.31.1.147";
	//uint32_t PORTn = 702;
	int PORTn = 30002;
	// Control variables
	bool error = false, display = false;
	fd_set fds;
	int n;
	struct timeval tv;
	// Functions
	void start_connection()
	{
		printf("TCP Connection - starting...\n");
		memset(recvbuf, '\0', BUFLEN);
		memset(sendbuf, '\0', BUFLEN);
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != NO_ERROR)
		{
			printf("TCP WSAStartup failed with error: %d\n", iResult);
		}
		// Create a SOCKET for connecting to server
		ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("TCP socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			error = true;
		}
		// The sockaddr_in structure specifies the address family,
		// IP address, and port of the server to be connected to.
		clientService.sin_family = AF_INET;
		clientService.sin_addr.s_addr = inet_addr(SERVERc); //inet_addr("172.31.1.200");
		clientService.sin_port = htons(PORTn);
		// Connect to server.
		iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
		if (iResult == SOCKET_ERROR)
		{
			printf("TCP connect failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			error = true;
			WSACleanup();
		}
		else
		{
			char* ip = inet_ntoa(clientService.sin_addr);
			int porti = PORTn;
			printf("TCP Client running on IP %s, port %d\n", ip, porti);
			//Set message for sending
			strcpy(sendbuf, "SCREEN;STATUS;1");
		}
		// Set up the struct timeval for the timeout.
		tv.tv_sec = 0;
		tv.tv_usec = 100;
		FD_ZERO(&fds);
		FD_SET(ConnectSocket, &fds);
	} //void start

	void update()
	{
		// Send data
		if (display)
		{
			printf("TCP Sending: %s\n", sendbuf);
		}
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR)
		{
			printf("TCP send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			error = true;
		}
		// Wait until timeout or data received.
		if (display)
		{
			printf("TCP Waiting message\n");
		}
		n = select(ConnectSocket, &fds, NULL, NULL, &tv);
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0) || (n == -1))
		{
			if (n == 0)
			{
				printf("TCP Timeout\n");
			}
			error = true;
			Sleep(3000);
		}
		if (!error)
		{
			// Receive data
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0 && display)
			{
				printf("TCP Received: %s\n", recvbuf);
			}
			else if (iResult == 0 && display)
			{
				printf("TCP Connection closed\n");
			}
			else if (display)
			{
				printf("TCP recv failed with error: %d\n", WSAGetLastError());
				error = true;
			}
		}

	} // void get

	void end_connection()
	{
		// shutdown the connection since no more data will be sent
		iResult = shutdown(ConnectSocket, SD_SEND);
		if (iResult == SOCKET_ERROR)
		{
			printf("TCP shutdown failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
		}
		// close the socket
		iResult = closesocket(ConnectSocket);
		if (iResult == SOCKET_ERROR)
		{
			printf("TCP close failed with error: %d\n", WSAGetLastError());
			WSACleanup();
		}
		// Receive until the peer closes the connection
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		do
		{
			if (iResult == 0)
			{
				printf("TCP Connection closed\n");
			}
			//else if (iResult > 0) {printf("Bytes received: %d\n", iResult);}
			iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		} while (iResult > 0);

		WSACleanup();
	} // void end
};
//----------------- Functions headers -------------------------
// Emulate UDP
void runUDP_30();
// Emulate TCP
bool TCP_decode(float &value1, float &value2, float &value3, char *message);
void keyboard();
void runTCP();
//-------------------------------------------------------------
//process
bool end_programme = false;
int one_to_escape;

int main(void)
{
	// runTCP();
	runUDP_30();

	return 0;
}

void keyboard()
{
	int ch;
	ch = _getch();
	ch = toupper(ch);
	printf("---> Key pressed: %c <---\n", ch);
	switch (ch)
	{
	case '0':
		end_programme = true;
		break;
// ------------------- TCP -----------------------
	case 'A':
		stimulator_code = Move3_ramp_less;
		break;
	case 'D':
		stimulator_code = Move3_ramp_more;
		break;
	case 'M':
		// Modify threshold
		recorder_code = Inge_decr;
		break;
	case 'P':
		// Modify threshold
		recorder_code = Inge_incr;
		break;
	case 'R':
		// Add more repetitions
		rep_nr++;
		break;
	case 'S':
		stimulator_code = Move3_decr;
		break;
	case 'T':
		// Reduce repetitions
		rep_nr--;
	case 'W':
		stimulator_code = Move3_incr;
		break;
// ------------------- UDP -----------------------
	// Two of the values had to be assigned to letters 
	// because there were no enough numbers
	case 'X':
		msgList.status = exDone;
		break;
	case 'I':
		msgList.status = msg_invalid;
		break;
	}
	if (ch >= '1' && ch <= '9')
	{
		int temp = (int)ch - '0';
		msgList.status = (tcp_msg_Type) temp;
		printf(" status = %d", msgList.status);
	}
}

bool TCP_decode(float &value1, float &value2, float &value3, char *message)
{
	int limit = (int)strlen(message);
	char format_start[4] = "SAS";
	char format_delimiter = ';';
	bool start_valid = true, delimiter_valid = true, numbers_valid = true;
	// Code stuff here:
	// 1. Verify the message starts with "SAS"
	// 2. Verify the position of the delimiters ';'
	// 3. Convert the rest of the message to float numbers
	value1 = 10.4; //Example values
	value2 = 3;
	value3 = 2.5;

	bool valid_msg = start_valid && delimiter_valid && numbers_valid;
	return valid_msg;
}

void runTCP()
{
	// TCP Connection variables
	TCPClient SAS;
	char message[BUFLEN];
	memset(message, '\0', BUFLEN);

	char ROB_STATUS = 'R';
	// for decoding
	bool decode_successful;
	float current = 0;
	float ramp = 0;
	float gain = 0;

	bool dummy_address = false;
	printf("=============== SCREEN TCP settings ===============\n");
	printf("1- 127.0.0.1 windows default private address for local testing.\n");
	printf("2- 172.31.1.147 ROBERT's controller address, for remote testing.\n");
	printf("3- 172.31.1.200 This computers' Ethernet address, for remote testing inside the same computer.\n");
	printf("---> Please select an IP address to connect: ");
	int option;
	do
	{
		option = _getch();
		option = toupper(option);
		dummy_address = (option == '1') || (option == '2') || (option == '3');
		printf("%c ", option);
	} while (!dummy_address);
	switch (option)
	{
	case '1':
		strcpy(SAS.SERVERc, "127.0.0.1");
		break;
	case '2':
		strcpy(SAS.SERVERc, "172.31.1.147");
		break;
	case '3':
		strcpy(SAS.SERVERc, "172.31.1.200");
		break;
	}
	std::cout << "selected addres = " << SAS.SERVERc << endl;
	//strcpy(SAS.SERVERc, "127.0.0.1"); // <- This is just a local IP address for testing
	printf("=========== SCREEN emulation controllers ===========\n");
	std::cout << "---> RehaMove3 controllers:\n-A = Reduce Ramp.\n-D = Increase ramp.\n-W = Increase current.\n-S = Decrease current.\n\n";
	std::cout << "---> RehaIngest controllers:\n-P = Increase threshold gain.\n-M = Decrease threshold gain.\n\n";
	std::cout << "---> Repetitions:\n-R = Add 1 (+1).\n-T = Reduce 1 (-1).\n\n";
	printf("- Key Number 0: finish program.\n");
	printf("===================================================\n");
	SAS.start_connection();

	printf("---> SCREEN start <---\n");
	stimulator_code = Move3_none;
	recorder_code = Inge_none;

	do
	{
		// Check if a key was pressed
		one_to_escape = _kbhit();
		if (one_to_escape != 0)
		{
			keyboard();
			// Send message
			// memset(message, '\0', BUFLEN);
			// sprintf(message, "SCREEN;%d;%d;%c%d;", stimulator_code, recorder_code, ROB_STATUS, rep_nr);
			// strcpy(SAS.sendbuf, message);
			// // Receive response
			// SAS.update();
			// strcpy(message,SAS.recvbuf);
			// decode_successful = TCP_decode(current, ramp, gain, message);
			// if (decode_successful){
			// 	printf("TCP Screen received a message.\n");
			// 	//printf("Values received: current = %2.1f, ramp = %1.1f, gain = %1.1f\n", current, ramp, gain);
			// }else{
			// 	printf("TCP received message not valid.\n");
			// }
			printf("\n");
		}
		memset(message, '\0', BUFLEN);
		sprintf(message, "SCREEN;%d;%d;%c%d;", stimulator_code, recorder_code, ROB_STATUS, rep_nr);
		stimulator_code = Move3_none;
		recorder_code = Inge_none;
		strcpy(SAS.sendbuf, message);
		// Receive response
		SAS.update();
		strcpy(message, SAS.recvbuf);
		decode_successful = TCP_decode(current, ramp, gain, message);
		Sleep(10);

	} while (!end_programme);

	// End TCP
	memset(message, '\0', BUFLEN);
	sprintf(message, "ENDTCP");
	strcpy(SAS.sendbuf, message);
	// Send-receive message
	SAS.update();

	printf("---> SCREEN end <---\n");
	SAS.end_connection();
}

void runUDP_30()
{
	// Start up
	char SCREEN_ADDRESS[15] = "127.0.0.1";
	uint32_t SCREEN_PORT = 30002;
	UdpClient touchPanel(SCREEN_ADDRESS, SCREEN_PORT);
	
	int index = 0;
	msgList.status = msg_none;

	touchPanel.display = true; //Chosen not to show messages during messages exchange
	do
	{
		touchPanel.start();
	} while (touchPanel.error);
	touchPanel.display = true;

	// run loop
	while (!end_programme)
	{
		// Get command from keyboard and send it to the SAS program
		printf("Press a key! (From 1-9, X and I)\n");
		keyboard();
		printf("\n");

		index = (int)msgList.status;
		//cout << "Selected : " << msgList.messages[index] << endl;
		sprintf(touchPanel.message, "%s", msgList.messages[index].c_str());
		touchPanel.get();

	}

	// end
	sprintf(touchPanel.message, "%s", msgList.messages[finish].c_str());
	touchPanel.get();

	touchPanel.end();
}