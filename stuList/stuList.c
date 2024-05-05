#pragma once
#define _CRT_SECURE_NO_WARNINGS 1
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "List.h"
#include "stuList.h"

void createStuList_head(LIST* stuList) {
	//定义变量接收输入信息
	uint64 id;
	char name[10];
	byte sex;
	unsigned age;
	float score;
	system("cls");
	printf("请输入学生学号（输入0以终止录入）:");
	scanf("%llu", &id);
	while (id)
	{
		printf("请输入学生姓名：");
		scanf("%s", name);  getchar();
		printf("请输入学生性别（男1女0）：");
		scanf("%d", &sex);
		printf("请输入学生年龄：");
		scanf("%u", &age);
		printf("请输入学生成绩：");
		scanf("%f", &score);
		STU* stu = createStu(id, name, sex, age, score);
		addList_h(stuList, stu);
		printf("输入成功！\n\n请输入下一位学生学号（输入0以终止录入）:");
	loop_7:
		scanf("%llu", &id);
		if (checkID(stuList, id)) {
			if (checkID(stuList, id)) {
				printf("学号重复！请检查后重新输入学号：");
				goto loop_7;
			}
		}
	}
	printf("录入结束！\n");
	return;
}

void createStuList_tail(LIST* stuList) {
	//定义变量接收输入信息
	uint64 id;
	char name[10];
	byte sex;
	unsigned age;
	float score;
	system("cls");
	printf("请输入学生学号（输入0以终止录入）:");
	scanf("%llu", &id);
	while (id)
	{
		//输入数据
		printf("请输入学生姓名：");
		scanf("%s", name);  getchar();
		printf("请输入学生性别（男1女0）：");
		scanf("%d", &sex);
		printf("请输入学生年龄：");
		scanf("%u", &age);
		printf("请输入学生成绩：");
		scanf("%f", &score);
		STU* stu = createStu(id, name, sex, age, score);
		//尾插插入
		addList_t(stuList, stu);
		printf("输入成功！\n\n请输入下一位学生学号（输入0以终止录入）:");
	loop_5:
		scanf("%llu", &id);
		if (checkID(stuList, id)) {
			printf("学号重复！请检查后重新输入学号：");
			goto loop_5;
		}
	}
	printf("录入结束！\n");
	return;
}

void createStuList_order_id(LIST* stuList) {
	//定义变量接收输入信息
	uint64 id;
	char name[10];
	byte sex;
	unsigned age;
	float score;
	system("cls");
	printf("请输入学生学号（输入0以终止录入）:");
	scanf("%llu", &id);
	while (id)
	{
		//输入数据
		printf("请输入学生姓名：");
		scanf("%s", name); getchar();
		printf("请输入学生性别（男1女0）：");
		scanf("%d", &sex);
		printf("请输入学生年龄：");
		scanf("%u", &age);
		printf("请输入学生成绩：");
		scanf("%f", &score);
		STU* stu = createStu(id, name, sex, age, score);
		NODE* newNode = createNode(stu);
		//按学号顺序插入
		addList_order_id(newNode, stuList);
		printf("输入成功！\n\n请输入下一位学生学号（输入0以终止录入）:");
	loop_4:
		scanf("%llu", &id);
		if (checkID(stuList, id)) {
			printf("学号重复！请检查后重新输入学号：");
			goto loop_4;
		}
	}
	printf("录入结束！\n");
	return;
}

void createStuList_file(LIST* stuList) {
	FILE* fp;
	fp = fopen("stuList.dat", "rb");
	while (!feof(fp)) {
		STU* stu = (STU*)malloc(sizeof(STU));
		fread(stu, sizeof(STU), 1, fp);
		addList_t(stuList, stu);
	}

	if (ferror(fp)) {
		puts("读取错误！");
		destrList(stuList);
	}
	else if (feof(fp)) {
		puts("文件读取结束！链表创建成功！");
		deleteNode_pos(stuList, stuList->size);
	}
	fclose(fp);
	return;
}

void printStuList(LIST* stuList) {
	printHeader();
	foreach(stuList, printStu);
	return;
}

void addStuList_position(LIST* stuList) {
	system("cls");
	//定义变量接收输入信息
	unsigned pos;
	uint64 id;
	char name[10];
	byte sex;
	unsigned age;
	float score;
	//输入数据
loop_1:
	printf("请输入学生在链表中的位序（输入0以终止录入）：");
	scanf("%u", &pos);
	if (pos == 0)
		return;
	if (pos > stuList->size + 1) {//合法性检查
		printf("当前学生个数为%d，您输入的位序过大，请重新输入！\n", stuList->size); system("pause");
		goto loop_1;
	}
	printf("请输入学生学号:");
loop_3:
	scanf("%llu", &id);
	if (checkID(stuList, id)) {
		printf("学号重复！请检查后重新输入学号：");
		goto loop_3;
	}
	printf("请输入学生姓名：");
	scanf("%s", name);  getchar();
	printf("请输入学生性别（男1女0）：");
	scanf("%d", &sex);
	printf("请输入学生年龄：");
	scanf("%u", &age);
	printf("请输入学生成绩：");
	scanf("%f", &score);
	//创建学生
	STU* stu = createStu(id, name, sex, age, score);
	//插入链表
	addList_pos(stuList, stu, pos);
	printf("插入成功！\n");
	return;
}

