#include <iostream>
#include <string>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

string encrypt(string text, int key = 3) {
    string result = "";
    for (char c : text) {
        if (isalpha(c)) {
            char base = islower(c) ? 'a' : 'A';
            result += char((c - base + key) % 26 + base);
        } else {
            result += c;
        }
    }
    return result;
}

string decrypt(string text, int key = 3) {
    return encrypt(text, 26 - key);
}

int main() {
    WSADATA wsa;
    SOCKET server, client;
    sockaddr_in serverAddr, clientAddr;
    int clientSize = sizeof(clientAddr);
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "❌ WSA Initialization failed.\n";
        return 1;
    }

    server = socket(AF_INET, SOCK_STREAM, 0);
    if (server == INVALID_SOCKET) {
        cout << "❌ Socket creation failed.\n";
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    if (bind(server, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "❌ Bind failed.\n";
        return 1;
    }

    listen(server, 1);
    cout << "🔌 Server listening on port 8080...\n";

    client = accept(server, (sockaddr*)&clientAddr, &clientSize);
    if (client == INVALID_SOCKET) {
        cout << "❌ Client connection failed.\n";
        return 1;
    }

    string clientIP = inet_ntoa(clientAddr.sin_addr);
    cout << "✅ Client connected from IP: " << clientIP << endl;

    // Receive username
    memset(buffer, 0, sizeof(buffer));
    recv(client, buffer, sizeof(buffer), 0);
    string username = decrypt(string(buffer));
    cout << "👤 Chatting with: " << username << endl;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(client, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;

        string received = decrypt(string(buffer));
        if (received == "exit") {
            cout << "🚪 " << username << " has left the chat.\n";
            break;
        }

        cout << "📥 " << username << " says: " << received << endl;

        cout << "📝 You > ";
        string reply;
        getline(cin, reply);
        if (reply == "exit") {
            send(client, encrypt(reply).c_str(), reply.length(), 0);
            break;
        }

        string encrypted = encrypt(reply);
        send(client, encrypted.c_str(), encrypted.length(), 0);
    }

    closesocket(client);
    closesocket(server);
    WSACleanup();
    return 0;
}