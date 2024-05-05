#pragma once
#include <stdlib.h>

//#include <stdio.h>
//#include <string.h>
//#include <Windows.h>
//抽象数据类型
typedef void* variant;

//定义一个节点
typedef struct node {
	variant data;
	struct node* next;
}NODE;

//创建新节点
NODE* createNode(variant data) {
	NODE* newNode = (NODE*)calloc(1, sizeof(NODE));
	if (!newNode)
		return NULL;
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

//链表结构体
typedef struct list {
	int size;		//节点个数
	NODE* head;		//头指针
	NODE* tail;		//尾指针
}LIST;
//判断链表是否为空
int empty(LIST* list) {
	return list->size == 0;
}

//初始化链表
void initList(LIST* list) {
	//申请头节点
	list->head = list->tail = NULL;
	list->size = 0;
}
//添加数据_头插法
void addList_h(LIST* list, variant data) {
	NODE* newNode = createNode(data);
	if (!list->head)
		list->tail = newNode;
	else
		newNode->next = list->head;
	list->head = newNode;
	list->size++;
}

//添加数据_尾插法
void addList_t(LIST* list, variant data) {
	NODE* newNode = createNode(data);
	if (!list->head)
		list->head = newNode;
	else
		list->tail->next=newNode;
	list->tail = newNode;
	list->size++;
}

//添加数据_按指定位置
int addList_pos(LIST* list, variant data, unsigned pos) {
	if (pos == 0)
		return 0;
	if (pos > list->size + 1)
		return -1;
	if (!list->head) {
		NODE* newNode = createNode(data);
		list->head = newNode;
		list->tail = newNode;
	}
	else  if (pos==1) {
		NODE* newNode = createNode(data);
		newNode->next = list->head;
		list->head = newNode;
	}
	else{
		NODE* p = list->head;
		for (int i = 1; i < pos-1; i++)
			p = p->next;
		NODE* newNode = createNode(data);
		newNode->next = p->next;
		p->next = newNode;
		if (pos == list->size)
			list->tail = newNode;
	}
	list->size++;
	return list->size;
}

//删除数据_按指定的位置
int deleteNode_pos(LIST* list, unsigned pos) {
	if (pos == 0)
		return 0;
	if (pos > list->size||!list->head)
		return -1;
	NODE* p = list->head;
	if (pos == 1){
		list->head = p->next;
		free(p);
	}
	else{
		NODE* q = p;
		for (int i = 1; i < pos; i++) {
			q = p;
			p = p->next;
		}
		q->next = p->next;
		if (p == list->tail)
			list->tail = q;
		free(p);
	}
	list->size--;
	return list->size;
}

//销毁链表
void destrList(LIST* list) {
	while (list->head) {
		NODE* temp = list->head;
		list->head = list->head->next;
		free(temp);
	}
	list->head=list->tail=NULL;
	list->size = 0;
	return;
}

//编写一个函数，用来进行回调登记
typedef void (*FUN)(variant data);
void foreach(LIST* list,  FUN fun) {
	NODE* curNode = list->head;
	while (curNode) {
		fun(curNode->data);
		curNode = curNode->next;
	}
	return ;
}
