#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <stdlib.h>

#define SERVER_PORT 80  //�������˿ں�
#define SERVER_IP_ADDRESS "127.0.0.1"	//������IP��ַ�����ص�ַ��
#define BACKLOG 10  // ��δ��������(����δaccept)������������п������ɵ������Ŀ
#define BUF_SIZE 1024
#define OK 1
#define ERROR 0

#pragma comment(lib, "ws2_32.lib")
using namespace std;

int InitServerSocket();
int Request_Message_Handler(char* message, int Socket, char* whole_path);
int URI_Handler(const char* path, int Socket,char* URI);
int Error_Request_Method(int Socket);
int Send_Info(int Socket, const char* sendbuf, int Length);
int Find_File(const char* path);
int Send_Response_Message(char* URI, int Socket, const char* path);
int File_Not_Find(char* URI,int Socket);
const char* Method_Handler(char* method, int Socket);
const char* Judge_File_Type(char* URI, const char* content_type);
const char* Get_Current_Time(const char* cur_time);

//��ʼ��Socket
int InitServerSocket() {
    int res = 0;
    WSADATA  wsadata;
    SOCKET ServerSock;
    struct sockaddr_in ServerAddr;

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
        cout << "Server Socket Initial Failed::" << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }

    //��һ���ͻ��������������ʱ��Create Socket
    ServerSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSock == INVALID_SOCKET) {
        cout << "Socket Create Failed::" << WSAGetLastError() << endl;
        closesocket(ServerSock);
        WSACleanup();
        return -1;
    }
    cout << "Succeed to create socket!" << endl;

    /* ���÷�����IP���˿���Ϣ */
    memset(&ServerAddr, 0, sizeof(ServerAddr));   // ��ʼ��ÿһ���ֽ�ȫ��0���
    ServerAddr.sin_family = AF_INET;  //�����ֽ�˳��
    ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
    ServerAddr.sin_port = htons(SERVER_PORT); // �����ֽ�˳��ת��Ϊ�����ֽ�˳��(������)

    //Bind Socket �����׽��ֺͱ��������ַ��ϵ��һ��,Ϊ�׽��ְ�һ���˿ں�
    res = bind(ServerSock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr));
    if (res != 0) {
        cout << "Bind Stream Socket Failed:" << WSAGetLastError() << endl;
        WSACleanup();
        return -1;
    }
    cout << "Succeed to bind stream socket!" << endl;

    return ServerSock;
}


//����HTTP��������Ϣ 
int Request_Message_Handler(char* message, int Socket, char* whole_path) {
    int res = 0;
    char RequestMethod[BUF_SIZE];  //���󷽷�
    char URI[BUF_SIZE];
    char Version[BUF_SIZE];  //HTTP�汾
    int pos;
    string path;
    const char* suffix;  //��׺��

    memset(RequestMethod, 0, sizeof(RequestMethod));
    memset(URI, 0, sizeof(URI));
    memset(Version, 0, sizeof(Version));

    //��ȡ"���󷽷�"��"URI"��"HTTP�汾"�����ؼ�Ҫ��
    if (sscanf(message, "%s %s %s", RequestMethod, URI, Version) != 3) {
        cout << "Request Line Error!" << endl;
        return ERROR;
    }

    if ((suffix = strrchr(whole_path, '\\')) != NULL) {
        string w_path = string(whole_path);
        pos = w_path.rfind("\\");
        w_path.replace(pos, string(suffix).length(), "");
        path = w_path + URI;
    }

    //����"���󷽷�" 
    if (Method_Handler(RequestMethod, Socket) == ERROR) {
        cout << "Handle RequestMethod Error!" << endl;
        return ERROR;
    }
    else if (Method_Handler(RequestMethod, Socket) == "GET") {
        cout << "Http Request's Detail Information: " << endl;
        cout << "     Http Request Method is Get!" << endl;
    }

    // ����"URI"
    if (URI_Handler(path.c_str(), Socket, URI) == ERROR) {
        cout << "Handle URI Error! Please reinput the URI!" << endl;
        return ERROR;
    }
    else {
        cout << "     URI:" << URI << endl;
        res = Send_Response_Message(URI, Socket, path.c_str());
    }

    if (res == OK) {
        cout << "Succeed to send files!" << endl;
    }
    return OK;
}


