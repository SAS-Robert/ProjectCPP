/** Library for UDP and TCP functions and communication
  *
*/

/* Notas en esta library:
* - Fue la peor de todas, porque windows.h y ws2tcpip.h son necesarias para implementar TCP, pero 
* estas 2 libraries estran en conflicto entre ellas. Este es un bug conocido de Windows y hay que declarar
* los headers como estan puestos ahora para evitar el conflicto. NO uses ninguna de estas librerias en otro header o script.
* - Si necesitas algo que requiera alguno de estos headers, ponlo en esta library.
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
#include <string>
// ------------------------------------------------------------------------

using std::string;

// ------------------ Additional libraries ------------------
#include "SASLIBbasic.hpp"
#include "smpt_definitions.h"

//Connection settings
#pragma warning(disable : 5208)
#pragma comment(lib, "ws2_32.lib") //Winsock Library

#define BUFLEN 512
using namespace std;

// ------------------ Globals definition ------------------
typedef struct tcp_msg_struct
{
public:
    const int size = 11;
    tcp_msg_Type status;
    string messages[30];
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
        messages[msgEnd] = "END";
        messages[msg_none] = " ";
        messages[res1] = "RESISTANCE;1";
        messages[res2] = "RESISTANCE;2";
        messages[res3] = "RESISTANCE;3";
        messages[res4] = "RESISTANCE;4";
        messages[res5] = "RESISTANCE;5";
        messages[res6] = "RESISTANCE;6";
        messages[res7] = "RESISTANCE;7";
        messages[res8] = "RESISTANCE;8";
        messages[res9] = "RESISTANCE;9";
        messages[res10] = "RESISTANCE;10";
        status = msg_none;
    }

};
tcp_msg_struct msgList;
// ------------------ Functions definition ------------------
bool decode_robot(char* message, double& value1, bool& value2, bool& value3)
{
    int field = 0, nrFields = 2, length = strlen(message) + 1;
    bool valid[10], valid_msg = false;
    bool value[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    string messageStr = convert_to_string(message, length) + ";---;";
    string delimiter = ";", emptyStr = "";
    size_t pos = 0;
    string token;
    double tempVal = 1.987654;
    const double tempMin = 0, tempMax = 100;
    // Separate every field on the string
    while ((pos = messageStr.find(delimiter)) != std::string::npos) {
        token = messageStr.substr(0, pos);
        messageStr.erase(0, pos + delimiter.length());

        switch (field) {
        case 0:
            if (strcmp(token.c_str(), emptyStr.c_str()) != 0)
            {
                // Convert string to double here
                std::string::size_type sz;     // alias of size_t
                tempVal = std::stod(token, &sz);
            }
            else
            {
                tempVal = -1;
            }

            valid[0] = (tempVal >= tempMin) && (tempVal <= tempMax);
        case 1:
            value[1] = (token == "true");
            valid[1] = (token == "true") || (token == "false");
            break;
        case 2:
            value[2] = (token == "true");
            valid[2] = (token == "true") || (token == "false");
            break;
        }
        field++;
    }
    // Check that all the fields are correct
    valid_msg = valid[0] && valid[1] && valid[2];

    if (valid_msg)
    {
        value1 = tempVal;
        value2 = value[1];
        value3 = value[2];
    }
    return valid_msg;
}

bool decode_robot_weight(char* message, double& value1)
{
    int field = 0, nrFields = 2, length = strlen(message) + 1;
    bool valid[10], valid_msg = false;
    bool value[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    string messageStr = convert_to_string(message, length) + ";---;";
    string delimiter = ";", emptyStr = "";
    size_t pos = 0;
    string token;
    double tempVal = 0.0;
    const double tempMin = 0, tempMax = 100;
    // Separate every field on the string
    while ((pos = messageStr.find(delimiter)) != std::string::npos) {
        token = messageStr.substr(0, pos);
        messageStr.erase(0, pos + delimiter.length());

        switch (field) {
        case 0:
            if (strcmp(token.c_str(), emptyStr.c_str()) != 0)
            {
                // Convert string to double here
                std::string::size_type sz;     // alias of size_t
                tempVal = std::stod(token, &sz);
            }
            else
            {
                tempVal = -1;
            }

            valid[0] = (tempVal >= tempMin) && (tempVal <= tempMax);
        }
        field++;
    }
    // Check that all the fields are correct
    valid_msg = valid[0];

    if (valid_msg)
    {
        value1 = tempVal;
    }
    return valid_msg;
}

bool decode_extGui(char* message, bool& finished, bool& playPause, int& level, tcp_msg_Type& result)
{
    int length = strlen(message);
    string messageStr = convert_to_string(message, length);
    string token;
    bool valid_msg = false;

    for (int i = 0; i < MSG_AMOUNT ;i++)
    {
        valid_msg = (strcmp(message, msgList.messages[i].c_str()) == 0);
        if (valid_msg)
        {
            msgList.status = (tcp_msg_Type)i;
            break;
        }
    }

    if (valid_msg)
    {
        result = msgList.status;
        // Update Play-Pause button status
        if (msgList.status == play)
        {
            playPause = true;
        }
        else if (msgList.status == pause)
        {
            playPause = false;
        }
        // resistance level update 
        if (msgList.status >= res1 && msgList.status <= res10)
        {
            level = ((int)msgList.status) - 10;
        }
        // In case the program has finished
        finished = (msgList.status == finish);
    }
    //else
    //{
    //    result = msg_none;
    //}

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
  int n, playPause_cnt;
  uint32_t PORTn;
  char SERVERc[15]; // IP address
  char itoaNr[32];

public:
  char buf[BUFLEN];
  char message[BUFLEN];
  struct timeval timeout;
  bool error, error_lim;
  int error_cnt;
  bool display;
  // Robot variables
  bool isMoving, playPause, buttonPressed;
  double isVelocity, legWeight;
  bool Reached;
  bool valid_msg;
  string displayMsg;
  bool legSaved;

  // Constructor 
  UdpClient(char* S_address, uint32_t port) {
      slen = sizeof(si_other);
      error = false; 
      error_lim = false;
      error_cnt = 0;
      display = true;
      // Robot variables
      isMoving = false;
      playPause = true;
      buttonPressed = false;
      playPause_cnt = 0;
      Reached = false;
      valid_msg = false;

      strcpy(SERVERc, S_address);
      PORTn = port;
      displayMsg = " ";

      legWeight = -1.0;
      legSaved = false;
  }
  //Functions
  void start()
  {
    //Initialise winsock
    if (display)
    {
        displayMsg ="UPD Connection - starting...\n";
    }
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "Failed. Error Code : ";
        displayMsg += string(itoaNr).c_str();
        error = true;
    }
    //create socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
      if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "socket() failed with error code : ";
          displayMsg += string(itoaNr).c_str();
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
        displayMsg = "UDP Client running on IP ";
        displayMsg += string(ip).c_str();
        displayMsg += ", port";
        itoa(PORTn, itoaNr, 10);
        displayMsg += string(itoaNr).c_str();
    }
    //send a start message so the server can start sending stuff
    strcpy(message, "0;STATUS");
    if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR)
    {
      error = true;
    }
    legSaved = false;
  };
  void get()
  {
    error = false;
    memset(buf, '\0', BUFLEN);
    valid_msg = false;
    if (display)
    {
        displayMsg = "UPD: Requesting status...";
    }
    if (sendto(s, message, strlen(message), 0, (struct sockaddr *)&si_other, slen) == SOCKET_ERROR)
    {
      if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "sendto() failed with error code : ";
          displayMsg += string(itoaNr).c_str();
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
          displayMsg = "Timeout - ";
          displayMsg += message;
      }
      else if (display)
      {
          displayMsg ="Error while receiving.";
      }
      error = true;
      error_cnt++;
    }

    if (!error)
    {
      int length = sizeof(SERVERc);
      recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen);
      // Decode message here
      valid_msg = decode_robot(buf, isVelocity, Reached, buttonPressed);
      if (valid_msg)
      {
        error_cnt = 0;
        if (buttonPressed && playPause_cnt >= 3)
        {
            playPause = !playPause;
            playPause_cnt = 0;
        }
        else if (!buttonPressed && playPause_cnt < 10)
        {
            playPause_cnt++;
        }
        if (display)
        {
            // show stimulation parameters
            std::stringstream tempValue;
            tempValue << std::setprecision(7) << isVelocity;
            string tempString = tempValue.str();
            displayMsg = "UDP Received: isVelocity=";
            displayMsg += tempString.c_str();
            itoa(Reached, itoaNr, 10);
            displayMsg += ",Reached=";
            displayMsg += string(itoaNr).c_str();
            itoa(playPause, itoaNr, 10);
            displayMsg += ",playPause=";
            displayMsg += string(itoaNr).c_str();
        }
      }
      else
      {
        if (display)
        {
            displayMsg = "UDP message not valid - Received: ";
            displayMsg += string(buf).c_str();
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
        displayMsg = "UDP Client connection closed";
    }
  };

  void getWeight()
  {
      legSaved = false;
      error = false;
      memset(buf, '\0', BUFLEN);
      valid_msg = false;
      if (display)
      {
          displayMsg = "UPD: Requesting status...";
      }
      if (sendto(s, message, strlen(message), 0, (struct sockaddr*)&si_other, slen) == SOCKET_ERROR)
      {
          if (display)
          {
              itoa(WSAGetLastError(), itoaNr, 10);
              displayMsg = "sendto() failed with error code : ";
              displayMsg += string(itoaNr).c_str();
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
              displayMsg = "Timeout - ";
              displayMsg += message;
          }
          else if (display)
          {
              displayMsg = "Error while receiving.";
          }
          error = true;
          error_cnt++;
      }

      if (!error)
      {
          int length = sizeof(SERVERc);
          recvfrom(s, buf, BUFLEN, 0, (struct sockaddr*)&si_other, &slen);
          // Decode message here
          valid_msg = decode_robot_weight(buf, legWeight);
          if (valid_msg)
          {
              error_cnt = 0;
              if (legWeight > 0.1)
              {
                  legSaved = true;
              }

              if (display)
              {
                  // show stimulation parameters
                  std::stringstream tempValue;
                  tempValue << std::setprecision(7) << legWeight;
                  string tempString = tempValue.str();
                  displayMsg = "UDP Received: legWeight=";
                  displayMsg += tempString.c_str();
              }
          }
          else
          {
              if (display)
              {
                  displayMsg = "UDP message not valid - Received: ";
                  displayMsg += string(buf).c_str();
              }
              error_cnt++;
          }
      }
      error_lim = error_cnt >= 10;
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
    char itoaNr[32];

  public:
    char recvbuf[BUFLEN];
    char senbuf[BUFLEN];
    bool error, new_message, finish, display, error_lim, playPause;
    struct timeval timeout;
    int error_cnt, ERROR_CNT_LIM;
    string displayMsg;
    int level;
    // constructor
    UdpServer(char *S_address, char *PORTc)
    {
      nPORT = 30001;
      slen = sizeof(si_other);
      display = false;
      playPause = true;         // lets assume that it starts like that
      finish = false;
      nPORT = atoi(PORTc);
      strcpy(SERVERc, S_address);
      ERROR_CNT_LIM = 5;
      displayMsg = " ";
      level = 5;
  }
  // methods
  void start()
  {
    //Initialise winsock
    if(display) { displayMsg = "UDP Server - Initialising Winsock..."; }
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "UDP Server - Failed. Error Code : ";
        displayMsg += string(itoaNr).c_str();
      //exit(EXIT_FAILURE);
    }
    printf("UDP Server - Initialised.\n");

    //Create a socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "UDP Server - Could not create socket : ";
        displayMsg += string(itoaNr).c_str();

    }
    displayMsg = "UDP Server - Socket created.";

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_addr.S_un.S_addr = inet_addr(SERVERc);
    server.sin_port = htons(nPORT);
    char *ip = inet_ntoa(server.sin_addr);

    displayMsg = "Running UDP Server on IP ";
    displayMsg += string(ip).c_str();
    displayMsg += ", port ";
    itoa(nPORT, itoaNr, 10);

    displayMsg += string(itoaNr).c_str();
    //Bind
    if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "UDP Server - Bind failed with error code : ";
        displayMsg += string(itoaNr).c_str();

    }
    displayMsg = "UDP Server - Bind done";
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
              itoa(WSAGetLastError(), itoaNr, 10);
              displayMsg = "recvfrom() failed with error code : ";
              displayMsg += string(itoaNr).c_str();
          }
          error = true;
          error_cnt++;
      } 
      else if (display)
      {
          displayMsg = "UDP Server received data: ";
          displayMsg += string(recvbuf).c_str();
        error = false;
      }
      error_lim = error_cnt >= ERROR_CNT_LIM;
  }

  void stream(double data)
  {
    if (display)
    {
        if (data != 0)
        {
            string temp = to_string(data);
            strcpy(senbuf, temp.c_str());
            displayMsg = "UDP Sending: ";
            displayMsg += string(senbuf).c_str();
        }
    }

    int sendOk = sendto(s, senbuf, BUFLEN, 0, (struct sockaddr*)&si_other, slen);
    if (sendOk == SOCKET_ERROR)
    {
      if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "sendto() failed with error code : ";
          displayMsg += string(itoaNr).c_str();
      }
    }
    memset(senbuf, '\0', BUFLEN);
  }

  void end()
  {
    closesocket(s);
    WSACleanup();
    if (display)
    {
        displayMsg = "UDP Connection closed";
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
  char itoaNr[32];

public:
  char sendbuf[BUFLEN]; // = "Client: sending data test";
  char recvbuf[BUFLEN]; // = "";
  char SERVERc[15];
  int PORTn = 702;
  struct timeval tv; // timeout settings
  string displayMsg;
  // Functions
  void start(char *S_address, uint32_t port)
  {
    strcpy(SERVERc, S_address);
    PORTn = port;

    displayMsg = "TCP Connection - starting...";
    memset(recvbuf, '\0', BUFLEN);
    memset(sendbuf, '\0', BUFLEN);
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR)
    {
        itoa(iResult, itoaNr, 10);
        displayMsg = "TCP WSAStartup failed with error: ";
        displayMsg += string(itoaNr).c_str();
    }
    // Create a SOCKET for connecting to server
    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "TCP socket failed with error: ";
        displayMsg += string(itoaNr).c_str();
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
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "TCP connect failed with error: ";
        displayMsg += string(itoaNr).c_str();
        closesocket(ConnectSocket);
        error = true;
        WSACleanup();
    }
    else
    {
      char *ip = inet_ntoa(clientService.sin_addr);
      int porti = PORTn;
      displayMsg = "TCP Client running on IP ";
      displayMsg += string(ip).c_str();
      displayMsg += ", port ";
      itoa(porti,itoaNr, 10);
      displayMsg += string(itoaNr).c_str();
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
        displayMsg = "TCP Requesting status...";
    }
    iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR)
    {
      itoa(WSAGetLastError(), itoaNr, 10);
      displayMsg = "TCP send failed with error: ";
      displayMsg += string(itoaNr).c_str();
      closesocket(ConnectSocket);
      WSACleanup();
      error = true;
    }
    // Wait until timeout or data received.
    if (display)
    {
        displayMsg ="TCP Waiting repsonse";
    }
    n = select(ConnectSocket, &fds, NULL, NULL, &tv);
    if ((n == 0) || (n == -1))
    {
      if (n == 0 && display)
      {
          displayMsg = "TCP Timeout";
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
          displayMsg = "TCP Received: ";
          displayMsg += string(recvbuf).c_str();
      }
      else if (iResult == 0 && display)
      {
          displayMsg = "TCP Connection closed";
      }
      else if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "TCP recv failed with error: ";
          displayMsg += string(itoaNr).c_str();
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
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "TCP shutdown failed with error: ";
        displayMsg += string(itoaNr).c_str();
      closesocket(ConnectSocket);
      WSACleanup();
    }
    // close the socket
    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "TCP close failed with error: ";
        displayMsg += string(itoaNr).c_str();
      WSACleanup();
    }
    // Receive until the peer closes the connection
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    do
    {
      if (iResult == 0)
      {
          displayMsg = "TCP Connection closed";
      }
      //else if (iResult > 0) {printf("Bytes received: %d\n", iResult);}
      iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    } while (iResult > 0);

    WSACleanup();
  }; // void end

};
/*
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
          itoa(iResult), itoaNr, 10);
          displayMsg ="WSAStartup failed with error: " + string(itoaNr).c_str();
      }
      error = true;
    }
    else if (display)
    {
        displayMsg = "TCP Winsock initialised.";
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
          itoa(iResult, itoaNr, 10);
          displayMsg = "TCP getaddrinfo failed with error: " + string(itoaNr).c_str();
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
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg ="TCP socket failed with error: " + string(itoaNr).c_str();
      }
      freeaddrinfo(result);
      WSACleanup();
      error = true;
    }
    else if (display)
    {
        displayMsg = "TCP Socket created.";
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
      if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "TCP bind failed with error: " + string(itoaNr).c_str();
      }
      freeaddrinfo(result);
      closesocket(ListenSocket);
      WSACleanup();
      error = true;
    }
    else if (!error && display)
    {
        displayMsg = "TCP Start-up finished.";
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
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "listen failed with error: " + itoa(WSAGetLastError());
      }
      closesocket(ListenSocket);
      WSACleanup();
      error = true;
    }
    else
    {
        displayMsg = "TCP Listening started on port " + PORTc + ". Waiting for a client.";
    }
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
      if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "TCP client accept failed with error: " + string(itoaNr).c_str();
      }
      closesocket(ListenSocket);
      WSACleanup();
      error = true;
    }
    else if (!error && display)
    {
        displayMsg = "A TCP client has been accepted.";
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
          displayMsg = "TCP Data received: " + string(recvbuf).c_str();
      }
      error_cnt = 0;
    }
    else
    {
      error = true;
      error_cnt++;
      if (display)
      {
          displayMsg = "TCP error.";
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
        displayMsg = "TCP sending: " + senbuf;
    }
    iSendResult = send(ClientSocket, senbuf, iResult, 0);
    if (iSendResult == SOCKET_ERROR)
    {
      finish = true;
      if (display)
      {
          itoa(WSAGetLastError(), itoaNr, 10);
          displayMsg = "TCP send failed with error: " + string(itoaNr).c_str();
      }
    }
  } // void stream

  void end()
  {
    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR && display)
    {
        itoa(WSAGetLastError(), itoaNr, 10);
        displayMsg = "TCP shutdown failed with error: " + string(itoaNr).c_str();
    }
    else if (display)
    {
        displayMsg = "TCP Connection closed.";
    }
    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
  } // void end
};
*/
// --------------------------------------------------
#endif