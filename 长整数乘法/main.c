#include<stdio.h>
#include<string.h>
#define N 100

int main()
{
	int Result[N];
	char mul_1[N], mul_2[N];
	scanf("%s %s", mul_1, mul_2);
	int len_1 = strlen(mul_1);
	int len_2 = strlen(mul_2);
	memset(Result, 0, sizeof(Result));//初始化为0

	for(int i = 0; i < len_1; i++)
		for(int j = 0; j < len_2; j++)
        //下面一行最关键！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！
			Result[i+j] += (mul_1[len_1 - i - 1] - '0') * (mul_2[len_2 - j - 1] - '0');//乘运算，注意Result数组是倒序的，便于进位
        //上面一行最关键！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！

    int i;
	for(i = 0; i < len_1 + len_2; i++)
		if(Result[i] >= 10)
		{
			Result[i+1] += Result[i] / 10;//进位
			Result[i] %= 10;//取余
		}
    //注意此时i的位置是len1+len2-1,不在最高位的位置
	while(Result[i] == 0&&i!=0)//所以要去零，直至i抵达最高位
		i--;
	//逆序输出答案
	while(i >= 0)
		printf("%d", Result[i--]);
	return 0;
}
