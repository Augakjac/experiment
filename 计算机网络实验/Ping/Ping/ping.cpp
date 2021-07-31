#include <winsock2.h>
#include<ws2tcpip.h>
#include <iostream>
#include<algorithm>

using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#define TIMEOUT 1000  //���峬ʱΪ1��

// ICMP Э��ͷ��
typedef struct{
    BYTE type; //���ͣ�8λ��1�ֽ�
    BYTE code; //���룺8λ��1�ֽ�
    USHORT cksum; //����ͣ�16λ��2�ֽ�
    USHORT id; // ��ʶ��(�ͱ����йص��߳�ID)��16λ��2�ֽ�
    USHORT seq; //���кţ�16λ��2�ֽ�
    unsigned int choose; //ѡ�32λ��4�ֽ�
} ICMP_HEADER; //һ��12�ֽ�

// IP���ݱ��ײ��̶�����
typedef struct{
    BYTE h_len_ver; //IP�汾�ź��ײ����� 1�ֽ�
    BYTE tos; // ��������  1�ֽ�
    USHORT total_len; //IP�����ܳ��ȣ�2�ֽ�
    USHORT identifier; // ��ʶ����2�ֽ�
    USHORT frag_and_flags; //3λ��־��13λƬƫ�ƣ�2�ֽ�
    BYTE ttl; //����ʱ�䣬������� 1�ֽ�
    BYTE proto; //Э�� 1�ֽ�
    USHORT cksum; //IP�ײ�У��ͣ�2�ֽ�
    unsigned long src_ip; //ԴIP��ַ 4�ֽ�
    unsigned long dst_ip; //Ŀ��IP��ַ 4�ֽ�
} IP_HEADER; //һ��20�ֽ�

// ���
typedef struct{
    int seqNum; //��¼���к�
    DWORD roundTripTime; //��¼��ǰʱ��,����ʱ��
    BYTE ttl; //����ʱ��
    in_addr scrIpAddr; //ԴIP��ַ
} DECODE_RESULT;

// ��������
USHORT CalChecksum(USHORT *pBuffer, int nLen) {
    unsigned long cksum = 0;

    // ����ҪУ������ݿ�����16λΪ��λ��������ɣ����ν��ж����Ʒ������
    while (nLen > 1) {
        cksum += *pBuffer++;//�Ѵ�У�������ÿ16λ��λ��ӱ�����cksum��
        nLen -= sizeof(USHORT); //ÿ16λ�����򽫴�У�����ݼ�16
    }
    //��Ϊ����ʱ��16λΪ��λ�����������У���������������ֽڣ���ʱ����У���ʱ��Ҫ���������һ������ֽ�0��
    //�����У���������������ֽڣ� ��ѭ�������Ҫ�����һ���ֽڵ�������֮ǰ������
    if (nLen)//ÿ��iSize-sizeof��USHORT�� ��ʵ���Ǽ�16
        cksum += *(UCHAR*)pBuffer;
    //֮ǰ�Ľ�������˽�λ����Ҫ�ѽ�λҲ���뵽���Ľ���� 0xffff ����ȫ1
    //��λ�н�λ����λ����λ���������д��뱣֤�˸�16λΪ0
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);

    return (USHORT)(~cksum);
}

