#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "List.h"
typedef unsigned long long uint64;
typedef unsigned char byte;
typedef struct student {
	uint64 id;
	char name[10];
	byte sex;
	unsigned int age;
	float score;
}STU;

//创建一条学生记录
STU* createStu(uint64 id, char* name, byte sex, unsigned int age, float score) {
	STU* stu = (STU*)malloc(sizeof(STU));
	if (!stu)
		return NULL;
	stu->id = id;
	strcpy(stu->name, name);
	stu->sex = sex;
	stu->age = age;
	stu->score = score;
	return stu;
}

//打印一条学生记录
void printStu(STU* stu) {
	printf("%llu\t", stu->id);
	printf("%s\t", stu->name);
	printf("%s\t", stu->sex ? "男" : "女");
	printf("%u\t", stu->age);
	printf("%.2f\n", stu->score);
	return;
}

//向二进制文件中写入一条学生记录
void writeStu(STU* stu, FILE* fp) {
	fwrite(stu, sizeof(STU), 1, fp);
	return;
}

//打印表头
void printHeader(void) {
	printf("学号\t\t姓名\t性别\t年龄\t成绩\n");
	return;
}

//显示主菜单
void showMenu(void) {
	printf("****学生信息管理系统****\n");
	printf("0退出程序\n");
	printf("1创建学生记录链表\n");
	printf("2打印全部学生记录\n");
	printf("3插入一条新的学生记录\n");
	printf("4按条件删除一条学生记录\n");
	printf("5按学号查找学生记录\n");
	printf("6统计\n");
	printf("7销毁学生链表\n");
	printf("8将学生信息写入磁盘文件\n");
	printf("9按学号升序排列\n");
	printf("************************\n");
	printf("请输入相应数字以执行功能：\n");
	return;
}

//按学号顺序插入
void addList_order_id(NODE* newNode, LIST* stuList) {
	STU* newData = newNode->data;
	if (stuList->head == NULL) {
		newNode->next = stuList->head;
		stuList->head = newNode;
	}
	else {
		STU* headData = stuList->head->data;
		//头文件中使用了void*类型的指针data，要访问data被赋以STU*类型的值后内部的id的值，必须新建STU*类型的指针
		if (headData->id > newData->id) {
			newNode->next = stuList->head;
			stuList->head = newNode;
		}
		else {
			NODE* p = stuList->head;
			while (p->next != NULL) {
				STU* p_next_data = p->next->data;
				if (p_next_data->id < newData->id) {
					p = p->next;
				}
				else
					break;
			}
			newNode->next = p->next;
			p->next = newNode;
		}
	}
	stuList->size++;
	return;
}

//判断链表是否按学号顺序排列
int IsInOrder(LIST* stuList) {
	if (stuList->size == 1 || stuList->size == 0)
		return 1;
	NODE* cur = stuList->head;
	while (cur && cur->next) {
		STU* curData = cur->data;
		STU* nextData = cur->next->data;
		if (curData->id > nextData->id)
			return 0;
		cur = cur->next;
	}
	return 1;
}

//删除指定学号的记录
int deleteStu_id(LIST* stuList, uint64 id) {
	STU* headData = stuList->head->data;
	if (stuList->head && headData->id == id) {
		NODE* p = stuList->head;
		stuList->head = p->next;
		free(p);
		stuList->size--;
		return stuList->size;
	}
	NODE* p = stuList->head;
	while (p->next != NULL) {
		STU* p_next_data = p->next->data;
		if (p_next_data->id != id) {
			p = p->next;
		}
		else
			break;
	}
	if (p->next) {
		NODE* q = p->next;
		p->next = q->next;
		free(q);
		stuList->size--;
		return stuList->size;
	}
	return -1;
}

//查找指定学号的记录
NODE* findStu(LIST* stuList, uint64 id) {
	NODE* curNode = stuList->head;
	while (curNode) {
		STU* curData = curNode->data;
		if (curData->id == id)
			break;
		curNode = curNode->next;
	}
	return curNode;
}

//学号查重，返回1重复，返回0无重复
int checkID(LIST* stuList, uint64 id) {
	if (!stuList->head)
		return 0;
	NODE* curNode = stuList->head;
	STU* curData = curNode->data;
	int flag = 0;
	while (curNode) {
		curData = curNode->data;
		if (curData->id == id)
			return 1;
		curNode = curNode->next;
	}
	return 0;
}

//统计平均分
float scoreAve(LIST* stuList) {
	float sum = 0;
	NODE* curNode = stuList->head;
	while (curNode) {
		STU* curData = curNode->data;
		sum += curData->score;
		curNode = curNode->next;
	}
	return sum / stuList->size;
}

//统计最高分
float scoreMax(NODE* head) {
	float max = 0;
	NODE* curNode = head;
	while (curNode) {
		STU* curData = curNode->data;
		if (max < curData->score) {
			max = curData->score;
		}
		curNode = curNode->next;
	}
	return max;
}

//统计不及格人数
unsigned failedNum(NODE* head) {
	unsigned sum = 0;
	NODE* cur = head;
	while (cur) {
		STU* data = cur->data;
		if (data->score < 60)
			sum++;
		cur = cur->next;
	}
	return sum;
}

//排序链表
LIST* rearrange(LIST* stuList) {
	if (empty(stuList)) {
		return NULL;
	}
	LIST* newList = (LIST*)malloc(sizeof(LIST));
	initList(newList);
	int n = stuList->size;
	for (int i = 0; i < n; i++) {
		NODE* cur = stuList->head;
		int i = 0;
		int flag = 0;
		STU* minData = cur->data;
		while (cur) {
			STU* curData = cur->data;
			i++;
			if (curData->id <= minData->id) {
				flag = i;
				minData = curData;
			}
			cur = cur->next;
		}
		STU* stu = (STU*)malloc(sizeof(STU));
		stu->age = minData->age;
		stu->id = minData->id;
		strcpy(stu->name, minData->name);
		stu->score = minData->score;
		stu->sex = minData->sex;
		addList_t(newList, stu);
		deleteNode_pos(stuList, flag);
	}
	destrList(stuList);
	return newList;
}

 void createStuList_head(LIST* stuList);
 void createStuList_tail(LIST* stuList);
 void createStuList_order_id(LIST* stuList);
 void createStuList_file(LIST* stuList);
 void printStuList(LIST* stuList);
 void addStuList_position(LIST* stuList);
 void addStuList_order_id(LIST* stuList);
 void deleteStu_position(LIST* stuList);
 void deleteStu_by_id(LIST* stuList);
 void  searchStu(LIST* stuList);
 void statistics(LIST* stuList);
 void reWriteStuListFile(LIST* stuList);
 void addStuListFile(LIST* stuList);
 void clearStuListFile(LIST* stuList);

typedef void* FUN_fp(variant data, FILE* fp);
 void foreach_fp1(LIST* list, FILE* fp, FUN_fp* fun_fp)
 /*void foreach_fp1(LIST* list, FILE* fp)*/
 {
	 NODE* curNode = list->head;
	 while (curNode) {
		 //FUN_fp(curNode->data, fp);
		 fun_fp(curNode->data,fp);
		 curNode = curNode->next;
	 }
	 return;
 }
