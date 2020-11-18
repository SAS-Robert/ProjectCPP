/** Library for RehaMove3 and RehaIngest devices Connection.
  * Including UDP Client and Server definitions.
  *
*/

#ifndef SASLIB_H_ // Include guards
#define SASLIB_H_

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

#include "smpt_ml_client.h"
#include "smpt_dl_client.h"
#include "smpt_definitions.h"

//Connection settings
#pragma warning(disable : 5208)
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512
using namespace std;
//
static std::vector<double> recorder_emg1;

// ------------------------------ Function hearders -----------------------------
void generate_date(char* outStr);
void get_dir(int argc, char *argv[], string& Outdir);
bool UDP_decode(char* message, bool& value1, bool& value2);
string convertToString(char* a, int size);

void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init);
void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update, Smpt_ml_channel_config values);
void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data);
void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init);
void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module);
void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack);

// Modified from original Hasomeds examples:
static void handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack)
{
	Smpt_dl_send_live_data live_data;
	float values[5] = { 0 };
	smpt_get_dl_send_live_data(device, &live_data);

	for (int i = 0; i < live_data.n_channels; i++)
	{
		values[i] = live_data.electrode_samples[i].value;
	}
	values[4] = (float)live_data.time_offset;

	//value[0] : channel 1, bioimpedance measurement
	//value[1] : channel 2, emg 1 measurement
	//value[2] : channel 3, emg 2
	//value[3] : channel 4, analog signal.
	//value[4] : time_ofset between last sample and actual sample
	recorder_emg1.push_back((double)values[1]);
	//recorder_time.push_back((double)values[4]);
}

static bool handle_dl_packet_global(Smpt_device* const device)
{
	Smpt_ack ack;
	smpt_last_ack(device, &ack);
	Smpt_Cmd cmd = (Smpt_Cmd)ack.command_number;
	bool output = false;

	switch (cmd)
	{
	case Smpt_Cmd_Dl_Power_Module_Ack:
	{
		handlePowerModuleAckReceived(device, ack);
		break;
	}
	case Smpt_Cmd_Dl_Get_Ack:
	{
		//handleGetAckReceived(ack);
		break;
	}
	case Smpt_Cmd_Dl_Init_Ack:
	{
		handleInitAckReceived(device, ack);
		break;
	}
	case Smpt_Cmd_Dl_Start_Ack:
	{
		//handleStartAckReceived(ack);
		break;
	}
	case Smpt_Cmd_Dl_Stop_Ack:
	{
		handleStopAckReceived(device, ack);
		break;
	}
	case Smpt_Cmd_Dl_Send_Live_Data:
	{
		handleSendLiveDataReceived(device, ack);
		output = true;
		break;
	}
	default:
	{
		break;
	}
	}
	return output;
}
// ---------------------- Variables for process control ---------------------
typedef enum
{
    Move3_none      	= 0,    // Nothing to do
    Move3_incr     		= 1,    // Increment current
    Move3_decr      	= 2,    // Reduce current
    Move3_ramp_more   = 3,    // Increase ramp
    Move3_ramp_less		= 4,    // Reduce ramp
    Move3_stop        = 5,    // Stop Stimulating
    Move3_start       = 6,    // Stimulate with initial values
}RehaMove3_Req_Type;

typedef enum
{
    Inge_none      	  = 0,    // Nothing to do
    Inge_incr     		= 1,    // Increase threshold gain
    Inge_decr       	= 2,    // Decrease threshold gain
}RehaIngest_Req_Type;

typedef enum
{
    pause       = 'P',    // 'P' = pause by user-request
    rep      	  = 'R',    // 'R' = repetition active
    start       = 'S',    // 'S' = start of repetition
}ROB_Type;

typedef enum
{
    st_init      	    = 0,    // Nothing to do
    st_th       		  = 1,    // set threshold
    st_wait           = 2,    // Waiting to overcome threshold
    st_running       	= 3,    // Stimulating
    st_stop           = 4,    // Done 1 seq, waiting for next
    st_end            = 5,    // Setting threshold
    st_test           = 6,    // Run stimulation test
}state_Type;

