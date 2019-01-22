
#ifndef __POW_FUNCTION_H__
#define __POW_FUNCTION_H__

#include <iostream>

//double pow_i(double num, int n)//计算num的n次幂，其中n为整数 
//{
//	double powint = 1;
//	int i;
//	for (i = 1; i <= n; i++) powint *= num;
//	return powint;
//}
//double pow_f(double num, double m)//计算num的m次幂，num和m可为双精度，num大于零 
//{
//	int i, j;
//	double powf = 0, x, tmpm = 1;
//	x = num - 1;
//	for (i = 1; tmpm>1e-12 || tmpm<-1e-12; i++)//当tmpm不在次范围时，停止循环,范围可改 
//	{
//		for (j = 1, tmpm = 1; j <= i; j++)
//			tmpm *= (m - j + 1)*x / j;
//		powf += tmpm;
//	}
//	return powf + 1;
//}
double pow_ff(double num, double m)//调用pow_f()和pow_i(),计算num的m次幂,是计算幂的入口
{
	if (num == 0 && m != 0) return 0;//若num为0，则返回0
	else if (num == 0 && m == 0) return 1;// 若num和m都为0，则返回1
	else if (num<0 && m - int(m) != 0) return 0;//若num为负，且m不为整数，则出错，返回0 

	if (num>2)//把底数大于2的情况转为(1/num)^-m计算
	{
		num = 1 / num;
		m = -m;
	}
	if (m < 0) {//把指数小于0的情况转为1/num^-m计算
		m = -m;
		int i, j;
		double powf = 0, x, tmpm = 1;
		x = num - 1;
		for (i = 1; tmpm>1e-12 || tmpm<-1e-12; i++)//当tmpm不在次范围时，停止循环,范围可改 
		{
			for (j = 1, tmpm = 1; j <= i; j++)
				tmpm *= (m - j + 1)*x / j;
			powf += tmpm;
		}
		return 1 / (powf + 1);
	}
	/*当指数为浮点数时，分成整数和小数分别求幂，这是因为当底数较小时，用pow_f直接求幂误差大，所以分为指数的整数部分用pow_i,小
	数部分用pow_f求.*/
	if (m - int(m) == 0) {
		double powint = 1;
		int i;
		for (i = 1; i <= int(m); i++) powint *= num;
		return powint;
	}
	else {
		//小数部分
		int i, j;
		double powf = 0, x, tmpm = 1;
		x = num - 1;
		for (i = 1; tmpm>1e-12 || tmpm<-1e-12; i++)//当tmpm不在次范围时，停止循环,范围可改 
		{
			for (j = 1, tmpm = 1; j <= i; j++)
				tmpm *= (m - int(m) - j + 1)*x / j;
			powf += tmpm;
		}
		//整数部分
		double powint = 1;
		int k;
		for (k = 1; k <= int(m); k++) powint *= num;
		return (powf + 1) * powint;
	}

	int i, j;
	double powf = 0, x, tmpm = 1;
	x = num - 1;
	for (i = 1; tmpm>1e-12 || tmpm<-1e-12; i++)//当tmpm不在次范围时，停止循环,范围可改 
	{
		for (j = 1, tmpm = 1; j <= i; j++)
			tmpm *= (m - j + 1)*x / j;
		powf += tmpm;
	}
	return powf + 1;
}
#endif
