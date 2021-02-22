/** Library for UDP and TCP functions and communication
  *
*/

#ifndef SASLIBcom_H_
#define SASLIBcom_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#define _CRT_SECURE_NO_WARNINGS

#undef UNICODE

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iostream>
// ------------------------------------------------------------------------


// ------------------ Additional libraries ------------------
#include "SASLIBbasic.hpp"
#include "smpt_definitions.h"

//Connection settings
#pragma warning(disable : 5208)
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define BUFLEN 512
using namespace std;

// ------------------ Functions definition ------------------
bool decode_robot(char *message, bool &value1, bool &value2)
{
  char b_true[6] = "true";
  char b_false[6] = "false";
  int comp_t = 0, comp_f = 0, pos = 0, pos_cont = 0;
  bool valid1 = false, valid2 = false, valid_msg = false, b_v1 = false, b_v2 = false;
  // Decode 1st part of the message:
  for (int j = 0; j <= 3; j++)
  {
    comp_t = comp_t + message[j] - b_true[j];
    comp_f = comp_f + message[j] - b_false[j];
  }
  if ((comp_t == 0) || (comp_f == 0))
  {
    valid1 = true;
  }
  // Decode 2nd part of the message:
  if (comp_t == 0)
  {
    pos = 5;
    b_v1 = true;
  }
  else
  {
    pos = 6;
  }
  comp_t = 0;
  comp_f = 0;
  for (int j = 0; j <= 3; j++)
  {
    comp_t = comp_t + message[j + pos] - b_true[j];
    comp_f = comp_f + message[j + pos] - b_false[j];
  }
  if (comp_t == 0)
  {
    valid2 = true;
    b_v2 = true;
  }
  else if (comp_f == 0)
  {
    valid2 = true;
  }
  // Outputs:
  valid_msg = valid1 && valid2;

  if (valid_msg)
  {
    value1 = b_v1;
    value2 = b_v2;
  }
  return valid_msg;
}

bool decode_gui(char* message, RehaMove3_Req_Type& stimulator, User_Req_Type& user, ROB_Type& status, int& rep, bool& finished, Smpt_Channel& sel_ch, exercise_Type& sel_ex, threshold_Type& sel_th)
{
    int field = 0, nrFields = 8, length = strlen(message);
    string start_msg = "SCREEN";
    string finish_msg = "ENDTCP";
    char temp_status[2] = "0";
    bool valid[10], valid_msg = true;
    int value[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    string messageStr = convert_to_string(message, length);
    string delimiter = ";";
    size_t pos = 0;
    string token;
    // Separate every field on the string
        while ( ((pos = messageStr.find(delimiter)) != std::string::npos) && !finished) {
            token = messageStr.substr(0, pos);
            messageStr.erase(0, pos + delimiter.length());

            switch (field) {
                // Start message
            case 0:
                finished = (token == finish_msg);
                valid[field] = (token == start_msg);

                if (finished) {
                    return valid_msg;
                }
                break;
                // standard command options and repetitions
            case 1:
                value[field] = stoi(token);
                valid[field] = (value[field] >= 0) && (value[field] <= 9);
                break;
            case 2:
                value[field] = stoi(token);
                valid[field] = (value[field] >= 0) && (value[field] <= 8);
                break;
            case 3:
                strcpy(temp_status, token.c_str());
                valid[field] = (temp_status[0] >= 'A') && (temp_status[0] <= 'Z');
                break;
            case 4:
                value[field] = stoi(token);
                valid[field] = (value[field] >= 0) && (value[field] <= 99);
                break;
                // Select channel, exercise and method
            case 5:
                value[field] = stoi(token);
                valid[field] = (value[field] >= 0) && (value[field] <= 4);
                break;
            case 6:
                value[field] = stoi(token);
                valid[field] = (value[field] >= 0) && (value[field] <= 4);
                break;
            case 7:
                value[field] = stoi(token);
                valid[field] = (value[field] >= 0) && (value[field] <= 3);
                break;
            }

            field++;
        }
        // Check that all the fields are correct
        for (int k = 0; k < nrFields; k++)
        {
            valid_msg = valid_msg && valid[k];
        }

        if (valid_msg)
        {
            stimulator = (RehaMove3_Req_Type)value[1];
            user = (User_Req_Type)value[2];
            status = (ROB_Type)temp_status[0];
            rep = value[4];
            sel_ch = (Smpt_Channel)value[5];
            sel_ex = (exercise_Type)value[6];
            sel_th = (threshold_Type)value[7];
        }
        return valid_msg;
}

// ------------------ Objects definition ------------------
typedef struct UdpClient
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
    memset((char *)&si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(PORTn);
    si_other.sin_addr.S_un.S_addr = inet_addr(SERVERc);
    // Set up the struct timeval for the timeout.
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(s, &fds);
    char *ip = inet_ntoa(si_other.sin_addr);
    if (!error)
    {
      std::cout << "UDP Client running on IP " << ip << ", port " << PORTn << endl;
    }
    //send a start message so the server can start sending stuff
    strcpy(message, "0;STATUS");
    if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR)
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
      printf("UPD: Requesting status...");
    }
    if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR)
    {
      if (display)
      {
        printf("sendto() failed with error code : %d", WSAGetLastError());
      }
      error = true;
      error_cnt++;
    }
    // Wait until timeout or data received.
    FD_ZERO(&fds);
    FD_SET(s, &fds);
    n = select(s, &fds, NULL, NULL, &timeout);
    if ((n == 0) || (n == -1))
    {
      if (n == 0 && display)
      {
        printf("Timeout\n");
      }
      else if (display)
      {
        printf("Error while receiving.\n");
      }
      error = true;
      error_cnt++;
    }

    if (!error)
    {
      int length = sizeof(SERVERc);
      recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen);
      // Decode message here
      valid_msg = decode_robot(buf, isMoving, Reached);
      if (valid_msg)
      {
        error_cnt = 0;
        if (display)
        {
          std::cout << "UDP Received: isMoving=" << isMoving << ",Reached=" << Reached << endl;
        }
      }
      else
      {
        if (display)
        {
          std::cout << "UDP message not valid" << endl;
        }
        error_cnt++;
      }
    }
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

