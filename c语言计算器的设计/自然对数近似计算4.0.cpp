#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#define pi 3.1415926

/*�û�ѡ��������*/
int My_choice(int c) {
	switch (c) {
	case 0: CAL_SINX();
		break;
	case 1:CAL_COSX();
		break;
	case 2:CAL_LNX();
		break;
	}
}

int main��void){
int my_choice,sec_choice;
menu:
printf("***********��ӭʹ����Ȼ�����������Һ����Ľ��Ƽ���ϵͳ��********\n"
	"��ѡ������Ҫ����ĺ������ͣ�\n"
	"sinx:0\n"
	"cosx:1\n"
	"lnx:2\n")
	scanf_s("%d\n", &my_choice);
    My_choice(my_choice);
	system("cls");
	printf("�����ٴ�ѡ�������ͣ��밴1�ص���ҳ��\n");
	scanf_s("%d", &sec_choice);
	if(sec_choice==1)
	goto menu;
	return 0;
}

/*��Ȼ����*/
double CAL_LNX() {
	double a;//aΪָ��

	/*�û�����ָ��*/
	printf("��������Ȼ������ָ����");
    scan:
	scanf_s("%lf", &a);
	if (a <= 0) {
		printf("input error!\n");
		printf("please input another number: \n");
		goto scan;
	}
	printf("����ǣ�%f", Calculation(a));

	/*��Ȼ�����Ľ��Ƽ���*/
	int n, i;
	double x, y = 1, Result = 0;
	double  FENSHU = 0;
	double Calculation(double a) {
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
	return 0;
}

/*sinx�ļ���*/
double CAL_SINX() {
		double a;
		float b;
		printf("�����������");
		scanf_s("%lf", &a);
		b = (float)a;
		a = angle_to_radian(a);
		printf("sin%.2f=%f", b, my_sin(a));

	/*�����뻡�ȵ�ת��*/
	double angle_to_radian(double k) {
		return  k / 180 * pi;
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

	/*����׳�*/
	int fact(int i) {
		if (i <= 1)
			return 1;
		else
			return i * fact(i - 1);
	}
	return 0;
}








