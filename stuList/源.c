#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
//#include "List.h"
#include "stuList.c"


int main() { 
	LIST* stuList = (LIST*)malloc(sizeof(LIST));
	initList(stuList);
	int option;
menu:	
	showMenu();
	scanf("%d", &option);
	switch (option) {
	case 0:
		return 0;
	//*@创建学生记录链表
	case 1:{
		system("cls");
		//若链表中已存在学生信息，不再开放此功能
		if (stuList->head) {
			printf("链表中已有学生信息！\n");
			break;
		}
		//显示二级菜单
		printf("0返回上级菜单\n");
		printf("1头插法\n");
		printf("2尾插法\n");
		printf("3创建按学号有序链表\n");
		printf("4打开学生信息文件创建链表\n");
		printf("**************************\n");
		printf("请输入相应数字以执行功能：\n");
		int option;
		scanf("%d", &option);
		//头插法
		if (option == 1)
			createStuList_head(stuList);
		//尾插法
		else if (option == 2)
			createStuList_tail(stuList);
		//按学号有序建立链表
		else if (option == 3)
			createStuList_order_id(stuList);
		//打开学生信息文件创建链表
		else if (option == 4)
			createStuList_file(stuList);
		else
			goto menu;
		break;
	}
	//*@打印全部学生记录
	case 2: {
		system("cls");
		printStuList(stuList);
		break;
	}
	//*@插入一条新的学生记录
	case 3: {
		system("cls");
		//显示二级菜单
		printf("0返回上级菜单\n");
		printf("1在指定位序插入\n");
		printf("2在有序链表中插入\n");
		printf("************************\n");
		printf("请输入相应数字以执行功能：\n");
		int option;
		scanf("%d", &option);
		//在指定位序插入
		if (option == 1)
			addStuList_position(stuList);
		//在有序链表中插入
		else if (option == 2)
			addStuList_order_id(stuList);
		else
			goto menu;
		break;
	}
	//*@按条件删除一条学生记录
	case 4: {
		system("cls");
		//显示二级菜单
		printf("0返回上级菜单\n");
		printf("1删除指定位置的记录\n");
		printf("2删除指定学号的记录\n");
		printf("************************\n");
		printf("请输入相应数字以执行功能：\n");
		int option;
		scanf("%d", &option);
		//删除指定位置的记录
		if (option == 1)
			deleteStu_position(stuList);
		//删除指定学号的记录
		else if (option == 2)
			deleteStu_by_id(stuList);
		else
			goto menu;
		break;
	}
	//*@按学号查找学生记录
	case 5: {
		system("cls");
		searchStu(stuList);
		break;
	}
	//*@统计学生信息
	case 6: {
		system("cls");
		statistics(stuList);
		break;
	}
	//*@销毁链表
	case 7: {
		system("cls");
		destrList(stuList);
		printf("销毁成功！\n");
		break;
	}
	//*@将学生信息写入磁盘文件
	case 8: {
		system("cls");
		printf("0返回上级菜单\n");
		printf("1重写学生文件\n");
		printf("2追加学生文件\n");
		printf("3清空学生文件\n");
		printf("************************\n");
		printf("请输入相应数字以执行功能：\n");
		int option;
		scanf("%d", &option);
		if (option == 1)
			reWriteStuListFile(stuList);
		else if (option == 2)
			addStuListFile(stuList);
		else if (option == 3)
			clearStuListFile(stuList);
		break;
	}
	//*@按学号升序排列链表
	case 9: {
		system("cls");
		LIST* p = rearrange(stuList);
		if (!p) 
			printf("请先初始化链表！\n");
		else {
			stuList = p;
			puts("排序成功！");
		}
		  }
	default:
		break;
	}
	system("pause");
	system("cls");
	goto menu;
}