// ------------------------------ Devices -----------------------------
// Stimulator
class RehaMove3 {
private:
	char* port_name_rm;
	Smpt_device device;
	Smpt_ml_init ml_init;           // Struct for ml_init command *
	Smpt_ml_get_current_data ml_get_current_data;
	uint8_t packet;
	int turn_on = 0; //Time if the device gets turned on in the middle of the process
	bool smpt_port, smpt_check, smpt_next, smpt_end, smpt_get;

public:
    Smpt_ml_update ml_update;       // Struct for ml_update command
    bool ready;
    Smpt_ml_channel_config stim;
    // From main:
    bool abort;

	// Constructor
	RehaMove3(char* port) {
		port_name_rm = port;
		device = { 0 };
		ml_init = { 0 };
		ml_get_current_data = { 0 };
		smpt_port = false;
		smpt_check = false;
		smpt_next = false;
		smpt_end = false;
		smpt_get = false;
		ready = false;
		abort = false;
	}
	// Destructor
	//~RehaMove3() = delete;
  // Functions
	void init() {
		// Stimulation values
		stim.number_of_points = 3;  //* Set the number of points
		stim.ramp = 3;              //* Three lower pre-pulses
		stim.period = 20;           //* Frequency: 50 Hz
		// Set the stimulation pulse
		stim.points[0].current = 0;
		stim.points[0].time = 200;
		stim.points[1].time = 200;
		stim.points[2].current = 0;
		stim.points[2].time = 200;
		// Start Process
		printf("Reha Move3 message: Initializing device on port %s... ", port_name_rm);//<<port_name_rm<<endl;
		while (!smpt_next) {
			smpt_check = smpt_check_serial_port(port_name_rm);
			smpt_port = smpt_open_serial_port(&device, port_name_rm);
			// Request ID Data
			packet = smpt_packet_number_generator_next(&device);
			smpt_send_get_device_id(&device, packet);
			//
			fill_ml_init(&device, &ml_init);
			smpt_send_ml_init(&device, &ml_init);
			fill_ml_update(&device, &ml_update, stim);
			smpt_send_ml_update(&device, &ml_update);
			fill_ml_get_current_data(&device, &ml_get_current_data);
			// This last command check if it's received all the data requested
			smpt_get = smpt_send_ml_get_current_data(&device, &ml_get_current_data);
			//std::cout << "Before while: get="<<smpt_get << endl;

			// smpt_next = go to next step -> Process running
			smpt_next = smpt_check && smpt_port && smpt_get;
			if (!smpt_next) {
				smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
				smpt_close_serial_port(&device);
				if (smpt_check) {
					std::cout << "\nError - Reha Move3: Device does not respond. Turn it on or restart it.\n";
					turn_on = 2500;
				}
				else {
					std::cout << "\nError - Reha Move3: Device not found. Check connection.\n";
				}
				Sleep(5000); // waits for 5 seconds to give you time to regret your life
				std::cout << "\nReha Move 3 message: Retrying... ";
				smpt_port = true;
			}
			if (abort) {
				smpt_end = true;
				break;
			}
		}

		// Run Process
		if (!smpt_end) {
			Sleep(turn_on); // wait for it to be properly started
			smpt_port = false;
			fill_ml_init(&device, &ml_init);
			smpt_send_ml_init(&device, &ml_init);
			//std::cout << "Reha Move3 message: Device ready.\n";
			ready = true;
		}

		// Update values
		stim.number_of_points = 3;  //* Set the number of points
		stim.ramp = 3;              //* Three lower pre-pulses
		stim.period = 20;           //* Frequency: 50 Hz
		// Set the stimulation pulse

		stim.points[0].current = 2.0;
		stim.points[0].time = 200;
		stim.points[1].time = 200;
		stim.points[2].current = -2.0;
		stim.points[2].time = 200;
		//printf("RehaMove3 message: Stimulation initial values -> current = %2.2f, ramp points = %d, ramp value = %d\n", stim.points[0].current, stim.number_of_points, stim.ramp);

		printf("Device RehaMove3 ready.\n");
	};
	void update() {
		fill_ml_update(&device, &ml_update, stim);
		smpt_send_ml_update(&device, &ml_update);

		fill_ml_get_current_data(&device, &ml_get_current_data);
		smpt_send_ml_get_current_data(&device, &ml_get_current_data);
	};
	void pause() {
		// Option 1: setting current value = 0 and update it
		// stim.number_of_points = 3;
		// stim.ramp = 3;
		// stim.period = 20;
		// stim.points[0].current = 0;
		// stim.points[0].time = 200;
		// stim.points[1].time = 200;
		// stim.points[2].current = -0;
		// stim.points[2].time = 200;
		//
		// fill_ml_update(&device, &ml_update, stim);
		// smpt_send_ml_update(&device, &ml_update);

		// Option 2: using stop comand and restart it?
		smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
		fill_ml_init(&device, &ml_init);
		smpt_send_ml_init(&device, &ml_init);
	};
	void end() {
		//  No need to repeat this, since the connectio was successfully stablished and
		// in case something went wrong, it'd get fixed on the next step
		smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
		if (!smpt_port) {
			smpt_port = smpt_close_serial_port(&device);
			smpt_check = smpt_check_serial_port(port_name_rm); // it must be available after closing
		}
		ready = false;
		if (smpt_check) { std::cout << "Reha Move3 message: Process finished.\n"; }
	};
};

