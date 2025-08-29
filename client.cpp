#include <iostream>
#include <string>
#include <winsock2.h>
#include <cstring> // For memset
#pragma comment(lib, "ws2_32.lib")
using namespace std;

// Caesar cipher encryption
string encrypt(const string& text, int key = 3) {
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

// Caesar cipher decryption
string decrypt(const string& text, int key = 3) {
    return encrypt(text, 26 - key);
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    sockaddr_in serverAddr;
    char buffer[1024];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        cout << "❌ WSA Initialization failed.\n";
        return 1;
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        cout << "❌ Socket creation failed.\n";
        WSACleanup();
        return 1;
    }

    // Server address setup
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Use localhost for same-PC testing

    // Connect to server
    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        cout << "❌ Connection to server failed.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "✅ Connected to server!\n";

    // Send username
    string username = "AapkaNaam"; // Change this to your name
    string encryptedUser = encrypt(username);
    int sent = send(sock, encryptedUser.c_str(), encryptedUser.length(), 0);
    if (sent == SOCKET_ERROR) {
        cout << "❌ Failed to send username.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Chat loop
    while (true) {
        cout << "📝 You > ";
        string msg;
        getline(cin, msg);

        if (msg.empty()) continue; // Skip empty messages

        string encrypted = encrypt(msg);
        sent = send(sock, encrypted.c_str(), encrypted.length(), 0);
        if (sent == SOCKET_ERROR) {
            cout << "❌ Failed to send message.\n";
            break;
        }

        if (msg == "exit") break;

        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            cout << "❌ Connection lost or server closed.\n";
            break;
        }

        string received = decrypt(string(buffer, bytesReceived));
        if (received == "exit") {
            cout << "🚪 Server has ended the chat.\n";
            break;
        }

        cout << "📥 Server says: " << received << endl;
    }

    // Cleanup
    closesocket(sock);
    WSACleanup();
    return 0;
}