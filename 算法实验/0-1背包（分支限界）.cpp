#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

int N;  //��Ʒ����
int W;  //����������

// ��Ʒ�Ľṹ��
struct Item{
	int itemId; //��Ʒ���
	int weight; //����
	int val;    //��ֵ
	int ratio;  //��ֵ������
};

// ״̬�ռ����Ľڵ�
struct Node {
	int val;  //�ý��ļ�ֵ
	int weight; //�ý���������
	float bound;  //�ýڵ�ļ�ֵ�Ͻ�
	int level;  //���
	struct Node* parent; //���ڵ�

	// ��ʼ���ڵ�
	Node() {
		val = 0;
		weight = 0;
		level = 0;
		parent = 0;
		bound = 0;
	}
};

//���մ󶥶ѵ���ʽ���
struct cmp{
	bool operator()(Node* a, Node* b) {
		return a->bound > b->bound;  // ����bound�Ӵ�С���У��ȴ���bound��Ľڵ�,���޸���װ�Ķ������ܾ�Խ��
	}
};

bool compare(Item item1, Item item2);
int branchAndBound(Item items[], int W);
float maxBound(Node* node, Item items[], int c);


int main(int argc, char** argv) {
	int maxVal;  //������װ������ֵ
	cout << "��������Ʒ�ĸ���:";
	cin >> N;
	cout << "�����뱳������:";
	cin >> W;

	int* w = new int[N];
	int* v = new int[N];

	cout << "����������" << N << "����Ʒ��������";
	for (int i = 0; i < N; i++) {
		cin >> w[i];
	}
	cout << "����������" << N << "����Ʒ�ļ�ֵ��";
	for (int i = 0; i < N; i++) {
		cin >> v[i];
	}
	cout << endl;
	Item* items = new Item[N];
	// ��ʼ����Ʒ�ṹ������
	for (int i = 0; i < N; i++) {
		items[i].itemId = i;
		items[i].weight = w[i];
		items[i].val = v[i];
		items[i].ratio = float(v[i] / w[i]);
	}

	// ����ֵ����������
	sort(items, items + N, compare);

	cout << "ѡȡ�ķ���Ϊ��" << endl;
	maxVal = branchAndBound(items,W);
	cout << "������װ������ֵΪ:" << maxVal;
}

// �Ƚϴ�С
bool compare(Item item1, Item item2) {
	return item1.ratio > item2.ratio;  // ����ratio�Ӵ�С����
}

// ��֧�޽纯��
int branchAndBound(Item items[], int W) {
	int maxVal = 0;  //����ֵ
	//x �����¼����������Ƿ�װ�뱳����װ��ֵΪ1��Ϊװ��Ϊ0
	int* x = new int[N];
	for (int i = 0; i < N; i++){
		x[i] = 0;  
	}

	Node* maxNode = new Node();  //��ǰ����ֵ�Ľڵ�
	//             ��������|��������| �ȽϷ�ʽ���󶥶ѣ�
	priority_queue<Node*, vector<Node*>, cmp> maxQueue; //���ȶ���,����Ϊ��ѡ�ڵ�
	Node* rootNode, * curNode; // ���ڵ㣬��ǰ�ڵ�

	rootNode = new Node();
	rootNode->bound = maxBound(rootNode, items, W);
	rootNode->parent = NULL;
	maxQueue.push(rootNode);
	maxVal = 0;
	maxNode = rootNode;
	while (!maxQueue.empty()) {   // ��ѡ�ڵ㲻Ϊ��
		curNode = maxQueue.top();
		maxQueue.pop();

		// ��չ����
		if (curNode->weight + items[curNode->level].weight <= W) {
			Node* leftNode = new Node();
			leftNode->val = curNode->val + items[curNode->level].val;
			leftNode->weight = curNode->weight + items[curNode->level].weight;
			leftNode->level = curNode->level + 1;
			leftNode->parent = curNode;
			leftNode->bound = maxBound(leftNode, items, W);
			if (leftNode->level < N) {
				maxQueue.push(leftNode);  //����Ҫ��ĵ�������
			}
			if (leftNode->val > maxVal) {
				maxVal = leftNode->val;
				maxNode = leftNode;
			}
		}

		// ��չ�Һ��ӽڵ�
		if (maxBound(curNode, items, W) > maxVal) {
			Node* rightNode = new Node();
			rightNode->val = curNode->val;
			rightNode->weight = curNode->weight;
			rightNode->level = curNode->level + 1;
			rightNode->parent = curNode;
			rightNode->bound = maxBound(rightNode, items, W);
			if (rightNode->level < N) {
				maxQueue.push(rightNode);
			}
			if (rightNode->val > maxVal) {
				maxVal = rightNode->val;
				maxNode = rightNode;
			}
		}
	}

    // ��ѡ�ڵ�Ϊ����
	curNode = maxNode;
	while (curNode) {
		int tempVal = curNode->val;
		curNode = curNode->parent;
		if (curNode && curNode->val != tempVal) {   //˵���ýڵ��Ǹ��ڵ������
			x[items[curNode->level].itemId] = 1;   // ��¼�ý�㱻װ���˱���
		}
	}

	cout << "��Ʒ��װ�뱳���������1Ϊװ��������0Ϊδװ��������" << endl;
	for (int i = 0; i < N; i++) {
		cout << x[i] << " ";
	}
	cout << endl;
	return maxVal;
}

float maxBound(Node* node, Item items[], int W) {
	// node �ǵ�ǰ��Ҫ�жϵĽڵ�
	float upperBound; //�Ͻ� 
	int rest;  //ʣ������
	int i;  //�ڼ�����Ʒ

	upperBound = node->val;
	rest = W - node->weight;
	i = node->level;

	if (rest >= 0) {
		upperBound += rest * items[i].ratio;
	}

	return upperBound;
}