//����http������ʽ 
const char* Method_Handler(char* method, int Socket) {
    if (strcmp(method, "GET") == 0) {
        return "GET";
    }
    else if (strcmp(method, "POST") == 0) {
        return "POST";
    }
    else {
        Error_Request_Method(Socket);
        return ERROR;
    }
}


//501 Not Implemented��Ӧ:������������Ӧ����ָʾ���󷽷����ܷ�����֧�ֲ����޷�����
int Error_Request_Method(int Socket) {
    const char* Method_Err_Line = "HTTP/1.0 501 Not Implemented\r\n";  
    const char* cur_time = "";  // ��ǰʱ��
    const char* Method_Err_End = "\r\n";
    const char* Method_Err_Info = "The request method is not yet completed!\n";

    if (Send_Info(Socket, Method_Err_Line, strlen(Method_Err_Line)) == ERROR) {
        cout << "Sending method_error_line failed!" << endl;
        return ERROR;
    }

    cur_time = Get_Current_Time(cur_time);
    Send_Info(Socket, "Date: ", 6);
    if (Send_Info(Socket, cur_time, strlen(cur_time)) == ERROR) {
        cout << "Sending current time error!" << endl;
        return ERROR;
    }

    if (Send_Info(Socket, Method_Err_End, strlen(Method_Err_End)) == ERROR) {
        cout << "Sending method_error_end failed!" << endl;
        return ERROR;
    }

    if (Send_Info(Socket, Method_Err_Info, strlen(Method_Err_Info)) == ERROR) {
        cout << "Sending method_error_info failed!" << endl;
        return ERROR;
    }
    return OK;
}


//��ȡWeb�������ĵ�ǰʱ����Ϊ��Ӧʱ�� 
const char* Get_Current_Time(const char* cur_time) {
    time_t curtime;
    time(&curtime);
    cur_time = ctime(&curtime);
    return cur_time;
}

//������Ϣ���ͻ��� ,Ҫ��ȫ
int Send_Info(int Socket, const char* sendbuf, int size) {
    int sendSize = 0;

    while (size > 0) {
        sendSize = send(Socket, sendbuf , size, 0);  //���ص����Ѿ����͵��ֽ���
        if (sendSize == SOCKET_ERROR)
            return ERROR;
        size = size - sendSize;  //ѭ���������˳�����
        sendbuf += sendSize;    //�����ѷ�buffer��ƫ����
    }
    return OK;
}


//����http�����е�URI
int URI_Handler(const char* path, int Socket, char* URI) {
    if (Find_File(path) == ERROR) {
        File_Not_Find(URI,Socket); //����ļ�δ�ҵ�����ʾ��Ϣ
        return ERROR;
    }
    else
        return OK;
}

//�ڷ������˲����ļ� 
int Find_File(const char* path) {
    struct _stat File_info;  //�ļ����У���Ϣ�Ľṹ��
    if (_stat(path, &File_info) == -1) {  //������ȡָ��·�����ļ������ļ��е���Ϣ
            return ERROR;
    }
    else {
        cout << "     path: " << path << endl;
        return OK;
    }
}


