#include <iostream> 
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "4444"

// initialize win socket
void iniWinSocket() {
    // stores data about windows socket implementation
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2,2), &wsaData);

    if(result != 0) {
        cerr << "WSAStartup failed." << endl;
        exit(1);
    }
}

// creating client socket
SOCKET createClientSocket() {
    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;    // means we want to use TCP based connection
    hints.ai_protocol = IPPROTO_TCP;
    // we do not include AI_PASSIVE because client side code should only connect to listener, and not listen for connections 

    int iResult = getaddrinfo(SERVER_IP, SERVER_PORT, &hints, &result);
    if(iResult != 0) {
        cerr << "Get Address Info failed." << endl;
        exit(1);
    }
    // creating the socket
    SOCKET clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if(clientSocket == INVALID_SOCKET) {
        cerr << "Socket creation failed." << endl;
        exit(1);
    }
    // connecting to server
    int connectionResult = connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);   // address of the IP and port of target
    if(connectionResult == SOCKET_ERROR) {
        cerr << "Connection failed." << endl;
        exit(1);
    }
    return clientSocket;
}

void receiveMessage(SOCKET clientSocket) {
    //creating a buffer for temp storage
    char buffer[512]; // can hold 512 bytes of data from server side code
    int iResult = recv(clientSocket, buffer, sizeof(buffer), 0);

    if(iResult > 0) {
        buffer[iResult] = '\0'; // terminates string when reaching end, add 0 because we are working with c style string
        cout << "Server message: " << buffer << endl;
    }else if (iResult == 0) {
        cout << "Connection Closed" << endl;
    }
    else {
        cerr << "Recv failed." << endl;
    }

}

int main() {

    iniWinSocket();
    SOCKET clientSocket = createClientSocket();
    cout << "Connected to the server" << endl;
    
    receiveMessage(clientSocket);
   
  

    return 0;
}