// Recorder
class RehaIngest {
private:
	const char* port_name_ri;
	uint8_t packet_number = 0;
	Smpt_device device_ri = { 0 };
	Smpt_ml_init ml_init = { 0 };           // Struct for ml_init command
	//Process variables
	bool smpt_port, smpt_check, smpt_stop, smpt_next, smpt_end;
public:
	bool abort, ready;
	bool data_received, data_start, data_printed;

	// Constructor
	RehaIngest(char* port) {
		port_name_ri = port;
		device_ri = { 0 };
		ml_init = { 0 };
		packet_number = 0 ;
		smpt_port = false;
		smpt_check = false;
		smpt_next = false;
		smpt_end = false;
		smpt_stop = false;
		data_received = false;
		data_start = false;
		data_printed = false;
		ready = false;
		abort = false;
	}
	// Functions
	void init() {
		// First step
		std::cout << "Reha Ingest message: Setting communication on port " << port_name_ri << "... ";
		while (!smpt_next) {
			smpt_check = smpt_check_serial_port(port_name_ri);
			smpt_port = smpt_open_serial_port(&device_ri, port_name_ri);
			packet_number = smpt_packet_number_generator_next(&device_ri);
			smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
			smpt_next = smpt_check && smpt_port && smpt_stop;
			if (!smpt_next) {
				std::cout << "Error - Reha Ingest: Device not found. Turn it on and/or check connection.\n";
				smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
				smpt_close_serial_port(&device_ri);
				Sleep(5000); // waits for 5 seconds to give you time to regret your life
				std::cout << "Reha Ingest message: Retrying... ";
			}
			if (abort) {
				smpt_end = true;
				break;
			}
		}
		std::cout << "Device RehaIngest found." << endl;
	};
	void start() {
		// Fourth step
		// Clean the input buffer
		while (smpt_new_packet_received(&device_ri))
		{
			handle_dl_packet_global(&device_ri);
		}
		// Second step: enable device
		std::cout << "Reha Ingest message: Enabling and initializing device... ";
		Smpt_dl_power_module dl_power_module = { 0 };
		fill_dl_power_module(&device_ri, &dl_power_module);
		smpt_send_dl_power_module(&device_ri, &dl_power_module);

		// wait, because the enabling takes some time (normal up to 4ms)
		Sleep(10);

		while (smpt_new_packet_received(&device_ri))
		{
			handle_dl_packet_global(&device_ri);
		}
		// Third step: initialize device
		Smpt_dl_init dl_init = { 0 };
		fill_dl_init(&device_ri, &dl_init);
		smpt_send_dl_init(&device_ri, &dl_init);

		Sleep(10);

		while (smpt_new_packet_received(&device_ri))
		{
			handle_dl_packet_global(&device_ri);
		}
		Sleep(10);
		// Every step is needed for ini.
		//---------------------------------------------
		// Waiting here for start from main:
		// send measurement start cmd
		packet_number = smpt_packet_number_generator_next(&device_ri);
		smpt_send_dl_start(&device_ri, packet_number);
		ready = true;
		std::cout << "Device ready.\n";
	};
	void record() {
		// clean data variables before starting
		data_received = false;
		while (smpt_new_packet_received(&device_ri))
		{
			data_received = handle_dl_packet_global(&device_ri);
			if (data_received && !data_start) {
				data_start = true;

			}
			// if (data_start && !data_printed) {
			// 	std::cout << "Reha Ingest message: Recording data.\n";
			// 	data_printed = true;
			// }

		}
	};
	void end() {
		packet_number = smpt_packet_number_generator_next(&device_ri);
		smpt_port = smpt_send_dl_stop(&device_ri, packet_number);
		Sleep(10);
		while (smpt_new_packet_received(&device_ri))
		{
			handle_dl_packet_global(&device_ri);
		}
		smpt_port = smpt_close_serial_port(&device_ri);
		smpt_check = smpt_check_serial_port(port_name_ri);
		ready = false;
		printf("Reha Ingest message: Process finished.\n");
	};
};