// ������������Ӧ���� ״̬��200��ʾ�ɹ�
int Send_Response_Message(char* URI, int Socket,const char* path) {
    const char* File_ok_line = "     HTTP/1.0 200 OK\r\n";
    const char* cur_time = "";
    const char* File_ok_type = "";
    const char* File_ok_length = "Content-Length: ";
    const char* File_ok_end = "\r\n";  // ����

    FILE* file;
    struct stat file_stat;
    char Length[BUF_SIZE];
    char sendBuf[BUF_SIZE];
    int send_len;   // ���͵ı��ĳ���

    if (Judge_File_Type(URI, File_ok_type) == ERROR) {
        cout << "The request file's type from client's request message is error!\n" << endl;
        return ERROR;
    }

    memset(Length, 0, BUF_SIZE);
    memset(sendBuf, 0, BUF_SIZE);

    file = fopen(path, "rb"); 
    if (file != NULL) {
        //��������ļ���״̬��Ϣ
        fstat(fileno(file), &file_stat); //fileno()����ȡ�ò���streamָ�����ļ�����ʹ�õ��ļ�������
        // Ҫת��������|Ҫд��ת�������Ŀ���ַ���|ת������
        itoa(file_stat.st_size, Length, 10);

        // ������Ӧ��
        if (Send_Info(Socket, File_ok_line, strlen(File_ok_line)) == ERROR) {
            cout << "Sending file_ok_line error!" << endl;
            return ERROR;
        }

        // ������Ӧͷ�е�Content-Type
        File_ok_type = Judge_File_Type(URI, File_ok_type);
        if (File_ok_type == "Content-type: text/html\r\n") {
            system("start C:/Users/С��/Desktop/study/����ʵ��/webServer/Debug/index.html");
        }
        cout << "     File type: " << File_ok_type << endl;
        if (Send_Info(Socket, File_ok_type, strlen(File_ok_type)) == ERROR) {
            cout << "Sending file_ok_type error!" << endl;
            return ERROR;
        }

        // ������Ӧͷ�е�Content-Length
        if (Send_Info(Socket, File_ok_length, strlen(File_ok_length)) != ERROR) {
            if (Send_Info(Socket, Length, strlen(Length)) != ERROR) {
                if (Send_Info(Socket, "\n", 1) == ERROR) {
                    cout << "Sending file_ok_length error!" << endl;
                    return ERROR;
                }
            }
        }

        // ������Ӧͷ�е�Date
        cur_time = Get_Current_Time(cur_time);
        Send_Info(Socket, "Date: ", 6);
        if (Send_Info(Socket, cur_time, strlen(cur_time)) == ERROR) {
            cout << "Sending current time error!" << endl;
            return ERROR;
        }

        // ���Ϳ���
        if (Send_Info(Socket, File_ok_end, strlen(File_ok_end)) == ERROR) {
            cout << "Sending file_ok_end error!" << endl;
            return ERROR;
        }

        //����������ļ�����
        while (file_stat.st_size > 0) {
            if (file_stat.st_size < 1024) {
                //fread(�ڴ���ָ��,ÿ������Ĵ�С(���ֽ�Ϊ��λ),Ԫ�ص�����(�ֽ�),Ҫ���ж�ȡ���ݵ��ļ���)
                send_len = fread(sendBuf, 1, file_stat.st_size, file); //���سɹ���ȡ�Ķ�����
                if (Send_Info(Socket, sendBuf, send_len) == ERROR) {
                    cout << "Sending file information error!" << endl;
                    continue;
                }
                file_stat.st_size = 0;
            }
            else {  
                send_len = fread(sendBuf, 1, 1024, file);
                if (Send_Info(Socket, sendBuf, send_len) == ERROR) {
                    cout << "Sending file information error!" << endl;
                    continue;
                }
                file_stat.st_size -= 1024;  //һ����1024���ֽ�
            }
        }
    }
    else {  // Ŀ���ļ�Ϊ��
        cout << "The file is NULL!" << endl;
        return ERROR;
    }
    return OK;
}


// �ж�http�����������ļ�������
const char* Judge_File_Type(char* URI, const char* content_type) {
    const char* suffix;  //��׺��
    // strrchr���� str �����һ�γ���ָ���ַ���λ�ã��������λ�õĵ�ַ
    if ((suffix = strrchr(URI, '.')) != NULL) {
        suffix = suffix + 1;

        if (strcmp(suffix, "html") == 0) {
            return content_type = "Content-type: text/html\r\n";
        }
        else if (strcmp(suffix, "jpg") == 0) {
            return content_type = "Content-type: image/jpg\r\n";
        }
        else if (strcmp(suffix, "png") == 0) {
            return content_type = "Content-type: image/png\r\n";
        }
        else if (strcmp(suffix, "txt") == 0) {
            return content_type = "Content-type: text/plain\r\n";
        }
        else
            return ERROR;
    }
}


