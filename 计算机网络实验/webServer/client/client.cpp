#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include<windows.h>

#define SERVER_PORT 80  //�������˿ں�
#define SERVER_IP_ADDRESS "127.0.0.1"	//������IP��ַ
#define TIMEOUT 3000  //��ʱΪ3��
#define BUF_SIZE 1024
#define NULL_LINE "\r\n"
#pragma comment(lib, "ws2_32.lib")
#pragma comment( lib,"winmm.lib" )

using namespace std;

// �������з��������ص���Ϣ
int Recv_info(int Socket, char* rev_buf, int size){
    int RecvSize = 0;
    DWORD t1, t2;
    t1 = timeGetTime();  //��ʱ����ʼ
    cout << "Response Message From Server:  " << endl;
    while (size > 0){ 
        RecvSize = recv(Socket, rev_buf, size, 0);
        if (RecvSize == SOCKET_ERROR) {
            cout << "Receive Response Message Error!" << endl;
            return 0;
        }
        else {
            //������յ�����Ϣ
            cout << rev_buf ;
        }
        size = size - RecvSize;
        rev_buf += RecvSize;
        t2 = timeGetTime();  //��ʱ������
        if ((t2 - t1) > TIMEOUT) {
            size = -1; //�˳�ѭ��������������Ӧ��Ϣȫ���������
            cout << endl << endl;
            cout << "Http request is finished! You can start the next request!" << endl;
        }
    }
    return 1;
}

int main(int argc, char* argv[]){   
    int res = 0;
    int addrLen = 0;
    string file;  //Ҫ���ҵ��ļ�
    string requestLine;  //������
    char sendBuffer[BUF_SIZE];  // ���͵�http������Ϣ
    char RecvBuffer[BUF_SIZE];  //���յ�����Ϣ
    WSADATA wsadata;  //��ʼ���׽��֣�IP��ַ���˿ںţ�
    SOCKET clientSocket;   //ʹ��TCPЭ�鴴�� Socket
    struct sockaddr_in serverAddr; //sockaddr_in:������������ͨ�ŵĵ�ַ

    while (true) {
        //��ʼ��Socket
        if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
            cout << "Client Socket Initial Failed::" << GetLastError() << endl;
            WSACleanup();
            return -1;
        }

        // �ڿͻ��˴���ʹ��TCPЭ�鴴����ʽ�׽��� (socket)
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET){
            cout << "Create Socket Defeat::" << GetLastError() << endl;
            closesocket(clientSocket);
            WSACleanup();
            return -1;
        }

        //����memset(&ServerAddr,0,sizeof(ServerAddr))��������������䣬�ٶ�ǰ����������һ��ֵ
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
        serverAddr.sin_port = htons(SERVER_PORT);

        //connect �ͻ�����������������������
        res = connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        if (res == SOCKET_ERROR){
            cout << "Connect Error::" << GetLastError() << endl;
            WSACleanup();
            return -1;
        }
        else{
            cout << "Succeed to connect with server[" << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port) << "]" << endl;
        }
    
        // ��ͻ��˷���http������
        cout << "Please input the file you want to find:";
        cin >> file;
        cout << endl;

        // http�е�������
        memset(sendBuffer, 0, sizeof(sendBuffer));
        requestLine = "GET \\" + file + " HTTP/1.0\r\n";
        cout << "RequestLine of Http:  " << endl;
        cout << "     " << requestLine << endl;
        strcat_s(sendBuffer, requestLine.c_str());
        strcat_s(sendBuffer, "\r\n");  // httpЭ���п���

        // ����http����
        res = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
        if (res == SOCKET_ERROR) {
            cout << "Send Http Request Error!" << WSAGetLastError() << endl;
            exit(0);
        }
        cout << "Succeed to send http request!" << endl;
        cout << endl;
        
        //���շ��������ص���Ϣ
        memset(RecvBuffer, 0, sizeof(RecvBuffer));
        res = Recv_info(clientSocket, RecvBuffer, sizeof(RecvBuffer));
        cout << "**************************************************" << endl;
    }
    
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