// -------------------- Communication  ----------------------
typedef struct {
private:
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	WSADATA wsa;
	fd_set fds;
	int n;
	uint32_t PORTn;
	char SERVERc[15]; // IP address

public:
	char buf[BUFLEN];
	char message[BUFLEN];
	struct timeval timeout;
	bool error = false;
	bool display = true;
	// Robot variables
	bool isMoving = false;
	bool Reached = false;
	bool valid_msg = false;

	//Functions
	void start(char* S_address, uint32_t port) {
		strcpy(SERVERc, S_address);
		PORTn = port;

		//Initialise winsock
		if(display){printf("UPD Connection - starting...\n");}
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			error = true;
		}
		//create socket
		if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
		{
			if(display){printf("socket() failed with error code : %d", WSAGetLastError());}
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
		if (!error) { std::cout << "UDP Client running on IP " << ip << ", port " << PORTn << endl; }
		//send a start message so the server can start sending stuff
		strcpy(message, "0;STATUS");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			error = true;
		}
	};
	void get() {
    error = false;
		memset(buf, '\0', BUFLEN);
		valid_msg = false;
		if(display){printf("UPD: Requesting status...");}
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			if(display){printf("sendto() failed with error code : %d", WSAGetLastError());}
			error = true;
		}
		// Wait until timeout or data received.
    FD_ZERO(&fds);
    FD_SET(s, &fds);
		n = select(s, &fds, NULL, NULL, &timeout);
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0) || (n == -1))
		{
			if(n==0){printf("Timeout\n");}
			else { printf("Error while receiving.\n"); }
			error = true;
		}
		if (!error) {
			int length = sizeof(SERVERc);
			recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);
	  // Decode message here
     valid_msg = UDP_decode(buf, isMoving, Reached);
     if(display && valid_msg){ std::cout<<"UDP Received: isMoving="<<isMoving<<",Reached="<<Reached<<endl;}
     if(display && !valid_msg){ std::cout<<"UDP message not valid"<<endl;}
     //error = !error;
		}
	};
	void end() {
		closesocket(s);
		WSACleanup();
		if (display) { printf("UDP Connection closed\n"); }
	};
}UDPClient;

