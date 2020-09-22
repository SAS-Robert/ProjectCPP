/** Library for RehaMove3 and RehaIngest devices Connection.
  * Including UDP Client and Server definitions.
  *
*/

#ifndef SAS_H_ // Include guards
#define SAS_H_
#include <iostream>
//UPD connection settings
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#define SERVER "172.31.1.147" //Robert IP address
#define BUFLEN 256//512	//Max length of buffer
#define PORT 30007 //30008 //Robert's port on which to listen for incoming data

using namespace std;

typedef struct{
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	fd_set fds ;
	int n ;
	struct timeval tv ;
	// New Variables
	bool error = false;
	uint32_t PORTn = PORT;
	bool display = true;
	char SERVERc[15] = "172.31.1.147";
	//Functions
	void start(){
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
		memset((char*)& si_other, 0, sizeof(si_other));
		si_other.sin_family = AF_INET;
		//si_other.sin_port = htons(PORT);
		//si_other.sin_addr.S_un.S_addr = inet_addr(SERVER);
		si_other.sin_port = htons(PORTn);
		si_other.sin_addr.S_un.S_addr = inet_addr(SERVERc);
		// Set up the struct timeval for the timeout.
		tv.tv_sec = 3 ;
		tv.tv_usec = 0 ;
		FD_ZERO(&fds) ;
		FD_SET(s, &fds) ;
		char *ip = inet_ntoa(si_other.sin_addr);
		std::cout<<"Client running on IP "<<ip<<", port "<<PORTn<<endl;
		//send a "hello!" so the server can start sending stuff
		strcpy(message, "hello!");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*) & si_other, slen) == SOCKET_ERROR)
		{
			error = true;
		}
	};
	void get(){
		memset(buf, '\0', BUFLEN);
		if(display){printf("UPD: Requesting status...\n");}
		strcpy(message, "RobotStateInformer;Ping;1");
		if (sendto(s, message, strlen(message), 0, (struct sockaddr*) & si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			error = true;
		}
		// Wait until timeout or data received.
		n = select ( s, &fds, NULL, NULL, &tv ) ;
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0)||(n==-1))
		{
			error = true;
			Sleep(3000);
		}
		if(!error){
			int length = sizeof(inet_addr(SERVER));
			recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*) & si_other, &slen);
			if(display){puts(buf);}
		}
	};
	void end(){
		closesocket(s);
		WSACleanup();
		if(display){ printf("UDP Connection closed\n");}
	};
}UDPClient;

typedef struct{
	SOCKET s;
	struct sockaddr_in server, si_other;
	int slen, recv_len;
	char buf[BUFLEN];
	WSADATA wsa;
	uint32_t PORTn = 30002;
	char SERVERc[15] = "172.31.1.147"; //Local IP address
	fd_set fds ;
	int n ;
	struct timeval tv ;
	bool error = false;
	//functions
	void start(){
		slen = sizeof(si_other);
		//Initialise winsock
		printf("\nInitialising Winsock...");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		printf("Initialised.\n");

		//Create a socket
		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
		}
		printf("Socket created.\n");

		//Prepare the sockaddr_in structure
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = INADDR_ANY;
		server.sin_addr.S_un.S_addr = inet_addr(SERVERc);
		server.sin_port = htons(PORTn);
		// Set up the struct timeval for the timeout.
		tv.tv_sec = 3 ;
		tv.tv_usec = 0 ;
		FD_ZERO(&fds) ;
		FD_SET(s, &fds) ;
		char *ip = inet_ntoa(server.sin_addr);
		std::cout<<"Running server on IP "<<ip<<", port "<<PORTn<<endl;
		//Bind
		if (bind(s, (struct sockaddr*) & server, sizeof(server)) == SOCKET_ERROR)
		{
			printf("Bind failed with error code : %d", WSAGetLastError());
			exit(EXIT_FAILURE);
		}
		puts("Bind done");
		//-------------------------------------------------
		fflush(stdout);
		printf("Waiting for a client connection...");
		fflush(stdout);
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		printf("\n========== sending data==========\n");
	};
	void stream(){
		printf("Waiting for request...");
		fflush(stdout);
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		// Wait until timeout or data received.
		n = select ( s, &fds, NULL, NULL, &tv ) ;
		// Re-editar esto para que no muestre el mensaje
		if ((n == 0)||(n==-1))
		{
			error = true;
			Sleep(1000);
		}
		if(!error){
			int length = sizeof(inet_addr(SERVER));
			recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*) & si_other, &slen);
			//print details of the client/peer and the data received
			printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			printf("Data: %s\n", buf);
		}
		//-------------------------------------------------
		memset(buf, '\0', BUFLEN);
		strcpy(buf, "IsMoving = 1, Stop =  0");
		//strftime(buf, sizeof(buf),"IsMoving=1");
		printf("Sending: %s\n", buf);
		//-------------------------------------------------
		//now reply the client with the same data
		//if (sendto(s, buf, recv_len, 0, (struct sockaddr*) & si_other, slen) == SOCKET_ERROR)
		if (sendto(s, buf, BUFLEN, 0, (struct sockaddr*) & si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}
		//Sleep(10);
	};
	void end(){
		closesocket(s);
		WSACleanup();
	};
}UDPServer;

// Others
void generate_date(char* outStr);

// From original HASOMED examples
void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init);
void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update);
void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data);
void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init);
void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module);
void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleGetAckReceived(Smpt_device* const device, const Smpt_ack& ack);

#endif // SAS_H_
