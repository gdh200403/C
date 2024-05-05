//设计一个实现任意长有符号整数加法运算的程序，要求能够实现任意长有符号整数的加法运算。
//利用双向循环链表实现长整数的存储，每个节点仅存十进制数的4位
//头结点：数据域符号代表长整数的符号，其绝对值代表元素结点数目
//相加过程中不破坏原有两个操作数链表，两操作数的头指针存于指针数组中
//输入和输出形式：每四位一组，组间用逗号隔开

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct node{
    int data;
    struct node *next;
    struct node *prev;
}Node;

void create(Node *head, char *str);
void printList(Node *head);
void add(Node **addNum, Node *result);
int compareAbs(Node *num1, Node *num2);

void appendR(Node *head, int data);//尾插法
void appendL(Node *head, int data);//头插法
void clear(Node *head);
Node* createNode(int data);
Node* createHead();

Node* createHead(){
    Node *head = (Node*)malloc(sizeof(Node));
    head->data = 0;
    head->next = head;
    head->prev = head;
    return head;
}

Node* createNode(int data){
    Node *node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void appendR(Node *head, int data){
    Node *node = createNode(data);
    node->prev = head->prev;
    node->next = head;
    head->prev->next = node;
    head->prev = node;
}

void appendL(Node *head, int data){
    Node *node = createNode(data);
    node->next = head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

void clear(Node *head){
    Node *p = head->next;
    while(p!=head){
        p = p->next;
        free(p->prev);
    }
    head->next = head;
    head->prev = head;
    head->data = 0;
}

void create(Node *head, char *str){
    char *token = strtok(str, ",");//strtok函数将字符串分割成一个个token
    while(token!=NULL){
        appendR(head, atoi(token));//atoi函数将字符串转换成整数
        head->data++;
        token = strtok(NULL, ",");
    }//注意此时除头结点外第一个结节点带有符号
    if(str[0]=='-'){
        head->data = -head->data;
        head->next->data = -head->next->data;
    }//将第一个结点的符号转移到头结点的数据域
}

void printList(Node *head){
    Node *p = head->next;
    if(head->data<0){
        printf("-");
    }
    printf("%d", p->data);//最高位前面不补0，同时如果这个数是0，也可以实现输出
    while(p->next!=head){
        p = p->next;
        printf(",%04d", p->data);//%04d表示输出4位，不足4位前面补0
    }
    printf("\n");
}

int compareAbs(Node *num1, Node *num2){
    int data1 = num1->data;
    int data2 = num2->data;
    if(data1<0){
        data1 = -data1;
    }
    if(data2<0){
        data2 = -data2;
    }
    if(data1>data2){
        return 1;
    }
    else if(data1<data2){
        return -1;
    }
    else{
        Node *p1 = num1->next;
        Node *p2 = num2->next;
        while(p1!=num1){
            if(p1->data>p2->data){
                return 1;//第一个数的绝对值大于第二个数的绝对值，返回1
            }
            else if(p1->data<p2->data){
                return -1;//第一个数的绝对值小于第二个数的绝对值，返回-1
            }
            else{
                p1 = p1->next;
                p2 = p2->next;
            }
        }
        return 0;//两个数的绝对值相等，返回0
    }
}

void add(Node **addNum, Node *result){
    int carry = 0;//进位
    int sum = 0;//每一位的和

    if(addNum[0]->data * addNum[1]->data > 0){
        Node *p1 = addNum[0]->prev;
        Node *p2 = addNum[1]->prev;
        while(p1!=addNum[0]&&p2!=addNum[1]){
            sum = p1->data + p2->data + carry;
            carry = sum/10000;
            sum = sum%10000;
            appendL(result, sum);
            result->data++;
            p1 = p1->prev;
            p2 = p2->prev;
        }
        while(p1!=addNum[0]){
            sum = p1->data + carry;
            carry = sum/10000;
            sum = sum%10000;
            appendL(result, sum);
            result->data++;
            p1 = p1->prev;
        }
        while(p2!=addNum[1]){
            sum = p2->data + carry;
            carry = sum/10000;
            sum = sum%10000;
            appendL(result, sum);
            result->data++;
            p2 = p2->prev;
        }
        if(carry!=0){
            appendL(result, carry);
            result->data++;
        }//两个数的位数相同，最高位相加需要进位的情况。注意：在减法中并没有这个环节，因为减法不会向虚空中借位，只有加法才会向虚空进位
        result->data = addNum[0]->data >= 0 ? result->data : -result->data;//将符号赋予头结点
    }
    else{ //减法
        int cmp = compareAbs(addNum[0], addNum[1]);
        Node *bjs, *js;//bjs为绝对值大的数，js为绝对值小的数
        switch(cmp){
            case 0:
                return;//两个数的绝对值相等，直接返回0
            case 1:
                bjs = addNum[0];
                js = addNum[1];
                break;
            case -1:
                bjs = addNum[1];
                js = addNum[0];
                break;
        }
        int a1 = bjs->data > 0 ? 1 : -1;//a1为绝对值大的数的符号
        Node *p1 = bjs->prev;
        Node *p2 = js->prev;
        while(p1!=bjs&&p2!=js){
            sum = p1->data - p2->data + carry;
            carry = sum < 0 ? -1 : 0;
            sum = sum < 0 ? sum + 10000 : sum;//减法的借位
            appendL(result, sum);
            result->data++;
            p1 = p1->prev;
            p2 = p2->prev;
        }
        // if(p1==bjs&&p2==js){//两个数的位数相同
        //     if(carry!=0){
        //         result->next->data = 10000 - result->next->data;
        //         result->data = -result->data;//将符号转移到头结点
        //         carry = 0;//避免进入下一个if
        //     }
        // }
        while(p1!=bjs){
            sum = p1->data + carry;
            carry = sum < 0 ? -1 : 0;
            sum = sum < 0 ? sum + 10000 : sum;//减法的借位
            appendL(result, sum);
            result->data++;
            p1 = p1->prev;
        }
        // while(p2!=addNum[1]){
        //     sum = p2->data * a2 + carry;
        //     carry = sum < 0 ? -1 : 0;
        //     sum = sum < 0 ? sum + 10000 : sum;
        //     appendL(result, sum);
        //     result->data++;
        //     p2 = p2->prev;
        // }
        // if(carry!=0){//两个数的位数不同，且最高位相减需要借位
        //     appendL(result, carry);
        //     result->data++;
        //     result->data = -result->data;
        //     result->next->data = -result->next->data;//将符号转移到头结点
        // }
        result->data *= a1;//将绝对值较大的数的符号赋予头结点
        Node *p = result->next;
        while(p!=result&&p->data==0){
            p = p->next;
            free(p->prev);
            p->prev = result;
            result->next = p;
            if(result->data>0){
                result->data--;
            }
            else{
                result->data++;
            }
        }//结果高位有多个0时，清理掉多余的0。特别的，若result==0，此时只保留头结点中的0
    }
    
}

int main(){
    Node *addNum[2];
    addNum[0] = createHead();
    addNum[1] = createHead();
    Node *result = createHead();
    while(1){
        char str1[1000], str2[1000];
        printf("请输入第一个整数：\n");
        scanf("%s", str1);
        printf("请输入第二个整数：\n");
        scanf("%s", str2);
        create(addNum[0], str1);
        create(addNum[1], str2);
        add(addNum, result);
        printf("两个整数的和为：\n");
        printList(result);
        printf("\n");
        clear(addNum[0]);
        clear(addNum[1]);
        clear(result);
    }
    return 0;
}