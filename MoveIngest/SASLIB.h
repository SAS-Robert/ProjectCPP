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

// ------------------------------ Function hearders -----------------------------
void generate_date(char* outStr);
void get_dir(int argc, char *argv[], string& Outdir);

void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init);
void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update, Smpt_ml_channel_config values);
void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data);
void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init);
void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module);
bool handle_dl_packet_global(Smpt_device* const device, char* outStr, float& raw_data);
float handleSendLiveDataReceived(Smpt_device* const device, const Smpt_ack& ack, char* outStr);
void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleGetAckReceived(Smpt_device* const device, const Smpt_ack& ack);

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
typedef struct{
  const char* port_name_rm = "COM1";
  Smpt_device device = { 0 };
  Smpt_ml_init ml_init = { 0 };           // Struct for ml_init command *
  Smpt_ml_update ml_update = { 0 };       // Struct for ml_update command
  Smpt_ml_get_current_data ml_get_current_data = { 0 };
  UINT i = 0;
  bool smpt_port = false, smpt_check = false, smpt_next = false;
  bool smpt_end = false, smpt_get = false, tstart = false, ready = false;
  double diff;
  uint8_t packet;
  int turn_on = 0; //Time if the device gets turned on in the middle of the process
  Smpt_ml_channel_config stim;
  // From main:
  bool abort = false;

  void init(){
	  // Stimulation values
	  stim.number_of_points = 3;  //* Set the number of points
	  stim.ramp = 3;              //* Three lower pre-pulses
	  stim.period = 20;           //* Frequency: 50 Hz
	  // Set the stimulation pulse
	  stim.points[0].current = 5;
	  stim.points[0].time = 200;
	  stim.points[1].time = 100;
	  stim.points[2].current = -5;
	  stim.points[2].time = 200;
	printf("RehaMove3 message: Stimulation initial values -> current = %2.2f, ramp points = %d, ramp value = %d\n",stim.points[0].current,stim.number_of_points,stim.ramp);
	// Start Process
	printf("Reha Move3 message: Initializing device on port %s\n",port_name_rm);//<<port_name_rm<<endl;
	while(!smpt_next){
	  smpt_check = smpt_check_serial_port(port_name_rm);
	  smpt_port = smpt_open_serial_port(&device, port_name_rm);
	  // Request ID Data
	  packet = smpt_packet_number_generator_next(&device);
	  smpt_send_get_device_id(&device, packet);

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
	  if(!smpt_next){
		smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
		smpt_close_serial_port(&device);
		if(smpt_check){
		  std::cout << "Error - Reha Move3: Device does not respond. Turn it on or restart it.\n";
		  turn_on = 2500;
		}else{
		  std::cout << "Error - Reha Move3: Device not found. Check connection.\n";
		}
		Sleep(5000); // waits for 5 seconds to give you time to regret your life
		std::cout << "Reha Move 3 message: Retrying...\n";
		smpt_port = true;
	  }
	  if(abort){
		smpt_end = true;
		break; }
	}

	// Run Process
	if(!smpt_end){
	  Sleep(turn_on); // wait for it to be properly started
	  smpt_port = false;
	  fill_ml_init(&device, &ml_init);
	  smpt_send_ml_init(&device, &ml_init);
	  std::cout << "Reha Move3 message: Device ready.\n";
	  ready = true;
	}
  }; // void start

  void update(){
	fill_ml_update(&device, &ml_update, stim);
	smpt_send_ml_update(&device, &ml_update);
	while (i <= 600)
	{
	  fill_ml_get_current_data(&device, &ml_get_current_data);
	  smpt_send_ml_get_current_data(&device, &ml_get_current_data);
	  Sleep(1);
	  i++;
	}

	fill_ml_get_current_data(&device, &ml_get_current_data);
	smpt_send_ml_get_current_data(&device, &ml_get_current_data);
  }; // void update

  void end(){
  //  No need to repeat this, since the connectio was successfully stablished and
  // in case something went wrong, it'd get fixed on the next step
	  smpt_send_ml_stop(&device, smpt_packet_number_generator_next(&device));
	  if(!smpt_port){
		smpt_port = smpt_close_serial_port(&device);
		smpt_check = smpt_check_serial_port(port_name_rm); // it must be available after closing
	  }
	  if(smpt_check){std::cout << "Reha Move3 message: Process successfully finished.\n";}
  }; // void end
}RehaMove3_type;