typedef struct {
private:
	// Communication variables
	int iResult;
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in clientService;
	int recvbuflen = BUFLEN;
	// Control variables
	bool error = false, display = true;
	fd_set fds;
	int n;

public:
	char sendbuf[BUFLEN];// = "Client: sending data test";
	char recvbuf[BUFLEN];// = "";
	char SERVERc[15];
	int PORTn = 702;
	struct timeval tv; 	// timeout settings
	// Functions
	void start(char* S_address, uint32_t port) {
		strcpy(SERVERc, S_address);
		PORTn = port;

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
		tv.tv_sec = 1;
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
		if (display) { printf("TCP Waiting repsonse\n"); }
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

typedef struct {
private:
	WSADATA wsaData;
	int iResult;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL;
	struct addrinfo hints;
	int iSendResult;
	fd_set fds;
	int n;
	int senbuflen = BUFLEN;
	int recvbuflen = BUFLEN;
	char PORTc[15];

public:
	char recvbuf[BUFLEN];
	char senbuf[BUFLEN];
	// New
	bool error = false, new_message = false, finish = false;
	bool display = false;
	struct timeval timeout;
  // Functions
	void start(char* S_port) {
		strcpy(PORTc, S_port);
		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			if(display){printf("WSAStartup failed with error: %d\n", iResult);}
			error = true;
		}
		else if (display) {
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
			if(display){printf("TCP getaddrinfo failed with error: %d\n", iResult);}
			WSACleanup();
			error = true;
		}

		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			if(display){printf("TCP socket failed with error: %ld\n", WSAGetLastError());}
			freeaddrinfo(result);
			WSACleanup();
			error = true;
		}
		else if (display) {
			printf("TCP Socket created.\n");
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			if(display){printf("TCP bind failed with error: %d\n", WSAGetLastError()); }
			freeaddrinfo(result);
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else if (!error && display) {
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
			if(display){printf("listen failed with error: %d\n", WSAGetLastError());}
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else { //if (display)
			printf("TCP Listening started on port %s. Waiting for a client.\n", PORTc);
		}
		// Accept a client socket
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			if(display){printf("TCP client accept failed with error: %d\n", WSAGetLastError());}
			closesocket(ListenSocket);
			WSACleanup();
			error = true;
		}
		else if (!error && display) {
			printf("A TCP client has been accepted.\n");
		}
		// No longer need server socket
		closesocket(ListenSocket);
		// Set up the struct timeval for the timeout.
		timeout.tv_sec = 2;
		timeout.tv_usec = 500;
		FD_ZERO(&fds);
		FD_SET(ClientSocket, &fds);
	} // void waiting

  void check() {
    memset(recvbuf, '\0', BUFLEN);
    memset(senbuf, '\0', BUFLEN);
    new_message = false;
    error = false;
    FD_ZERO(&fds);
    FD_SET(ClientSocket, &fds);
    // Necessary to restart here the timer
    n = select(ClientSocket, &fds, NULL, NULL, &timeout);
    // Re-editar esto para que no muestre el mensaje
    if ((n == 0) || (n == -1))
    {
      error = true;
      if (n == 0 && display) { printf("TCP Timeout\n"); }
      //Sleep(3000);
    }
    if (!error) {
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
  		if (iResult > 0) {
  			if(display){printf("TCP Data received: %s\n", recvbuf);}
        new_message = true;
  		}
  		else if (iResult == 0 && display)
  			printf("TCP timeout\n");
  		else if(display) {
  			printf("TCPrecv failed with error: %d\n", WSAGetLastError());
			  error = true;
  		}
    }
    //Decode after this
	} // void check

	void stream() {
			iSendResult = 0;
			//strcpy(senbuf, "SAS;10.4;3;2.5;"); // New format
			if(display){ printf("TCP sending: %s\n", senbuf); }
			iSendResult = send(ClientSocket, senbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR && display) {
				printf("TCP send failed with error: %d\n", WSAGetLastError());
				finish = true;
			}
	} // void stream

	void end() {
		// shutdown the connection since we're done
		iResult = shutdown(ClientSocket, SD_SEND);
		if (iResult == SOCKET_ERROR && display) {
			printf("TCP shutdown failed with error: %d\n", WSAGetLastError());
		}
		else if (display) {
			printf("TCP Connection closed.\n");
		}
		// cleanup
		closesocket(ClientSocket);
		WSACleanup();
	} // void end
}TCPServer;


#endif // SASLIB_H_