struct UdpServer
{
  private:
    uint32_t nPORT;
    SOCKET s;
    fd_set fds;
    struct sockaddr_in server, si_other;
    int slen, recv_len, n;
    WSADATA wsa;
    char SERVERc[15]; // IP address

  public:
    char recvbuf[BUFLEN];
    char senbuf[BUFLEN];
    bool error, new_message, finish, display, error_lim;
    struct timeval timeout;
    int error_cnt, ERROR_CNT_LIM;
    // constructor
    UdpServer(char *S_address, char *PORTc)
    {
      nPORT = 30001;
      slen = sizeof(si_other);
      display = false;
      nPORT = atoi(PORTc);
      strcpy(SERVERc, S_address);
      ERROR_CNT_LIM = 5;
  }
  // methods
  void start()
  {
    //Initialise winsock
    if(display) { printf("UDP Server - Initialising Winsock..."); }
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
      printf("UDP Server - Failed. Error Code : %d", WSAGetLastError());
      exit(EXIT_FAILURE);
    }
    printf("UDP Server - Initialised.\n");

    //Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
      printf("UDP Server - Could not create socket : %d", WSAGetLastError());
    }
    printf("UDP Server - Socket created.\n");

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_addr.S_un.S_addr = inet_addr(SERVERc);
    server.sin_port = htons(nPORT);
    char *ip = inet_ntoa(server.sin_addr);
    std::cout << "Running UDP Server on IP " << ip << ", port " << nPORT << endl;
    //Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
      printf("UDP Server - Bind failed with error code : %d\n", WSAGetLastError());
    }
    puts("UDP Server - Bind done");
    fflush(stdout);
    //clear the buffer by filling null, it might have previously received data
    memset(recvbuf, '\0', BUFLEN);
    memset(senbuf, '\0', BUFLEN);
    // Set up the struct timeval for the timeout.
    timeout.tv_sec = 0;
    timeout.tv_usec = 500;
    FD_ZERO(&fds);
    FD_SET(s, &fds);
  }

  // method with blocking call
  void check()
  {
      //clear the buffer(s)
      fflush(stdout);
      memset(recvbuf, '\0', BUFLEN);
      //try to receive some data, this is a blocking call
      if ((recv_len = recvfrom(s, recvbuf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen)) == SOCKET_ERROR)
      {
          if (display)
          {
              printf("recvfrom() failed with error code : %d\n", WSAGetLastError());
          }
          error = true;
          error_cnt++;
      } 
      else if (display)
      {
        printf("UDP Server received data: %s\n", recvbuf);
        error = false;
      }
      error_lim = error_cnt >= ERROR_CNT_LIM;
  }

  // method with timeout control
  /*
  void check()
  {
      // Wait until timeout or data received.
      FD_ZERO(&fds);
      FD_SET(s, &fds);
      n = select(s, &fds, NULL, NULL, &timeout);
      if ((n == 0) || (n == -1))
      {
          if (n == 0 && display)
          {
              printf("UDP Server - Timeout\n");
          }
          else if (display)
          {
              printf("UDP Server - Error while receiving.\n");
          }
          error = true;
          error_cnt++;
      }
      // Data has been received
      if (!error)
      {
          //clear the buffer(s)
          fflush(stdout);
          memset(recvbuf, '\0', BUFLEN);
          //try to receive some data, this is a blocking call
          recv_len = recvfrom(s, recvbuf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);
          //print details of the client/peer and the data received
          //printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
          if (display)
          {
              printf("UDP Server received data: %s\n", recvbuf);
          }
      }
      error_lim = error_cnt >= ERROR_CNT_LIM;

  }
  */
  void stream()
  {
    if (display)
    {
      printf("UDP Sending: %s\n", senbuf);
    }
    if (sendto(s, senbuf, BUFLEN, 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR)
    {
      if (display)
      {
        printf("sendto() failed with error code : %d", WSAGetLastError());
      }
    }
  }

  void end()
  {
    closesocket(s);
    WSACleanup();
    if (display)
    {
        printf("UDP Connection closed\n");
    }
  }
};