void addStuList_order_id(LIST* stuList) {
	system("cls");
	//定义变量接收输入信息
	unsigned pos;
	uint64 id;
	char name[10];
	byte sex;
	unsigned age;
	float score;
	//输入数据
	if (!IsInOrder(stuList)) {
		printf("链表未按照学号升序排列！请排序后再进行此操作！\n");
		return;
	}
	printf("请输入学生学号（输入0以终止录入）:");
loop_6:
	scanf("%llu", &id);
	if (!id)
		return;
	if (checkID(stuList, id)) {
		printf("学号重复！请检查后重新输入学号：");
		goto loop_6;
	}
	printf("请输入学生姓名：");
	scanf("%s", name);  getchar();
	printf("请输入学生性别（男1女0）：");
	scanf("%d", &sex);
	printf("请输入学生年龄：");
	scanf("%u", &age);
	printf("请输入学生成绩：");
	scanf("%f", &score);
	//创建学生
	STU* stu = createStu(id, name, sex, age, score);
	NODE* newNode = createNode(stu);
	//插入链表
	addList_order_id(newNode, stuList);
	printf("插入成功！\n");
	return;
}

void deleteStu_position(LIST* stuList) {
	system("cls");
	//定义变量接收输入信息
	unsigned pos;
	uint64 id;
	//输入数据
loop_2:
	printf("请输入学生在链表中的位序（输入0以终止录入）：\n");
	scanf("%u", &pos);
	if (pos == 0)
		return;
	//合法性检查
	if (pos > stuList->size) {
		printf("当前学生个数为%d，您输入的位序过大，请重新输入！\n", stuList->size);
		goto loop_2;
	}
	deleteNode_pos(stuList, pos);
	printf("删除成功！\n");
	return;
}

void deleteStu_by_id(LIST* stuList) {
	//定义变量接收输入信息
	uint64 id;
	printf("请输入学生学号:\n");
	scanf("%llu", &id);
	int flag = deleteStu_id(stuList, id);
	if (flag == -1)
		printf("未找到该学生！\n");
	else if (flag > -1)
		printf("删除成功！\n");
	return;
}

void searchStu(LIST* stuList) {
	uint64 id;
	printf("请输入学生学号:\n");
	scanf("%llu", &id);
	NODE* found = findStu(stuList, id);
	if (!found)
		printf("未找到该学生！\n");
	else {
		STU* stu = found->data;
		printHeader();
		printStu(stu);
	}
	return;
}

void statistics(LIST* stuList) {
	if (!stuList->head) {
		printf("统计失败！请先创建链表初始化学生信息！\n");
		return;
	}
	printf("学生人数：%d\n", stuList->size);
	printf("平均成绩：%.2f\n", scoreAve(stuList));
	printf("最高分：%.2f\n", scoreMax(stuList->head));
	printf("不及格人数：%d\n", failedNum(stuList->head));
	printf("************************\n");
	return;
}

void reWriteStuListFile(LIST* stuList) {
	FILE* fp;
	if (!stuList->head) {
		printf("没有可以写入的学生信息！\n");
		return;
	}
	fp = fopen("stuList.dat", "wb");
	if (!fp) {
		printf("发生未知错误导致打开文件失败！\n");
		return;
	}
	foreach_fp1(stuList, fp, writeStu);
	fclose(fp);
	printf("重写成功！保存学生信息的二进制文件\"stuList.dat\"位于程序所在同一目录下！\n");
	return;
}


void clearStuListFile(LIST* stuList) {
	FILE* fp;
	fp = fopen("stuList.dat", "wb");
	if (!fp) {
		printf("发生未知错误导致打开文件失败！\n");
		return;
	}
	fclose(fp);
	printf("清空学生文件成功！\n");
	return;
}
//typedef void* FUN_fp(variant data, FILE* fp);
////void foreach_fp1(LIST* list, FILE* fp, FUN_fp fun_fp)
//void foreach_fp1(LIST* list, FILE* fp)
//
//{
//	NODE* curNode = list->head;
//	while (curNode) {
//		fun_fp(curNode->data, fp);
//		curNode = curNode->next;
//	}
//	return;
//}

void addStuListFile(LIST* stuList) {
	FILE* fp;
	if (!stuList->head) {
		printf("没有可以追加的学生信息！\n");
		return;
	}
	fp = fopen("stuList.dat", "ab");
	if (!fp) {
		printf("发生未知错误导致打开文件失败！\n");
		return;
	}
	foreach_fp1(stuList, fp, writeStu);
	//foreach_fp1(stuList, fp);
	fclose(fp);
	printf("追加成功！保存学生信息的二进制文件\"stuList.dat\"位于程序所在同一目录下！\n");
	return;
}