//��pingӦ����Ϣ���н���
boolean DecodePingResponse(char *pBuf, int packetSize, DECODE_RESULT& stDecodeResult) {
    //��ȡ�յ���IP���ݰ����ײ���Ϣ
    IP_HEADER *pIpHrd = (IP_HEADER*)pBuf; //ָ���յ�Ӧ�����ݵ�ָ��
    int ipHrdLen = 20; //ip���ݱ��ײ��̶�����20�ֽ�
    if (packetSize < (int)(ipHrdLen + sizeof(ICMP_HEADER))) { //ICMP�Ƿ�װ��IP���ĵ����ݲ����з������ģ����Գ�����Ҫ����IP�ײ���ICMP�ײ��ĳ���
        cout << "Ӧ�����ݳ�������,���ݿ��ܶ�ʧ��" << endl;
        return false;
    }

    //ָ��ָ��ICMP���ĵ��׵�ַ
    ICMP_HEADER* pIcmpHrd = (ICMP_HEADER*)(pBuf + ipHrdLen);//�����ƶ���20���ֽ�
    // cout<<pIcmpHrd->type<<","<<pIcmpHrd->code<<","<<pIcmpHrd->cksum<<","<<pIcmpHrd->id<<","<<pIcmpHrd->seq<<","<<pIcmpHrd->choose<<endl;
    USHORT usID, usSeqNum;
    //��õ����ݱ���type�ֶ�Ϊ0�����յ�һ������Ӧ��ICMP����
    if (pIcmpHrd->type == 0) {
        usID = pIcmpHrd->id;
        //���ܵ����������ֽ�Ϊseq�ֶ���Ϣ��Ҫת��Ϊ�����ֽ�˳��
        usSeqNum = ntohs(pIcmpHrd->seq);
    }
    if (usID != GetCurrentProcessId() || usSeqNum != stDecodeResult.seqNum) return false;

    //��¼�Է�������IP��ַ�Լ�����������ʱRTT
    if (pIcmpHrd->type == 0) {
        stDecodeResult.ttl = pIpHrd->ttl;
        stDecodeResult.scrIpAddr.s_addr = pIpHrd->src_ip;
        stDecodeResult.roundTripTime = (DWORD)GetTickCount64() - stDecodeResult.roundTripTime;
        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    string DestIp;  //�ṹ�����ip��ַ��������
    int timeSum = 0;   // �ܺ�ʱ
    int maxTime = 0;   // ���ʱ
    int minTime = 999999;   // ��̺�ʱ
    int recv = 0;    // �հ�������
    int lost = 0;    // ����������
    int send = 0;    // ����������
    string IP_show;
    WSADATA wsadata;  //��ʼ���׽��֣�IP��ַ���˿ںţ�
    //��ʼ�������� gethostbyname �ò������
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {   //ʹ��2.2�汾��Socket
        cout<<"INITIAL  FAILED!!!"<<endl;
    }

    cout << "-------��ӭʹ��MyPing----------" <<endl;
    cout << "ping >> " ;
    cin >> DestIp;
    cout << endl;

    //internet�������׽��ֵĵ�ַ��ʽ
    sockaddr_in mysock;   /* ������������ --> ��ַ�أ�sin_family; 16λTCP/UDP�˿ںţ�sin_port;  32λIP��ַ��sin_addr; */

    //������������Ĳ���ת��ΪIP��ַ
    u_long ul_DestIP = inet_addr(DestIp.c_str()); //��һ�����ʮ���Ƶ�IPת���������ֽ��������ֵ
    IP_show = DestIp.c_str();
    if (ul_DestIP == INADDR_NONE) {       //INADDR_NONE:�궨�壬������Ч��IP��ַ
        //ת�����ɹ���ʱ��ʹ����������,����һ��������ַ�ṹ�壬hostent
        hostent *pHostent = gethostbyname(DestIp.c_str());
        //cout << "pHostent"<<pHostent << endl;
        if (pHostent) {
            //������������ȡ�õ������ĵ�ַ
            for (int n = 0; pHostent->h_addr_list[n]; n++) {
                memcpy(&mysock.sin_addr.s_addr, pHostent->h_addr_list[n], pHostent->h_length);  //��Դsource�п���n���ֽڵ�Ŀ��destin��
                IP_show = inet_ntoa(mysock.sin_addr);  //�����ֽ����ַ->��׼�ĵ��ʮ���Ƶ�ַ
            }
            ul_DestIP = (*(in_addr*)pHostent->h_addr_list[1]).s_addr;  //�����ʮ���ƹ��ɵ�ip��ַת����ʮ�������ֺ��ֵ
            //cout << "DestIp:" << ul_DestIP << endl;
            cout << "���� Ping " << pHostent->h_name << " [" << IP_show << "] " << "����" << " 32 " << "�ֽڵ����ݣ�" << endl;
        }

        else {
            cout << "Ping �����Ҳ�������" << DestIp << "����������ĵ�ַ��" << endl;
            WSACleanup();
            exit(0);
        }
    }

    //���Ŀ��Socket��ַ
    sockaddr_in destSockAddr; //Ŀ��socket��ַ
    ZeroMemory(&destSockAddr, sizeof(sockaddr_in)); //��ʼ������0���
    destSockAddr.sin_family = AF_INET; //ָ����ַ����ʹ��IPv4
    destSockAddr.sin_addr.s_addr = ul_DestIP; //Ŀ��IP��ַ

    //ʹ��ICMPЭ�鴴�� RAW Socket
    //WSASocket �����󶨵��ض���������ṩ�ߵ��׽��֡�
    //                          IPV4 , ԭʼ�׽���,  ICMPЭ�� ��           ,ָ��socket������Ϊ�ص��׽���
    SOCKET sock_raw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sock_raw == INVALID_SOCKET) { 
        cout << "Create Socket Defeat!" << endl;
        return 0;
    }

    /*���ó�ʱ��ֹ
        setsockopt����(������ĳ���׽��ֹ�����ѡ��)�Ĳ���:  
        s : �׽��ֱ�ʶ������
        level : ����ѡ��ļ���
        optname : ���õ��׽���ѡ���ֵ�������׽��ּ�����
        optval: ָ�򻺳�����ָ��
        optlen: ָ��ָ��Ļ������Ĵ�С */
    int timeout = TIMEOUT;//���ó�ʱΪ1��

    //�趨�׽��ֵĳ�ʱʱ�䣬��ֹ���պͷ�����Ϣ��ʱ��������ʹ�ó����ܽ���
    //���ý���ʱ�ޣ��Ժ���Ϊ��λ��
    if (setsockopt(sock_raw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cout<<"setsockopt SO_RCVTIMEO error!"<<endl;
        return 0;
    }
    //���÷���ʱ�ޣ��Ժ���Ϊ��λ����
    if (setsockopt(sock_raw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cout<<"setsockopt SO_SNDTIMEO error!"<<endl; 
        return 0;
    }

    //���巢�͵����ݰ�
    char IcmpSendPacket[32];  //32���ֽ� �����ŷ��͵�ICMP���ݱ�����Ϣ
    // ���ICMP���ݰ��ĸ����ֶ�
    // ����8������0����ʾ��������(ping����)��
    // ����0������0����ʾ����Ӧ��(pingӦ��)
    ICMP_HEADER *pIcmpHeader = (ICMP_HEADER*)IcmpSendPacket;
    pIcmpHeader->type = 8;//������������8
    pIcmpHeader->code = 0;
    pIcmpHeader->id = (USHORT)GetCurrentProcessId(); //��ȡһ�µ�ǰ���̵� id ���ں����жϸ�icmp Ӧ���ǲ��Ǹ���ǰ���̵ġ�
    memset(IcmpSendPacket + sizeof(ICMP_HEADER), 'w', 20);//���ֽڶ��ڴ����г�ʼ��������ICMPͷ������ʼ�������ֶΡ�

    //ѭ������4��icmp����������ݱ�
    DECODE_RESULT stDecodeResult;
    for (int i = 0; i <= 3; i++) {
        //seq��cksum ��������ÿ�ζ�Ҫ��
        pIcmpHeader->seq = htons(i); //htons ������ת��Ϊ�����ֽ���
        pIcmpHeader->cksum = 0; //����Ҳ��������
        // pIcmpHeader->cksum = CalChecksum((USHORT*)IcmpSendBuf,sizeof(ICMP_HEADER)+20);
        pIcmpHeader->cksum = CalChecksum((USHORT*)IcmpSendPacket, sizeof(IcmpSendPacket));

        //��¼���кź͵�ǰʱ��
        stDecodeResult.seqNum = i;
        stDecodeResult.roundTripTime = (DWORD)GetTickCount64();

        /*����ICMP �� EchoRequset ���ݰ� ��������
        sendto����������
                s �׽���
                buff ���������ݵĻ�����
                size ����������
                Flags ���÷�ʽ��־λ, һ��Ϊ0, �ı�Flags������ı�Sendto���͵���ʽ
                addr ����ѡ��ָ�룬ָ��Ŀ���׽��ֵĵ�ַ
                len   addr��ָ��ַ�ĳ���
        */
        if (sendto(sock_raw, IcmpSendPacket, sizeof(IcmpSendPacket), 0, (sockaddr*)&destSockAddr, sizeof(destSockAddr)) == SOCKET_ERROR) {
            cout << "Send error!" << WSAGetLastError()<< endl;
            exit(0);         
        }
        // �����ɹ�
        send++;

        //���ܷ��ص�ICMP����Ӧ��
        char IcmpRecvPacket[1024];
        sockaddr_in from_add; //���ܵ������ݱ���ַ
        int iFromLen = sizeof(from_add);
        int iReadLen;
        int t; //���ĵ�ʱ��
        
        while (1) {
            /*recvfrom ����
                sockfd����ʶһ���������׽ӿڵ������֡�
                buf���������ݻ�������
                len�����������ȡ�
                flags�����ò�����ʽ��
                from������ѡ��ָ�룬ָ��װ��Դ��ַ�Ļ�������
                fromlen������ѡ��ָ�룬ָ��from����������ֵ��
            */
            iReadLen = recvfrom(sock_raw, IcmpRecvPacket, 1024, 0, (sockaddr*)&from_add, &iFromLen);
            if (iReadLen != SOCKET_ERROR) {
                //�ж��ǲ��Ƿ��������̵�ICMP���ģ�����Ǿʹ�ӡ��Ϣ
                if (DecodePingResponse(IcmpRecvPacket, sizeof(IcmpRecvPacket), stDecodeResult)) {
                    printf("���� %s �Ļظ�: �ֽ� = %d ʱ�� = %dms TTL = %d\n", inet_ntoa(stDecodeResult.scrIpAddr), iReadLen - 20, stDecodeResult.roundTripTime, stDecodeResult.ttl);
                    // ��������ʱ��
                    t = stDecodeResult.roundTripTime;
                    if (t > maxTime) {
                        maxTime = t;
                    }
                    if (t < minTime) {
                        minTime = t;
                    }
                    timeSum += stDecodeResult.roundTripTime;
                    recv++;
                }
                break;
            }
            else if (WSAGetLastError() == WSAETIMEDOUT) {
                cout << "����ʱ!" << endl;
                lost++;
                break;
            }
            else {
                cout << "Unknown Error!" << endl;
                lost++;
                break;
            }
        }
    }
    //�����Ϣ
    cout << "\nPing complete. \n" << endl;
    cout << IP_show << " �� Ping ͳ����Ϣ:" << endl;
    cout << "    ���ݰ�: �ѷ��� = " << send << "���ѽ��� = " << recv << "����ʧ = " << lost << "(" << (lost/send)*100 <<"%)"<< endl;
    if (recv != 0) {
        cout << "�����г̵Ĺ���ʱ��(�Ժ���Ϊ��λ):" << endl;
        cout << "    ��� = " << minTime << "ms��� = " << maxTime << "ms��ƽ�� = " << (int)(timeSum / 4) << "ms" << endl;
    }
    closesocket(sock_raw);
    WSACleanup();   //�������绷��,�ͷ�socket��ռ����Դ
    return 0;
}