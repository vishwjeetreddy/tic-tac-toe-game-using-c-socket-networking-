#include <iostream>
#include <string>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
#endif

using namespace std;

// 1. Board display function
void printBoard(char b[]) {
    cout << "\n " << b[0] << " | " << b[1] << " | " << b[2] << "\n";
    cout << "---|---|---\n";
    cout << " " << b[3] << " | " << b[4] << " | " << b[5] << "\n";
    cout << "---|---|---\n";
    cout << " " << b[6] << " | " << b[7] << " | " << b[8] << "\n\n";
}

// 2. Win evaluation function
bool checkWin(char b[], char p) {
    int win[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8}, // Rows
        {0,3,6}, {1,4,7}, {2,5,8}, // Columns
        {0,4,8}, {2,4,6}           // Diagonals
    };
    for (int i = 0; i < 8; i++) {
        if (b[win[i][0]] == p && b[win[i][1]] == p && b[win[i][2]] == p)
            return true;
    }
    return false;
}

int main() {
#ifdef _WIN32
    WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa);
#endif

    string ip;
    cout << "PC 1 (Server) ka IP Address enter karein: ";
    cin >> ip;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Socket create nahi ho paya!\n";
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    cout << "PC 1 se connect ho rahe hain...\n";
    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "Connection failed! Check karein ki PC 1 par server chal raha hai aur IP sahi hai.\n";
        return 1;
    }

    cout << "Connect ho gaya! Aap Player O hain.\n";

    char board[9] = {'1','2','3','4','5','6','7','8','9'};
    printBoard(board);

    for (int turn = 0; turn < 9; turn++) {
        int move;

        if (turn % 2 == 0) {
            // Player X (Server) ki baari ka wait
            cout << "Player X ki baari ka intezaar hai...\n";
            recv(sock, (char*)&move, sizeof(move), 0);

            board[move - 1] = 'X';
            cout << "Player X ne chuna: " << move << "\n";
            printBoard(board);

            if (checkWin(board, 'X')) {
                cout << "Player X Jeet gaya!\n";
                break;
            }
        } else {
            // Player O (Client) ki baari
            cout << "Aapki baari (O). Box number chune (1-9): ";
            cin >> move;
            board[move - 1] = 'O';
            printBoard(board);

            send(sock, (char*)&move, sizeof(move), 0);

            if (checkWin(board, 'O')) {
                cout << "Badhaai ho! Aap (O) Jeet gaye!\n";
                break;
            }
        }

        if (turn == 8) {
            cout << "Match Draw ho gaya!\n";
        }
    }

    closesocket(sock);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}