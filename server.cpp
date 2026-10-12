#include <iostream>
#include <vector>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define closesocket close
#endif

using namespace std;

void printBoard(char b[]) {
    cout << "\n " << b[0] << " | " << b[1] << " | " << b[2] << "\n";
    cout << "---|---|---\n";
    cout << " " << b[3] << " | " << b[4] << " | " << b[5] << "\n";
    cout << "---|---|---\n";
    cout << " " << b[6] << " | " << b[7] << " | " << b[8] << "\n\n";
}

bool checkWin(char b[], char p) {
    int win[8][3] = {
        {0,1,2}, {3,4,5}, {6,7,8},
        {0,3,6}, {1,4,7}, {2,5,8},
        {0,4,8}, {2,4,6}
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

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Socket create karne me error!\n";
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cerr << "Bind failed! Port 8080 busy ho sakta hai.\n";
        return 1;
    }

    listen(server_fd, 1);
    cout << "Server Chalu Hai. Player O ke connect hone ka wait kar rahe hain...\n";

    int client_fd = accept(server_fd, NULL, NULL);
    cout << "Player O Connect ho gaya!\n";

    char board[9] = {'1','2','3','4','5','6','7','8','9'};
    printBoard(board);

    for (int turn = 0; turn < 9; turn++) {
        int move;
        if (turn % 2 == 0) {
            // Player X (Server) ki baari
            cout << "Aapki baari (X). Box number chune (1-9): ";
            cin >> move;
            board[move - 1] = 'X';
            printBoard(board);

            send(client_fd, (char*)&move, sizeof(move), 0);

            if (checkWin(board, 'X')) {
                cout << "Badhaai ho! Aap (X) Jeet gaye!\n";
                break;
            }
        } else {
            // Player O (Client) ki baari
            cout << "Player O ki baari ka intezaar hai...\n";
            recv(client_fd, (char*)&move, sizeof(move), 0);

            board[move - 1] = 'O';
            cout << "Player O ne chuna: " << move << "\n";
            printBoard(board);

            if (checkWin(board, 'O')) {
                cout << "Player O Jeet gaya!\n";
                break;
            }
        }

        if (turn == 8) {
            cout << "Match Draw ho gaya!\n";
        }
    }

    closesocket(client_fd);
    closesocket(server_fd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
