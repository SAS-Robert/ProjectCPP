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

typedef enum
{
    Move3_none      	= 0,    // Nothing to do
    Move3_incr     		= 1,    // Increment current
    Move3_decr      	= 2,    // Reduce current
    Move3_ramp_more   = 3,    // Increase ramp
    Move3_ramp_less		= 4,    // Reduce ramp
}RehaMove3_Req_Type;

typedef enum
{
    Inge_none      	  = 0,    // Nothing to do
    Inge_incr     		= 1,    // Increase threshold gain
    Inge_decr       	= 2,    // Decrease threshold gain
}RehaIngest_Req_Type;

typedef enum
{
    st_init      	    = 0,    // Nothing to do
    st_wait     		  = 1,    // Waiting for RMS_EMG>threshold
    st_running       	= 2,    // Stimulating
    st_stop           = 3,    // Done 1 seq, waiting for next
}state_Type;


typedef struct{
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);
	char buf[BUFLEN];
	char message[BUFLEN];
	WSADATA wsa;
	fd_set fds ;
	int n ;
	struct timeval tv ;
	bool error = false;
	uint32_t PORTn = PORT;
	bool display = true;
	char SERVERc[15] = "172.31.1.147";
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
		// For afterwards:
		strcpy(message, "RobotStateInformer;Status;1");
	};
	void get(){
		memset(buf, '\0', BUFLEN);
		//if(display){printf("UPD: Requesting status...\n");}
		//strcpy(message, "RobotStateInformer;Ping;1");
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
			if (display) { std::cout << "UDP Received: " << buf << endl; }// puts(buf);

			// Decode message
      int comp = 0;
	  char temp_c;
      for(int j=0; j<=18; j++){
        if((j>=0)&&(j<=5)){
          temp_c = start_msg[j];      // Start message
        }else if((j==6)||(j==12)){
          temp_c = delim_msg;         // Delimiters between fields
        }else if((j>=7)&&(j<=10)){
          temp_c = field1[j-7];       // isMoving field
        }else if((j>=13)&&(j<=16)){
          temp_c = field2[j-13];      // endPointReached field
        }else if(j==18){
          temp_c = end_msg;           // End message
        }else{
          temp_c = 0;
        }

		if ((j != 11) && (j != 17)) { // No comparing value fields
			comp = comp + (buf[j] - temp_c);
		}
      }
      bool valid1 = (buf[11]=='1')||(buf[11]=='0');
      bool valid2 = (buf[17]=='1')||(buf[17]=='0');
      valid_msg = (comp==0) && valid1 && valid2;
      //printf("RESULT: valid1 = %d, valid2 = %d, valid_msg = %d. ", valid1, valid2, valid_msg);
      // Booleans from Robert: only true if valid
      isMoving = valid_msg && (buf[11]=='1');
      Reached = valid_msg && (buf[17]=='1');
		  if (valid_msg && display) {
			  printf("UDP interpretation: isMoving = %d, Reached = %d\n", isMoving, Reached);
		  }
		  else if (display) {
			  printf("UDP message not valid\n");
		  }

		}
	};
	void end(){
		closesocket(s);
		WSACleanup();
		if(display){ printf("UDP Connection closed\n");}
	};
}UDPClient;

// Others
void generate_date(char* outStr);

// From original HASOMED examples
void fill_ml_init(Smpt_device* const device, Smpt_ml_init* const ml_init);
void fill_ml_update(Smpt_device* const device, Smpt_ml_update* const ml_update, Smpt_ml_channel_config values);
void fill_ml_get_current_data(Smpt_device* const device, Smpt_ml_get_current_data* const ml_get_current_data);
void fill_dl_init(Smpt_device* const device, Smpt_dl_init* const dl_init);
void fill_dl_power_module(Smpt_device* const device, Smpt_dl_power_module* const dl_power_module);
void handleInitAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handlePowerModuleAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleStopAckReceived(Smpt_device* const device, const Smpt_ack& ack);
void handleGetAckReceived(Smpt_device* const device, const Smpt_ack& ack);

#endif // SAS_H_
