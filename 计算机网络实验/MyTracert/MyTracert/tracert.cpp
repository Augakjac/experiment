#include <winsock2.h>   
#include<ws2tcpip.h>
#include <iostream>
#include <iomanip>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#define TIMEOUT 3000  //���峬ʱΪ3��


// ICMP Э��ͷ��
typedef struct {
    BYTE type; //���ͣ�8λ��1�ֽ�
    BYTE code; //���룺8λ��1�ֽ�
    USHORT cksum; //����ͣ�16λ��2�ֽ�
    USHORT id; // ��ʶ��(�ͱ����йص��߳�ID)��16λ��2�ֽ�
    USHORT seq; //���кţ�16λ��2�ֽ�
    unsigned int choice; //ѡ�32λ��4�ֽ�
} ICMP_HEADER; //һ��12�ֽ�

// IP���ݱ��ײ��̶�����
typedef struct {
    char hdr_len : 4;//4λͷ������
    char version : 4;//���Ȱ汾��
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
typedef struct {
    int seqNum; //��¼���к�
    DWORD roundTripTime; //��¼��ǰʱ��,����ʱ��
    BYTE ttl; //����ʱ��
    in_addr scrIpAddr; //ԴIP��ַ
} DECODE_RESULT;

// ��������
USHORT CalChecksum(USHORT* pBuffer, int nLen) {
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
BOOL DecodeIcmpResponse(char* pBuf, int packetSize, DECODE_RESULT& stDecodeResult) {
    //��ȡ�յ�IP���ݰ����ײ���Ϣ
    IP_HEADER* pIpHrd = (IP_HEADER*)pBuf; //ָ���յ�Ӧ�����ݵ�ָ��
    int ipHrdLen = pIpHrd->hdr_len * 4; //ip��ͷ�����ֽ�Ϊ��λ��
    if (packetSize < (int)(ipHrdLen + sizeof(ICMP_HEADER))) { //ICMP�Ƿ�װ�����ݱ��з������ģ����Գ�����Ҫ����IP�ײ���ICMP�ײ��ĳ���
        cout << "Ӧ�����ݳ�������" << endl;  return false;
    }

    //ָ��ָ��ICMP���ĵ��׵�ַ
    ICMP_HEADER* pIcmpHrd = (ICMP_HEADER*)(pBuf + ipHrdLen);//�����ƶ���20���ֽ�
    USHORT usID, usSeqNum;
    if (pIcmpHrd->type == 0) {
        usID = pIcmpHrd->id;
        usSeqNum = pIcmpHrd->seq;
    }
    //ICMP��ʱ�����,�ڰ���Դ��ַ���Լ�IP���ݰ���·����Ϣ
    else if (pIcmpHrd->type == 11) {//��ʱ����
        //�غ��е�IPͷ
        char* pInnerIpHdr = pBuf + ipHrdLen + sizeof(ICMP_HEADER);
        //�غ��е�IPͷ��
        int iInnerIPHdrLen = ((IP_HEADER*)pInnerIpHdr)->hdr_len * 4;
        //�غ��е�ICMPͷ
        ICMP_HEADER* pInnerIcmpHdr = (ICMP_HEADER*)(pInnerIpHdr + iInnerIPHdrLen);
        //����ID
        usID = pInnerIcmpHdr->id;
        //���к�
        usSeqNum = pInnerIcmpHdr->seq;
    }
    else
        return false;

    if (usID != (USHORT)GetCurrentProcessId() || usSeqNum != stDecodeResult.seqNum) return false;

    //������ȷ�յ���ICMP����
    if (pIcmpHrd->type == 0 || pIcmpHrd->type == 11) {
        //���ؽ�����
        stDecodeResult.scrIpAddr.s_addr = pIpHrd->src_ip;
        stDecodeResult.roundTripTime = GetTickCount64() - stDecodeResult.roundTripTime;

        //��ӡ��Ļ��Ϣ��
        if (stDecodeResult.roundTripTime) {
            cout << "\t" << stDecodeResult.roundTripTime << "ms" << flush;
        }
        else
            cout << "\t" << "1ms" << flush;

        return true;
    }
    return false;
}

int main(int argc, char** argv) {
    string DestIp;  //�ṹ�����ip��ַ��������

    cout << "-------��ӭʹ��MyTracert----------" << endl;
    cout << "tracert >> ";
    cin >> DestIp;

    //��ʼ���׽���
    WSADATA wsadata;
    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        cout << "INITIAL  FAILED!!!" << endl;

    sockaddr_in mysock;   /*�ṹ�� ����һ���������� --> ��ַ�أ�sin_family;     16λTCP/UDP�˿ںţ�sin_port;    32λIP��ַ��sin_addr; */
    string IP_show;
    string domain_name;   //����

    //���ַ�����ʽ��IP��ַת��Ϊ�����з��������u_long����
    u_long ulDestIP = inet_addr(DestIp.c_str()); //��һ�����ʮ���Ƶ�IPת����һ������������

    if (ulDestIP == INADDR_NONE) {          //INADDR_NONE һ���궨�壬������Ч��IP��ַ
        //ת�����ɹ���ʱ��ʹ����������,����һ��������ַ�ṹ�壬hostent
        hostent* pHostent = gethostbyname(DestIp.c_str());
        if (pHostent) {
            memcpy(&mysock.sin_addr.s_addr, pHostent->h_addr_list[0], pHostent->h_length);
            IP_show = inet_ntoa(mysock.sin_addr);
            domain_name = DestIp.c_str();
            ulDestIP = (*(in_addr*)pHostent->h_addr_list[0]).s_addr;
        }
        else {
            cout << "�޷�����Ŀ��ϵͳ���� " << DestIp << "����������ĵ�ַ��" << endl;
            WSACleanup();
            return 0;
        }
    }

    // �ж����������������IP��ַ
    cout << " " << endl;
    if (empty(domain_name)) {
        cout << "ͨ�����30��Ծ����ٵ� " << IP_show << " ��·��:" << endl;
    }
    else
        cout << "ͨ�����30��Ծ����ٵ� " << domain_name << " [" << IP_show << "] ��·��:" << endl;
    cout << " " << endl;

    //���Ŀ��Socket��ַ
    sockaddr_in destSockAddr; //Ŀ��socket��ַ
    ZeroMemory(&destSockAddr, sizeof(sockaddr_in)); //��ʼ������0���
    destSockAddr.sin_family = AF_INET; //ָ����ַ����ʹ��IPv4
    destSockAddr.sin_addr.s_addr = ulDestIP; //Ŀ��IP��ַ

    //ʹ��ICMPЭ�鴴�� RAW Socket
    //WSASocket �����󶨵��ض���������ṩ�ߵ��׽��֡�
    //                          IPV4 ,ԭʼ�׽���,   ICMPЭ�� ��   ����ָ��socket������Ϊ�ص��׽���
    SOCKET sockRaw = WSASocket(AF_INET, SOCK_RAW, IPPROTO_ICMP, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (sockRaw == INVALID_SOCKET) { 
        cout << "Create Socket Defeat��" << endl; 
        return 0; 
    }

    /*���ó�ʱ��ֹ
    setsockopt�����Ĳ���
    s : �׽��ֱ�ʶ������
    level : ����ѡ��ļ���
    optname : ���õ��׽���ѡ���ֵ�������׽��ּ�����
    optval: ָ�򻺳�����ָ��
    optlen: ָ��ָ��Ļ������Ĵ�С */
    int timeout = TIMEOUT;//���ó�ʱΪ3��
    //��ֹ���պ��г�ʱ���Ժ���Ϊ��λ��
    if (setsockopt(sockRaw, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cout << "setsockopt SO_RCVTIMEO error" << endl;
        return 0;
    }
    //��ֹ���ͺ��г�ʱ���Ժ���Ϊ��λ����
    if (setsockopt(sockRaw, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        cout << "setsockopt SO_SNDTIMEO error" << endl;
        return 0;
    }

    //���巢�͵����ݰ�
    char IcmpSendBuf[sizeof(ICMP_HEADER) + 32];//32���ֽ� �����ŷ��͵�ICMP���ݱ�����Ϣ ��һ���ֽ�8λ
    //���ICMP���ݰ��ĸ����ֶ�
    // ����8������0����ʾ��������(ping����)��
    // ����0������0����ʾ����Ӧ��(pingӦ��)
    //memset(IcmpSendBuf, 0, sizeof(IcmpSendBuf));
    ICMP_HEADER* pIcmpHeader = (ICMP_HEADER*)IcmpSendBuf;
    pIcmpHeader->type = 8; //�������ͣ�������������8
    pIcmpHeader->code = 0;
    pIcmpHeader->id = (USHORT)GetCurrentProcessId(); //��ȡ��ǰ���̵� id ���ں����жϸ�icmp Ӧ���ǲ��Ǹ���ǰ���̵ġ�
    memset(IcmpSendBuf + sizeof(ICMP_HEADER), 'E', 32);//���ֽڶ��ڴ����г�ʼ��������ICMPͷ������ʼ�������ֶΡ�

    //���ͨ��30��Խ��������·��
    bool quit_flag = false; //�˳�ѭ����־
    int max_hop = 30; //�����������ջ����
    int TTL = 1; //��ʼ�Ĵ��ʱ��Ϊ1 
    USHORT usSeqNo = 0; //�������к�
    DECODE_RESULT DecodeResult;

    while (!quit_flag && max_hop--) {
        //����ttl
        setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(TTL));
        cout << TTL << flush; //�����ǰ�ǵڼ���
        //ÿһ������3��
        int request_count = 0;  // �����������ӡ*������
        for (int i = 0; i < 3; i++) {
            //seq��cksum ��������ÿ�ζ�Ҫ��
            pIcmpHeader->seq = htons(usSeqNo++); //htons ������ת��Ϊ�����ֽ���
            pIcmpHeader->cksum = 0; //����Ҳ��������
            pIcmpHeader->cksum = htons(CalChecksum((USHORT*)IcmpSendBuf, sizeof(ICMP_HEADER) + 32));

            //��¼���кź͵�ǰʱ��
            DecodeResult.seqNum = ((ICMP_HEADER*)IcmpSendBuf)->seq; ;
            DecodeResult.roundTripTime = GetTickCount64();
            
            // ��������
            if (sendto(sockRaw, IcmpSendBuf, sizeof(ICMP_HEADER)+32, 0, (sockaddr*)&destSockAddr, sizeof(destSockAddr)) == SOCKET_ERROR) {
                if (WSAGetLastError() == WSAEHOSTUNREACH) {
                    cout << "�������ɴ�" << endl;
                    exit(0);
                }
            }

            //���ܷ��ص�ICMP����Ӧ��
            char IcmpRecvBuf[1024];
            memset(IcmpRecvBuf, 0, sizeof(IcmpRecvBuf));
            sockaddr_in from; //���ܵ������ݱ���ַ
            int fromLen = sizeof(from);
            int recvlen;  //���յ������ݵ��ֽ���
            while (1) {
                /*recvfrom ����
                    sockfd����ʶһ���������׽ӿڵ������֡�
                    buf���������ݻ�������
                    len�����������ȡ�
                    flags�����ò�����ʽ��
                    from������ѡ��ָ�룬ָ��װ��Դ��ַ�Ļ�������
                    fromlen������ѡ��ָ�룬ָ��from����������ֵ��
                */
                recvlen = recvfrom(sockRaw, IcmpRecvBuf, 1024, 0, (sockaddr*)&from, &fromLen);
                if (recvlen != SOCKET_ERROR) {
                    //�ж��ǲ��Ƿ��������̵�ICMP���ģ�����Ǿʹ�ӡ��Ϣ
                    if (DecodeIcmpResponse(IcmpRecvBuf, recvlen, DecodeResult)) {
                        //����Ŀ�ĵأ����finash_loop =true;
                        if (DecodeResult.scrIpAddr.s_addr == destSockAddr.sin_addr.s_addr) {
                            quit_flag = true;
                        }
                        break;
                    }
                }
                else if (WSAGetLastError() == WSAETIMEDOUT) {
                    cout << "\t" << '*' << flush;
                    request_count++;
                    break;
                }
            }
        }
        if (request_count == 3) {
            cout << "\t" << "����ʱ" << endl;
        }
        else {
            cout << "\t" << inet_ntoa(DecodeResult.scrIpAddr) << endl;
        }
        TTL++;
    }
    cout << "�������" << endl;
    closesocket(sockRaw);
    WSACleanup();
    return 0;
}