typedef struct TcpClient
{
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
  char sendbuf[BUFLEN]; // = "Client: sending data test";
  char recvbuf[BUFLEN]; // = "";
  char SERVERc[15];
  int PORTn = 702;
  struct timeval tv; // timeout settings
  // Functions
  void start(char *S_address, uint32_t port)
  {
    strcpy(SERVERc, S_address);
    PORTn = port;

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
    clientService.sin_addr.s_addr = inet_addr("127.0.0.1"); //inet_addr(SERVERc);
    clientService.sin_port = htons(PORTn);                  //htons(DEFAULT_PORT);
    // Connect to server.
    iResult = connect(ConnectSocket, (SOCKADDR *)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR)
    {
      printf("TCP connect failed with error: %d\n", WSAGetLastError());
      closesocket(ConnectSocket);
      error = true;
      WSACleanup();
    }
    else
    {
      char *ip = inet_ntoa(clientService.sin_addr);
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
  }; //void start

  void get()
  {
    // Send data
    if (display)
    {
      printf("TCP Requesting status...\n");
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
      printf("TCP Waiting repsonse\n");
    }
    n = select(ConnectSocket, &fds, NULL, NULL, &tv);
    if ((n == 0) || (n == -1))
    {
      if (n == 0 && display)
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

  }; // void get

  void end()
  {
    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, 1); // 1 = SD_SEND
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
  }; // void end

};

typedef struct TcpServer
{
private:
  WSADATA wsaData;
  int iResult;
  SOCKET ListenSocket;
  SOCKET ClientSocket;
  struct addrinfo *result = NULL;
  struct addrinfo hints;
  int iSendResult;
  fd_set fds;
  int n;
  int senbuflen;
  int recvbuflen;
  char PORTc[15];

public:
  char recvbuf[BUFLEN];
  char senbuf[BUFLEN];
  bool error, new_message, finish, display, error_lim;
  struct timeval timeout;
  int error_cnt, ERROR_CNT_LIM;

  // Constructor 
  TcpServer(char* S_port) {
      ListenSocket = INVALID_SOCKET;
      ClientSocket = INVALID_SOCKET;
      senbuflen = BUFLEN;
      recvbuflen = BUFLEN;

      error = false; 
      new_message = false;
      finish = false;
      display = false;
      error_cnt = 0;
      ERROR_CNT_LIM = 5;
      error_lim = false;

      strcpy(PORTc, S_port);
  }

  // Functions
  void start()
  {
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
      if (display)
      {
        printf("WSAStartup failed with error: %d\n", iResult);
      }
      error = true;
    }
    else if (display)
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
      if (display)
      {
        printf("TCP getaddrinfo failed with error: %d\n", iResult);
      }
      WSACleanup();
      error = true;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET)
    {
      if (display)
      {
        printf("TCP socket failed with error: %ld\n", WSAGetLastError());
      }
      freeaddrinfo(result);
      WSACleanup();
      error = true;
    }
    else if (display)
    {
      printf("TCP Socket created.\n");
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
      if (display)
      {
        printf("TCP bind failed with error: %d\n", WSAGetLastError());
      }
      freeaddrinfo(result);
      closesocket(ListenSocket);
      WSACleanup();
      error = true;
    }
    else if (!error && display)
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
      if (display)
      {
        printf("listen failed with error: %d\n", WSAGetLastError());
      }
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
      if (display)
      {
        printf("TCP client accept failed with error: %d\n", WSAGetLastError());
      }
      closesocket(ListenSocket);
      WSACleanup();
      error = true;
    }
    else if (!error && display)
    {
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

  void check()
  {
    memset(recvbuf, '\0', BUFLEN);
    memset(senbuf, '\0', BUFLEN);
    new_message = false;
    error = false;
    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0)
    {
      new_message = true;
      if (display)
      {
        printf("TCP Data received: %s\n", recvbuf);
      }
      error_cnt = 0;
    }
    else
    {
      error = true;
      error_cnt++;
      if (display)
      {
        printf("TCP error.\n");
      }
    }
    error_lim = error_cnt >= ERROR_CNT_LIM;
    //Decode after this
  } // void check

  void stream()
  {
    iSendResult = 0;
    if (display)
    {
      printf("TCP sending: %s\n", senbuf);
    }
    iSendResult = send(ClientSocket, senbuf, iResult, 0);
    if (iSendResult == SOCKET_ERROR)
    {
      finish = true;
      if (display)
      {
        printf("TCP send failed with error: %d\n", WSAGetLastError());
      }
    }
  } // void stream

  void end()
  {
    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR && display)
    {
      printf("TCP shutdown failed with error: %d\n", WSAGetLastError());
    }
    else if (display)
    {
      printf("TCP Connection closed.\n");
    }
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
  } // void end
};

// --------------------------------------------------
#endif