#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;

#define TIMEOUT 3000  //���峬ʱΪ3��
//����ICMP����ͷ���ṹ��
#pragma comment( lib, "ws2_32.lib" )   

const int ICMP_DATA_SIZE = 32;
const int MAX_ICMP_PACKET_SIZE = 1024;
const int ICMP_TIMEOUT = 11;

typedef struct {
	BYTE type; //���ͣ�8λ��1�ֽ�
	BYTE code; //���룺8λ��1�ֽ�
	USHORT cksum; //����ͣ�16λ��2�ֽ�
	USHORT id; // ��ʶ��(�ͱ����йص��߳�ID)��16λ��2�ֽ�
	USHORT seq; //���кţ�16λ��2�ֽ�
}ICMP_HEADER;

//IP��ͷ
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
	//BYTE ttl; //����ʱ��
	in_addr scrIpAddr; //ԴIP��ַ
}DECODE_RESULT;


//��������У��ͺ���
unsigned short checkSum(char* pBuffer, int nLen)
{
	unsigned short nWord;
	unsigned int nSum = 0;
	int i;

	//Make 16 bit words out of every two adjacent 8 bit words in the packet
	//and add them up
	for (i = 0; i < nLen; i = i + 2)
	{
		nWord = ((pBuffer[i] << 8) & 0xFF00) + (pBuffer[i + 1] & 0xFF);
		nSum = nSum + (unsigned int)nWord;
	}

	//Take only 16 bits out of the 32 bit sum and add up the carries
	while (nSum >> 16)
	{
		nSum = (nSum & 0xFFFF) + (nSum >> 16);
	}

	//One's complement the result
	nSum = ~nSum;

	return ((unsigned short)nSum);
}

BOOL DecodeIcmpResponse(char* pBuf, int iPacketSize, DECODE_RESULT& DecodeResult, BYTE ICMP_ECHO_REPLY, BYTE  ICMP_TIMEOUT)
{
	IP_HEADER* piphdr = (IP_HEADER*)pBuf;
	int iphdrlen = piphdr->hdr_len * 4;		//IP���ݱ����ײ�����
	if (iPacketSize < (int)(iphdrlen + sizeof(ICMP_HEADER)))
		return FALSE;

	ICMP_HEADER* ptr_icmp = (ICMP_HEADER*)(pBuf + iphdrlen);
	USHORT usID, usSquNo;
	if (ptr_icmp->type == ICMP_ECHO_REPLY)
	{
		usID = ptr_icmp->id;
		usSquNo = ptr_icmp->seq;
	}
	else if (ptr_icmp->type == ICMP_TIMEOUT)
	{
		char* innerip = pBuf + iphdrlen + sizeof(ICMP_HEADER);
		int inneriplen = ((IP_HEADER*)innerip)->hdr_len * 4;
		ICMP_HEADER* innericmp = (ICMP_HEADER*)(innerip + inneriplen);
		usID = innericmp->id;
		usSquNo = innericmp->seq;
	}
	else
		return false;


	if (usID != (USHORT)GetCurrentProcessId() || usSquNo != DecodeResult.seqNum)
		return false;

	DecodeResult.scrIpAddr.S_un.S_addr = piphdr->src_ip;
	DecodeResult.roundTripTime = GetTickCount64() - DecodeResult.roundTripTime;//��������ʱ��

	if (ptr_icmp->type == ICMP_ECHO_REPLY || ICMP_TIMEOUT)
	{
		if (DecodeResult.roundTripTime) {
			cout << "	" << DecodeResult.roundTripTime << "ms" << flush;
		}
		else
			cout << "	" << "1ms" << flush;
	}
	return true;
}