typedef struct{
	const char* port_name_ri = "COM2";
	uint8_t packet_number = 0;
	Smpt_device device_ri = { 0 };
	Smpt_ml_init ml_init = { 0 };           // Struct for ml_init command
	char data[256];
	float raw_value = 0;
	//Process variables
	bool smpt_port = false, smpt_check = false, smpt_stop = false, smpt_next = false;
	bool smpt_end = false;
	int limit_samples = 400;
	double diff;
	bool abort = false, ready = false;
	bool data_received = false, data_start = false, data_printed = false;
	std::vector<float> channel_raw;

	int iterator = 0;

	void init(){ // intializes communication
	  // First step
	  std::cout <<"Reha Ingest message: Setting communication on port "<<port_name_ri<<endl;
	  while(!smpt_next){
		smpt_check = smpt_check_serial_port(port_name_ri);
		smpt_port = smpt_open_serial_port(&device_ri, port_name_ri);
		packet_number = smpt_packet_number_generator_next(&device_ri);
		smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
		smpt_next = smpt_check && smpt_port && smpt_stop;
		if(!smpt_next){
		  std::cout << "Error - Reha Ingest: Device not found. Turn it on and/or check connection.\n";
		  smpt_stop = smpt_send_dl_stop(&device_ri, packet_number);
			smpt_close_serial_port(&device_ri);
		  Sleep(5000); // waits for 5 seconds to give you time to regret your life
		  std::cout << "Reha Ingest message: Retrying...\n";
		}
		if(abort){
		  smpt_end = true;
		  break; }
	  }
	} // void init
	// 2nd-3rd-4th steps if the user has not stopped the process
	void start(){ // starts the device internal measurement unit
	  // Fourth step
	  // Clean the input buffer
	  while (smpt_new_packet_received(&device_ri))
	  {
		  handle_dl_packet_global(&device_ri, data, raw_value);
	  }
	  // Second step: enable device
	  std::cout << "Reha Ingest message: Enabling and initializing device...\n";
	  Smpt_dl_power_module dl_power_module = { 0 };
	  fill_dl_power_module(&device_ri, &dl_power_module);
	  smpt_send_dl_power_module(&device_ri, &dl_power_module);

	  // wait, because the enabling takes some time (normal up to 4ms)
	  Sleep(10);

	  while (smpt_new_packet_received(&device_ri))
	  {
		  handle_dl_packet_global(&device_ri, data, raw_value);
	  }
	  // Third step: initialize device
	  Smpt_dl_init dl_init = { 0 };
	  fill_dl_init(&device_ri, &dl_init);
	  smpt_send_dl_init(&device_ri, &dl_init);

	  Sleep(10);

	  while (smpt_new_packet_received(&device_ri))
	  {
		  handle_dl_packet_global(&device_ri, data, raw_value);
	  }
	  Sleep(10);
	  // Every step is needed for ini.
		//---------------------------------------------
	  // Waiting here for start from main:
	  // send measurement start cmd
	  packet_number = smpt_packet_number_generator_next(&device_ri);
	  smpt_send_dl_start(&device_ri, packet_number);
	  ready = true;
	  std::cout << "Reha Ingest message: device ready.\n";
	};// void start

	void record(){
	  // clean data variables before starting
	  data_received = false;
	  memset(data, '\0', 256);
	  while (smpt_new_packet_received(&device_ri))
	  {
		  data_received = handle_dl_packet_global(&device_ri, data, raw_value);
		  if (data_received && !data_start) { data_start = true; }
		  if(data_start && !data_printed){
			std::cout<< "Reha Ingest message: Data available at iteration number "<<iterator<<endl;
			std::cout << "Reha Ingest message: Recording data.\n";
			data_printed = true;
		  }

		  if (data_received && data_start) {
			channel_raw.push_back(raw_value);
		  }
	  }
	}; // void record
	void end(){
	  printf("Reha Ingest message: Ending process.\n");
	  packet_number = smpt_packet_number_generator_next(&device_ri);
	  smpt_port = smpt_send_dl_stop(&device_ri, packet_number);
	  smpt_port = smpt_close_serial_port(&device_ri);
	  smpt_check = smpt_check_serial_port(port_name_ri);
	}; // void end
}RehaIngest_type;

// -------------------- Communication  ----------------------
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
		tv.tv_sec = 1;
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
    error = false;
		memset(buf, '\0', BUFLEN);
		valid_msg = false;
		//if(display){printf("UPD: Requesting status...\n");}
		//strcpy(message, "RobotStateInformer;Ping;1");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
		{
			if(display){printf("sendto() failed with error code : %d", WSAGetLastError());}
			error = true;
		}
		// Wait until timeout or data received.
    FD_ZERO(&fds);
    FD_SET(s, &fds);
		n = select(s, &fds, NULL, NULL, &tv);
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0) || (n == -1))
		{
			//if(n==0){printf("Timeout\n");}
			error = true;
			//Sleep(10);
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
	bool error = false, new_message = false, finish = false;
  bool display = false;
  fd_set fds;
	int n;
	struct timeval tv;
  // Functions
	void start() {
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
    tv.tv_sec = 2;
    tv.tv_usec = 500;
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
    n = select(ClientSocket, &fds, NULL, NULL, &tv);
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
