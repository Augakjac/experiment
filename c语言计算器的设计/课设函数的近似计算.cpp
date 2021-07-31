#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#define pi 3.1415926
double CAL_LNX();
double CAL_SINX();
double CAL_COSX();
double Calculation(double a);
double angle_to_radian(double k);
double my_sin(double x);
double my_cos(double x1);
double My_choice(int c);
int fact(int i);


/*��Ȼ����*/
double CAL_LNX() {
	double a;//aΪָ��
	printf("***************��ӭʹ����Ȼ�����Ľ��Ƽ���ϵͳ��***************\n");
	printf("ln");
    scan:
	scanf_s("%lf", &a);
	if (a <= 0) {
		printf("input error!\n");
		printf("please input another number: \n");
		goto scan;
	}
	printf("����ǣ�ln%f=%f\n",a, Calculation(a));
	return 0;
}

/*��Ȼ�����Ľ��Ƽ���*/
double Calculation(double a) {
	int n, i;
	double x, y = 1, Result = 0;
	double  FENSHU = 0;
	x = (double)(a - 1) / (a + 1);
	for (n = 1;;n++) {
		FENSHU = (double)1 / (2 * n + 1) * pow(pow(x, 2), n);
		y = y + FENSHU;

		/*������*/
		if (FENSHU < 0.000001) {
			Result = 2 * x * y;
			return Result;
		}
	}
}

/*sinx�ļ���*/
double CAL_SINX() {
	double a;
	float b;
	printf("***************��ӭʹ�����ҵĽ��Ƽ���ϵͳ��***************\n");
	printf("sin");
	scanf_s("%lf", &a);
	b = (float)a;
	a = angle_to_radian(a);
	printf("sin%.2f=%f\n", b, my_sin(a));
	return 0;
}

/*sina�Ľ��Ƽ���*/
double my_sin(double x) {
	double fenzi, fenmu, fore, y = 0;
	int n, f = 0;
	for (n = 1;;n += 2) {
		fenzi = pow(x, n);
		fore = pow((-1), f++);
		fenmu = fact(n);
		y = y + (double)fore * fenzi / fenmu;
		if ((double)fenzi / fenmu < 1e-6)
			break;
	}
	return y;
}

/*�����뻡�ȵ�ת��*/
double angle_to_radian(double k) {
	return  k / 180 * pi;
}

/*����׳�*/
int fact(int i) {
	if (i <= 1)
		return 1;
	else
		return i * fact(i - 1);
}

/*cosx*/
double CAL_COSX() {
	double a1;
	float b1;
	printf("***************��ӭʹ�����ҵĽ��Ƽ���ϵͳ��***************\n");
	printf("cos");
	scanf_s("%lf", &a1);
	b1 = (float)a1;
	a1 = angle_to_radian(a1);
	printf("cos%.2f=%f\n", b1, my_cos(a1));
	return 0;
}

/*���Ƽ���cosa1*/
double my_cos(double x1) {
	double fenzi, fenmu, fore, y = 0;
	int n, f = 0;
	for (n = 0;;n += 2) {
		fenzi = pow(x1, n);
		fore = pow((-1), f++);
		fenmu = fact(n);
		y = y + (double)fore * fenzi / fenmu;
		if ((double)fenzi / fenmu < 1e-6)
			break;
	}
	return y;
}

/*�û�ѡ��������*/
double My_choice(int c) {
	switch (c) {
	case 0: CAL_SINX();
		break;
	case 1:CAL_COSX();
		break;
	case 2:CAL_LNX();
		break;
	}
	return 0;
}

int main(void) {
	int my_choice, sec_choice;
menu:
	printf("   **********��ӭʹ����Ȼ�����������Һ����Ľ��Ƽ���ϵͳ��*********\n"
		   "   **                ��ѡ������Ҫ����ĺ������ͣ�               **\n"
		   "   **                sinx:0                                     **\n"
		   "   **                cosx:1                                     **\n"                                  
		   "   **                lnx:2                                      **\n"
	       "   ***************************************************************\n");
	scanf_s("%d", &my_choice);
	system("cls");
	My_choice(my_choice);
	printf("�����ٴ�ѡ�������ͣ��밴1�ص���ҳ��\n");
	scanf_s("%d", &sec_choice);
	system("cls");
	if (sec_choice == 1)
		goto menu;
	return 0;
}