//�ļ�û���ҵ�ʱ404 Not Found��Ӧ 
int File_Not_Find(char* URI,int Socket) {
    const char* File_err_line = "HTTP/1.0 404 Not Found\r\n";
    const char* cur_time = "";
    const char* File_err_type ="";
    const char* File_err_end = "\r\n";
    const char* File_err_info = "The file which is requested is not found!\n";

    if (Judge_File_Type(URI, File_err_type) == ERROR) {
        cout << "The request file's type from client's request message is error!\n" << endl;
        return ERROR;
    }

    if (Send_Info(Socket, File_err_line, strlen(File_err_line)) == ERROR) {
        cout << "Sending file_error_line error!" << endl;
        return ERROR;
    }

    cur_time = Get_Current_Time(cur_time);
    Send_Info(Socket, "Date: ", 6);
    if (Send_Info(Socket, cur_time, strlen(cur_time)) == ERROR) {
        cout << "Sending cur_time error!" << endl;
        return ERROR;
    }

    // ������Ӧͷ�е�Content-Type
    File_err_type = Judge_File_Type(URI, File_err_type);
    cout << "     File type: " << File_err_type << endl;
    if (Send_Info(Socket, File_err_type, strlen(File_err_type)) == ERROR) {
        cout << "Sending file_error_type error!" << endl;
        return ERROR;
    }

    if (Send_Info(Socket, File_err_end, strlen(File_err_end)) == ERROR) {
        cout << "Sending file_error_end error!" << endl;
        return ERROR;
    }

    if (Send_Info(Socket, File_err_info, strlen(File_err_info)) == ERROR) {
        cout << "Sending file_error_info failed!" << endl;
        return ERROR;
    }

    return OK;
}

int main(int argc, char* argv[]){
    int res = 0;
    int addrLen = 0;
    char revbuf[BUF_SIZE];

    /* ServerSock ��ʼ���׽�����������һֱ�ڶ˿���listen
       NewSock: accept֮����µ��׽���������*/
    SOCKET ServerSock, NewSock;
    struct sockaddr_in ClientAddr;

    cout << "Elvira's Web Server is Starting!" << endl;
    ServerSock = InitServerSocket();
    //printf("argv[0]:%s\n", argv[0]);//ȫ·��

    while (true){  //Ҫʱ�̼���socket���������http�����Ա㼰ʱ��Ӧ
        //listen �����ͻ��˵�socket��������
        res = listen(ServerSock, BACKLOG);
        if (res == SOCKET_ERROR) {
            cout << "Listen Socket Failed:" << WSAGetLastError() << endl;
            return -1;
        }
        cout << "*************************************************************" << endl;
        cout << "Listening the socket ......" << endl;
        cout << endl;

        // ���ܿͻ������������� 
        addrLen = sizeof(struct sockaddr);
        /* accept()����һ���µ��׽���������������������ʹ����˺�Զ�̼����������
            ���µ��׽����������Ϳ��Խ���send()������recv()���� */
        NewSock = accept(ServerSock, (SOCKADDR*)&ClientAddr, &addrLen);
        if (NewSock == INVALID_SOCKET) {
            printf("Failed to accept connection from client!\n");
            system("pause");
            exit(1);
        }
        cout << "Succeed to accept connection from [" << inet_ntoa(ClientAddr.sin_addr) << ":" << ntohs(ClientAddr.sin_port) << "]" << endl;
        
        // ���ܿͻ��˵�http��������
        memset(revbuf, 0, BUF_SIZE);	//ÿһ���ֽڶ���0����� 
        res = recv(NewSock, revbuf, BUF_SIZE, 0);
        if (res <= 0 || res == SOCKET_ERROR)
            cout << "Failed to receive request message from client!" << endl;
        else {
            cout << "Request Message From Client:"<< endl;  //���������������
            cout << "    " << revbuf;
            res = Request_Message_Handler(revbuf, NewSock, argv[0]);
        }
        closesocket(NewSock);
    }
    closesocket(ServerSock);
    WSACleanup();
    return OK;
}