int main() {
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
	u_long ulDestIP = inet_addr(DestIp.c_str());

	if (ulDestIP == INADDR_NONE) { 	//�������IP��ַ����ʧ��
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

	char IcmpSendBuf[ICMP_DATA_SIZE + sizeof(ICMP_HEADER)] = { 0 };		//���巢�ͻ�����
	char IcmpRecvBuf[MAX_ICMP_PACKET_SIZE] = { 0 };				//������ջ�����

	//��ʼ��ICMP����ͷ��
	//����������һ���ڴ�ռ䣬Ȼ���ٶ��ڴ�ռ���в������ѽṹ���еĳ�Ա��ֵ��ȥ
	//���ICMP���ݰ��ĸ����ֶ�
	// ����8������0����ʾ��������(ping����)��
	// ����0������0����ʾ����Ӧ��(pingӦ��)
	ICMP_HEADER* pIcmpHeader = (ICMP_HEADER*)IcmpSendBuf;
	pIcmpHeader->type = 8; //�������ͣ�������������8
	pIcmpHeader->code = 0;
	pIcmpHeader->id = (USHORT)GetCurrentProcessId(); //��ȡ��ǰ���̵� id ���ں����жϸ�icmp Ӧ���ǲ��Ǹ���ǰ���̵ġ�
	memset(IcmpSendBuf + sizeof(ICMP_HEADER), 'E', ICMP_DATA_SIZE);//���ֽڶ��ڴ����г�ʼ��������ICMPͷ������ʼ�������ֶΡ�

	//���ͨ��30��Խ��������·��
	bool quit_flag = false; //�˳�ѭ����־
	int max_hop = 30; //�����������ջ����
	int TTL = 1; //��ʼ�Ĵ��ʱ��Ϊ1 
	USHORT usSeqNo = 0; //�������к�
	DECODE_RESULT DecodeResult;

	while (!quit_flag && max_hop--) {	  //��ʼ׷��·�ɲ���
		//����ÿ�η��͵�IP���ݱ���TTL
		setsockopt(sockRaw, IPPROTO_IP, IP_TTL, (char*)&TTL, sizeof(TTL));
		cout << TTL << flush;
		int request_count = 0;  // �����������ӡ*������

		//ÿһ������3��
		for (int i = 0; i < 3; i++) {
			//seq��cksum ��������ÿ�ζ�Ҫ��
			pIcmpHeader->cksum = 0;
			pIcmpHeader->seq = htons(usSeqNo++);
			pIcmpHeader->cksum = htons(checkSum(IcmpSendBuf, sizeof(ICMP_HEADER) + ICMP_DATA_SIZE));

			//��¼���кź͵�ǰʱ��
			DecodeResult.seqNum = ((ICMP_HEADER*)IcmpSendBuf)->seq;
			DecodeResult.roundTripTime = GetTickCount64();

			// ��������
			if (sendto(sockRaw, IcmpSendBuf, sizeof(ICMP_HEADER) + 32, 0, (sockaddr*)&destSockAddr, sizeof(destSockAddr)) == SOCKET_ERROR) {
				if (WSAGetLastError() == WSAEHOSTUNREACH) {
					cout << "�������ɴ�" << endl;
					exit(0);
				}
			}

			sockaddr_in recvinfo;
			int recvlen = 0; //���յ������ݵ��ֽ���
			int fromlen = sizeof(recvinfo);
			memset(IcmpRecvBuf, 0, sizeof(IcmpRecvBuf));

			//���ܷ��ص�ICMP����Ӧ��
			while (1) {
				/*recvfrom ����
					sockfd����ʶһ���������׽ӿڵ������֡�
					buf���������ݻ�������
					len�����������ȡ�
					flags�����ò�����ʽ��
					from������ѡ��ָ�룬ָ��װ��Դ��ַ�Ļ�������
					fromlen������ѡ��ָ�룬ָ��from����������ֵ��
				*/
				recvlen = recvfrom(sockRaw, IcmpRecvBuf, MAX_ICMP_PACKET_SIZE, 0, (SOCKADDR*)&recvinfo, &fromlen);
				if (recvlen != SOCKET_ERROR) {
					//�ж��ǲ��Ƿ��������̵�ICMP���ģ�����Ǿʹ�ӡ��Ϣ
					if (DecodeIcmpResponse(IcmpRecvBuf, recvlen, DecodeResult, 0, ICMP_TIMEOUT)) {
						if (DecodeResult.scrIpAddr.S_un.S_addr == destSockAddr.sin_addr.S_un.S_addr)
							quit_flag = true;
						break;
					}
				}
				else if (WSAGetLastError() == WSAETIMEDOUT) {
					cout << "\t" << '*' << flush;
					request_count++;
					break;
				}
				else
					break;
			}
		}
		if (request_count == 3) {
			cout << "\t" << "����ʱ" << endl;
		}
		else {
			cout << "\t" << inet_ntoa(DecodeResult.scrIpAddr) << endl;
		}
		//׼��������һ�����ݱ�
		TTL++;
	}
	cout << "�������" << endl;
	closesocket(sockRaw);
	WSACleanup();
	return 0;
}