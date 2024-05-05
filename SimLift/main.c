#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define Status int
#define OK 1
#define ERROR 0
#define TRUE 1
#define FALSE 0

#define MAX_TIME 10000

#define T 1
#define CloseTestTime (40*T)
#define CloseTime (76*T)
#define DoorTime (20*T)
#define PersonTime (25*T)
#define BoredTime (300*T)
#define StartTime (15*T)//电梯加速时间
#define StopTime (15*T)//电梯减速时间
#define OneFloorTime (51*T)//电梯运行一层所需时间

#define GiveUpTimeTest (100*T)
#define IntertimeMAX (1000*T)

#define BaseFloor 2
#define TopFloor 13
#define BottomFloor 0
#define FloorNum 14

#define MAX_PASSENGER 12

int Time = 0;
int IDCount = 1;

/*------------------人员结构体---------------------*/
typedef struct{
    int ID;
    int currentFloor;
    int targetFloor;
    int GiveUpTime;//他能容忍的等候时间
    Status Iswaiting;
    int Intertime;//下一人出现的时间间隔，据此系统预置下一人进入系统 的时刻
}Person;

/*-------------等候队列和乘客链表-------------*/
typedef struct Node {
    Person* data;
    struct Node* next;
} Node;

typedef struct Queue {
    Node* front;
    Node* rear;
} Queue;

typedef struct List {
    int length;
    Node* head;
}List;

/*------------------电梯和楼层结构体----------------------*/
typedef struct {
    Status CallUp;
    Status CallDown;
    Queue* UpQueue;
    Queue* DownQueue;
}Floor;

typedef enum {
    Idle,
    Up,
    Down
}ElevatorState;

typedef enum {
    Opening,
    Open,
    Closing,
    Closed
}DoorState;

typedef struct{
    ElevatorState State;
    DoorState Door;
    int Floor;
    Status CallCar[FloorNum];//CallCar[i]的值为0，除非有人在电梯内按了第i层的按钮；
    Status IsEating;//值为0，除非人们正在进入和离开电梯；
    Status IsBored;//值为0，如果电梯已经在某层停候300t以上；
    Status IsWaiting;//值为0，除非电梯门正开着又无人迸出电梯；
    List* Passengers;
}Elevator;

/*------------------------事件队列的基本操作-------------------------*/
typedef enum {
    M1,M2,M3,M4,M5,M6,E1,E2,E3,E4,E5,E6,E7,E8,E9,C1
}EventType;

typedef struct {
    int time;
    EventType type;
    Person* person;
}Event;

typedef struct EventNode{
    Event data;
    struct EventNode* next;
}EventNode;

typedef struct EventQueue{
    EventNode* front;
    EventNode* rear;
}EventQueue;

//创建事件
Event* newEvent(int time, EventType type, Person* person) {
    Event* e = (Event*)malloc(sizeof(Event));
    e->time = time;
    e->type = type;
    e->person = person;
    return e;
}

//创建事件队列
EventQueue* createEventQueue() {
    EventQueue* q = (EventQueue*)malloc(sizeof(EventQueue));
    q->front = q->rear = NULL;
    return q;
}

//按时间顺序插入事件
Status addEvent(EventQueue* q, Event data){
    EventNode* temp = (EventNode*)malloc(sizeof(EventNode));
    if(temp == NULL) {
        return ERROR;
    }
    temp->data = data;
    temp->next = NULL;
    if(q->front == NULL) {
        q->front = q->rear = temp;
    } else {
        EventNode* p = q->front;
        EventNode* pre = NULL;
        while(p != NULL && p->data.time <= data.time) {
            pre = p;
            p = p->next;
        }
        if(pre == NULL) {
            temp->next = q->front;
            q->front = temp;
        } else {
            temp->next = p;
            pre->next = temp;
        }
    }
}

//出队
Status dequeueEvent(EventQueue* q, Event* e) {
    if(q->front == NULL) {
        return ERROR;
    }
    EventNode* temp = q->front;
    *e = temp->data;
    q->front = q->front->next;
    if(q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return OK;
}

//判断队列是否为空
int isEmptyEvent(EventQueue* q) {
    return q->front == NULL;
}

/*------------------人员队列的基本操作---------------------*/
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

Status enqueue(Queue* q, Person* data){
    Node* temp = (Node*)malloc(sizeof(Node));
    if(temp == NULL) {
        return ERROR;
    }
    temp->data = data;
    temp->next = NULL;
    if(q->front == NULL) {
        q->front = q->rear = temp;
    } else {
        q->rear->next = temp;
        q->rear = temp;
    }
    return OK;
}

Person* dequeue(Queue* q) {
    if(q->front == NULL) {
        return NULL;
    }
    Node* temp;
    temp = q->front;
    q->front = q->front->next;
    if(q->front == NULL) {
        q->rear = NULL;
    }
    return temp->data;
}

int isEmpty(Queue* q) {
    return q->front == NULL;
}

//特定乘客离开队列
Status exitQueue(Queue* q, Person* e){
    if(q->front == NULL) {
        return ERROR;
    }
    Node* p = q->front;
    Node* pre = NULL;
    while(p != NULL && p->data->ID != e->ID) {
        pre = p;
        p = p->next;
    }
    if(p == NULL) {
        return ERROR;
    }
    if(pre == NULL) {
        q->front = q->front->next;
    } else {
        pre->next = p->next;
    }
    if(q->front == NULL) {
        q->rear = NULL;
    }
    free(p);
    return OK;
}


/*------------------乘客链表的基本操作---------------------*/
List* createList() {
    List* list = (List*)malloc(sizeof(List));
    list->head = (Node*)malloc(sizeof(Node));
    list->length = 0;
    list->head->next = NULL;
    return list;
}

void insert(List* list, Person* data) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = data;
    temp->next = list->head->next;
    list->head->next = temp;
    list->length++;
}

Status delete(List* list, Person* e) {
    if(list->head->next == NULL) {
        return ERROR;
    }
    Node* p = list->head->next;
    Node* pre = list->head;
    while(p != NULL && p->data->ID != e->ID) {
        pre = p;
        p = p->next;
    }
    if(p == NULL) {
        return ERROR;
    }
    pre->next = p->next;
    free(p);
    list->length--;
    return OK;
}

int isEmptyList(List* list) {
    return list->head->next == NULL;
}


/*------------------电梯的基本操作---------------------*/
void initElevator(Elevator* e){
    e->State = Idle;
    e->Door = Closed;
    e->Floor = BaseFloor;
    for(int i = BottomFloor; i <= TopFloor; i++){
        e->CallCar[i] = FALSE;
    }
    e->IsEating = FALSE;
    e->IsBored = FALSE;
    e->IsWaiting = FALSE;
    e->Passengers = createList();
}

void initFloor(Floor* f){
    f->CallUp = FALSE;
    f->CallDown = FALSE;
    f->UpQueue = createQueue();
    f->DownQueue = createQueue();
}

void initBuilding(Floor* f, Elevator* e){
    for(int i = BottomFloor; i <= TopFloor; i++){
        initFloor(&f[i]);
    }
    initElevator(e);
}

void printTime(int Time){
    printf("Time: %5d ", Time);
}

Person* newPerson(){
    Person* p = (Person*)malloc(sizeof(Person));
    p->ID = IDCount++;
    p->currentFloor = rand() % TopFloor;
    p->targetFloor = rand() % TopFloor;
    while(p->currentFloor == p->targetFloor){
        p->targetFloor = rand() % TopFloor;
    }
    p->GiveUpTime = rand() % GiveUpTimeTest;
    p->Intertime = rand() % IntertimeMAX;
    p->Iswaiting = FALSE;
    return p;
}

int getOutOfElevator(int time, Elevator* elevator, EventQueue* q){
    //电梯停靠在某层时，乘客下客
    //返回值，下客人数
    if(elevator->Door != Open){
        return ERROR;
    }
    Node* p = elevator->Passengers->head->next;
    int i = 0;
    while(p != NULL){
        if(p->data->targetFloor == elevator->Floor){
            addEvent(q, *newEvent(time + i * PersonTime, M6, p->data));
            i++;
        }
        p = p->next;
    }
    return i;
}

// Status hurryIntoElevator(EventQueue* q, Elevator* Elevator, Floor* f, Event* e){
//     if(!Elevator->IsEating){
//         return ERROR;
//     }
//     EventNode* p = q->front;
//     while(p != NULL && p->data.time <= e->time){
//         p = p->next;
//     }
//     while(p != NULL && p->data.type != M6 && p->data.type != M5 && p->data.type != E5){

//     }
// }

void handleEvent(Event* e, Floor f[FloorNum], Elevator* elevator, EventQueue* q){
    switch(e->type){
        //事件M1，乘客进入大楼
        case M1:
            //输出信息
            printTime(e->time);
            printf("【乘客 %d】\t\t想从第 %d 层到第 %d 层，他的忍耐限度是 %d \n", e->person->ID, e->person->currentFloor, e->person->targetFloor, e->person->GiveUpTime);
            //他将进入状态M2
            addEvent(q, *newEvent(e->time, M2, e->person));
            //预置下一人进入系统的时刻
            addEvent(q, *newEvent(e->time + e->person->Intertime, M1, newPerson()));
            break;

        //事件M2，乘客按电梯按钮
        case M2:
            //乘客在电梯所在楼层,电梯门正在开启
            if(e->person->currentFloor == elevator->Floor && elevator->Door == Opening){
                if(e->person->currentFloor > e->person->targetFloor){
                    enqueue(f[e->person->currentFloor].DownQueue, e->person);
                    e->person->Iswaiting = TRUE;
                    addEvent(q, *newEvent(e->time, M3, e->person));
                }
                else{
                    enqueue(f[e->person->currentFloor].UpQueue, e->person);
                    e->person->Iswaiting = TRUE;
                    addEvent(q, *newEvent(e->time, M3, e->person));
                }
            }
            //乘客在电梯所在楼层,电梯门已经完全开启
            else if(e->person->currentFloor == elevator->Floor && elevator->Door == Open){
                if(elevator->IsEating == TRUE){
                    //草率地假设，如果电梯正在上下客，乘客将立刻进入电梯
                    addEvent(q, *newEvent(e->time, M5, e->person));
                    break;
                }else if(elevator->IsWaiting == TRUE){
                    if(e->person->currentFloor > e->person->targetFloor){
                        enqueue(f[e->person->currentFloor].DownQueue, e->person);
                        e->person->Iswaiting = TRUE;
                        addEvent(q, *newEvent(e->time, M3, e->person));
                        if(elevator->State == Down){
                            addEvent(q, *newEvent(e->time, E4, NULL));
                        }else{
                            f[e->person->currentFloor].CallDown = TRUE;
                        }
                    }
                    else{
                        enqueue(f[e->person->currentFloor].UpQueue, e->person);
                        e->person->Iswaiting = TRUE;
                        addEvent(q, *newEvent(e->time, M3, e->person));
                        if(elevator->State == Up){
                            addEvent(q, *newEvent(e->time, E4, NULL));
                        }else{
                            f[e->person->currentFloor].CallUp = TRUE;
                        }
                    }
                }
            //乘客在电梯所在楼层,电梯门正在关闭
            }else if(e->person->currentFloor == elevator->Floor && elevator->Door == Closing){
                if(e->person->currentFloor > e->person->targetFloor){
                    enqueue(f[e->person->currentFloor].DownQueue, e->person);
                    e->person->Iswaiting = TRUE;
                    addEvent(q, *newEvent(e->time, M3, e->person));
                    if(elevator->State == Down){
                        addEvent(q, *newEvent(e->time, E3, NULL));
                    }else{
                        f[e->person->currentFloor].CallDown = TRUE;
                    }
                }
                else{
                    enqueue(f[e->person->currentFloor].UpQueue, e->person);
                    e->person->Iswaiting = TRUE;
                    addEvent(q, *newEvent(e->time, M3, e->person));
                    if(elevator->State == Up){
                        addEvent(q, *newEvent(e->time, E3, NULL));
                    }else{
                        f[e->person->currentFloor].CallUp = TRUE;
                    }
                }
            //乘客在电梯所在楼层,电梯空闲
            }else if(e->person->currentFloor == elevator->Floor && elevator->State == Idle){
                if(e->person->currentFloor > e->person->targetFloor){
                    enqueue(f[e->person->currentFloor].DownQueue, e->person);
                    e->person->Iswaiting = TRUE;
                    addEvent(q, *newEvent(e->time, M3, e->person));
                    f[e->person->currentFloor].CallDown = TRUE;
                    addEvent(q, *newEvent(e->time, E2, NULL));
                }
                else{
                    enqueue(f[e->person->currentFloor].UpQueue, e->person);
                    e->person->Iswaiting = TRUE;
                    addEvent(q, *newEvent(e->time, M3, e->person));
                    f[e->person->currentFloor].CallUp = TRUE;
                    addEvent(q, *newEvent(e->time, E2, NULL));
                }
            }else{//乘客不在电梯所在楼层，或与电梯在同楼层但电梯已准备好运行或正在运行
                //乘客要下楼
                if(e->person->currentFloor > e->person->targetFloor){
                    f[e->person->currentFloor].CallDown = TRUE;
                    enqueue(f[e->person->currentFloor].DownQueue, e->person);
                    e->person->Iswaiting = TRUE;
                }
                //乘客要上楼
                else{
                    f[e->person->currentFloor].CallUp = TRUE;
                    enqueue(f[e->person->currentFloor].UpQueue, e->person);
                    e->person->Iswaiting = TRUE;
                }
                addEvent(q, *newEvent(e->time, M3, e->person));
                //如果电梯空闲，呼叫电梯。这里加上&& elevator->Door == Closed是因为elevator->State == Idle时，电梯可能正开着门，这时候不应该呼叫电梯
                if(elevator->State == Idle && elevator->Door == Closed){
                    addEvent(q, *newEvent(e->time, E6, NULL));
                }
            }
            break;

        //事件M3，乘客排队等待电梯
        case M3:
            //输出信息
            printTime(e->time);
            printf("【乘客 %d】\t\t在第 %d 层等待电梯\n", e->person->ID, e->person->currentFloor);
            //乘客开始等待
            addEvent(q, *newEvent(e->time + e->person->GiveUpTime, M4, e->person));
            break;

        //事件M4，乘客GiveUp检查点
        case M4:
            if(!e->person){//乘客已经下梯离开
                break;
            }
            if(e->person->Iswaiting == FALSE){//乘客已经进入电梯
                break;
            }
            if(e->person->Iswaiting == TRUE){//乘客还在等待
                if(elevator->State == Open && e->person->currentFloor == elevator->Floor){
                    break;//若此时电梯停靠且门开着，则继续等待
                }
                else{//其他情况，乘客离开队列
                    if(e->person->currentFloor > e->person->targetFloor){
                        exitQueue(f[e->person->currentFloor].DownQueue, e->person);
                    }else{
                        exitQueue(f[e->person->currentFloor].UpQueue, e->person);
                    }
                    //输出离开信息
                    printTime(e->time);
                    printf("【乘客 %d】\t\t等的不耐烦了，离开了大楼\n", e->person->ID);
                    free(e->person);
                    break;
                }
            }
            break;
        
        //事件M5，乘客进入电梯
        case M5:
            //输出信息
            printTime(e->time);
            printf("【乘客 %d】#进入#\t电梯，按下了%d层的按钮\n", e->person->ID, e->person->targetFloor);
            //乘客进入电梯
            insert(elevator->Passengers, e->person);
            //乘客按下电梯内的按钮
            elevator->CallCar[e->person->targetFloor] = TRUE;
            e->person->Iswaiting = FALSE;
            break;

        //事件M6，乘客离开电梯
        case M6:
            //输出信息
            printTime(e->time);
            printf("【乘客 %d】#到达#\t第 %d 层，离开了电梯\n", e->person->ID, e->person->targetFloor);
            //乘客离开电梯
            delete(elevator->Passengers, e->person);
            //乘客离开系统
            free(e->person);
            break;

        //事件E1，电梯在1层停候
        case E1:
            //输出信息
            printTime(e->time);
            printf("\t\t\t\t\t\t\t\t\t\t电梯在第 %d 层停候\n", BaseFloor);
            elevator->State = Idle;
            elevator->Door = Closed;
            elevator->Floor = BaseFloor;
            elevator->IsEating = FALSE;
            elevator->IsBored = FALSE;
            elevator->IsWaiting = FALSE;
            break;
        
        //事件E2，电梯将要停靠在某层前的检查点，若IsBored == TRUE,不开门，转到E1；反之，在开门前判断关门后电梯的运行方向，并转到开门事件E3
        case E2:
            {ElevatorState next_state;
            if(elevator->IsBored == TRUE){
                next_state = Idle;
                elevator->State = next_state;
                addEvent(q, *newEvent(e->time, E1, NULL));
                break;
            }

            if(elevator->State == Up){//如果电梯运行模式是向上
                //检查下一个状态电梯是否要继续向上
                Status upcalled = FALSE;//开关变量
                for(int i = elevator->Floor + 1; i <= TopFloor; i++){
                    if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                        upcalled = TRUE;
                        break;
                    }
                }//如果当前层以上有人呼叫电梯，或电梯内有人要上楼，电梯将继续向上
                if(f[elevator->Floor].CallUp == TRUE){
                    upcalled = TRUE;
                }//如果电梯在当前层有人要上楼，电梯无论如何都要继续向上

                if(upcalled == TRUE){
                    next_state = Up;
                }
                else{
                    Status downcalled = FALSE;
                    for(int i = elevator->Floor - 1; i >= BottomFloor; i--){
                        if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                            downcalled = TRUE;
                            break;
                        }
                    }
                    if(f[elevator->Floor].CallDown == TRUE){
                        downcalled = TRUE;
                    }

                    if(downcalled == TRUE){
                        next_state = Down;
                    }
                    else{
                        next_state = Idle;
                    }
                }
            }
            else if(elevator->State == Down){//如果电梯运行模式是向下
                //检查下一个状态电梯是否要继续向下
                Status downcalled = FALSE;//开关变量
                for(int i = elevator->Floor - 1; i >= BottomFloor; i--){
                    if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                        downcalled = TRUE;
                        break;
                    }
                }//如果当前层以下有人呼叫电梯，或电梯内有人要下楼，电梯将继续向下
                if(f[elevator->Floor].CallDown == TRUE){
                    downcalled = TRUE;
                }//如果电梯在当前层有人要下楼，电梯无论如何都要继续向下

                if(downcalled == TRUE){
                    next_state = Down;
                }
                else{
                    Status upcalled = FALSE;
                    for(int i = elevator->Floor + 1; i <= TopFloor; i++){
                        if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                            upcalled = TRUE;
                            break;
                        }
                    }
                    if(f[elevator->Floor].CallUp == TRUE){
                        upcalled = TRUE;
                    }

                    if(upcalled == TRUE){
                        next_state = Up;
                    }
                    else{
                        next_state = Idle;
                    }
                }
            }
            else if(elevator->State == Idle){
                if(f[elevator->Floor].CallUp == TRUE){
                    next_state = Up;
                }else if(f[elevator->Floor].CallDown == TRUE){
                    next_state = Down;
                }
            }

            elevator->State = next_state;
            //输出信息
            printTime(e->time);
            printf("\t\t\t\t\t\t\t\t\t\t电梯在第 %d 层停靠 ", elevator->Floor);
            switch (next_state){
                case Up:
                    printf("并置状态为向上\n");
                    break;
                case Down:
                    printf("并置状态为向下\n");
                    break;
                case Idle:
                    printf("并置状态为空闲\n");
                    break;
            }
            addEvent(q, *newEvent(e->time, E3, NULL));//电梯将要开门
            break;}

        //事件E3，电梯开门瞬间
        case E3:
            //输出信息
            printTime(e->time);
            printf("\t\t\t\t\t\t\t\t\t\t电梯开门\n");
            //电梯开门
            elevator->Door = Opening;
            //置电梯内本层按钮为0
            elevator->CallCar[elevator->Floor] = FALSE;
            //置本层对应上下行按钮为0
            if(elevator->State == Up){
                f[elevator->Floor].CallUp = FALSE;
            }
            else if(elevator->State == Down){
                f[elevator->Floor].CallDown = FALSE;
            }
            //置下一个事件，电梯将在DoorTime后完全开启门，进入状态E4，上下客人
            addEvent(q, *newEvent(e->time + DoorTime, E4, NULL));
            //置下一个事件，电梯将在CloseTime后试图关门，进入状态E5
            addEvent(q, *newEvent(e->time + CloseTime, E5, NULL));
            break;

        //事件E4，电梯门完全开，上下客
        case E4:
            {elevator->Door = Open;
            elevator->IsEating = TRUE;
            //下客
            int out_number = getOutOfElevator(e->time, elevator, q);
            int out_time = out_number * PersonTime;
            int in_time = 0;
            //上客
            if(elevator->State == Up){
                int in_count = 0;
                while(!isEmpty(f[elevator->Floor].UpQueue)){
                    if(in_count >= MAX_PASSENGER - elevator->Passengers->length + out_number){
                        break;
                    }//电梯已满，无法再上客
                    Person* p = dequeue(f[elevator->Floor].UpQueue);
                    addEvent(q, *newEvent(e->time + out_time + in_count * PersonTime, M5, p));
                    in_count++;
                }
                f[elevator->Floor].CallUp = FALSE;
                in_time = in_count * PersonTime;
            }
            else if(elevator->State == Down){
                int in_count = 0;
                while(!isEmpty(f[elevator->Floor].DownQueue)){
                    if(in_count >= MAX_PASSENGER - elevator->Passengers->length + out_number){
                        break;
                    }//电梯已满，无法再上客
                    Person* p = dequeue(f[elevator->Floor].DownQueue);
                    addEvent(q, *newEvent(e->time + out_time + in_count * PersonTime, M5, p));
                    in_count++;
                }
                f[elevator->Floor].CallDown = FALSE;
                in_time = in_count * PersonTime;
            }
            //置下一个事件，电梯将在in_time + out_time 后，进入C1，置IsEating为0
            addEvent(q, *newEvent(e->time + in_time + out_time, C1, NULL));
            break;}

        //事件E5，电梯检测并决定是否关门
        case E5:
            if(elevator->IsEating == TRUE){
                //如果电梯正在上下客，电梯将在CloseTestTime后再次检测
                addEvent(q, *newEvent(e->time + CloseTestTime, E5, NULL));
                //输出信息
                printTime(e->time);
                printf("\t\t\t\t\t\t\t\t\t\t电梯正在上下客，电梯将在 %d 后再次检测是否关门\n", CloseTestTime);
            }
            //电梯不在上下客，电梯开始关门
            else{
                addEvent(q, *newEvent(e->time + DoorTime, E6, NULL));
                elevator->Door = Closing;
                elevator->IsWaiting = FALSE;
            }
            break;

        //事件E6，电梯关门，并根据启动前判断的运行状态，并加以检查，置下一个事件
        case E6:
            //若进入此状态时电梯处于开门或正在开门状态，一定是是有人在关门时抢进电梯，电梯又开了门，电梯将在DoorTime后再次检测是否关门
            if(elevator->Door == Opening || elevator->Door == Open){
                //addEvent(q, *newEvent(e->time + DoorTime, E5, NULL));//这句要删掉，引发了严重的错误
                //反思：思路不够清晰，能唤起关门判断事件E5的应该只有E3，别的任何事件试图触发额外的E5都会导致门“多次关闭”，导致严重的错误
                //E5相当于一个周期性的检查事件，只有E3能唤起E5，其他事件只能改变相应状态参量，由E5根据状态决定是否关门
                break;
            }
            //如果此时elevator->Door != Closing，则这个E6是由M2中乘客呼叫空闲电梯的情况触发的，无需进行关门操作，直接检查运行状态
            else if(elevator->Door == Closing){
                //电梯关门
                elevator->Door = Closed;
                //输出信息
                printTime(e->time);
                printf("\t\t\t\t\t\t\t\t\t\t电梯关门\n");
            }
            //电梯关门后，检查电梯的运行状态
            if(elevator->State == Idle){
                Status upcalled = FALSE;//开关变量
                for(int i = elevator->Floor + 1; i <= TopFloor; i++){
                    if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                        upcalled = TRUE;
                        break;
                    }
                }
                Status downcalled = FALSE;//开关变量
                for(int i = elevator->Floor - 1; i >= BottomFloor; i--){
                    if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                        downcalled = TRUE;
                        break;
                    }
                }
                if(upcalled == TRUE){
                    //电梯将在StartTime后进入E7，上升
                    elevator->State = Up;
                    addEvent(q, *newEvent(e->time + StartTime, E7, NULL));
                    //输出信息
                    printTime(e->time);
                    printf("\t\t\t\t\t\t\t\t\t\t电梯在停侯状态被呼叫，开始上升\n");
                    break;
                }else if(downcalled == TRUE){
                    //电梯将在StartTime后进入E8，下降
                    elevator->State = Down;
                    addEvent(q, *newEvent(e->time + StartTime, E8, NULL));
                    //输出信息
                    printTime(e->time);
                    printf("\t\t\t\t\t\t\t\t\t\t电梯在停侯状态被呼叫，开始下降\n");
                    break;
                }else{
                    //如果电梯在本垒层，将立刻进入E1状态
                    if(elevator->Floor == BaseFloor){
                        addEvent(q,*newEvent(e->time,E1,NULL));
                    }
                    //电梯不在本垒层，电梯将在BoredTime后进入E9
                    else{
                        addEvent(q, *newEvent(e->time + BoredTime, E9, NULL));
                    }
                }
                
            }
            else if(elevator->State == Up){
                //电梯已达到最大容量但仍有要上楼客人未能上来，他们将再次按下本层的上楼按钮
                if(!isEmpty(f[elevator->Floor].UpQueue)){
                    f[elevator->Floor].CallUp = TRUE;
                }
                //检查下一个状态电梯是否真的要继续向上，避免有坏人按了按钮不进电梯（乘客等不及了放弃乘坐）
                Status upcalled = FALSE;//开关变量
                for(int i = elevator->Floor + 1; i <= TopFloor; i++){
                    if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                        upcalled = TRUE;
                        break;
                    }
                }
                if(!upcalled){
                    Status downcalled = FALSE;//开关变量
                    for(int i = elevator->Floor - 1; i >= BottomFloor; i--){
                        if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                            downcalled = TRUE;
                            break;
                        }
                    }
                    if(f[elevator->Floor].CallDown == TRUE){
                        downcalled = TRUE;
                    }
                    if(downcalled == TRUE){
                        elevator->State = Down;
                        addEvent(q, *newEvent(e->time + StartTime, E8, NULL));
                        //输出信息
                        printTime(e->time);
                        printf("\t\t\t\t\t\t\t\t\t\t电梯发现不需要上升了，状态改为下降\n");
                    }else{
                        elevator->State = Idle;
                        printTime(e->time);
                        printf("\t\t\t\t\t\t\t\t\t\t电梯发现不需要上升了，状态改为停侯\n");
                    }
                }
                else{
                    //电梯将在StartTime后进入E7，上升
                    addEvent(q, *newEvent(e->time + StartTime, E7, NULL));
                    //输出信息
                    printTime(e->time);
                    printf("\t\t\t\t\t\t\t\t\t\t电梯开始上升\n");
                }
            }
            else if(elevator->State == Down){
                //电梯已达到最大容量但仍有要下楼客人未能上来，他们将再次按下本层的下楼按钮
                if(!isEmpty(f[elevator->Floor].DownQueue)){
                    f[elevator->Floor].CallDown = TRUE;
                }
                //检查下一个状态电梯是否真的要继续向下，避免有坏人按了按钮不进电梯
                Status downcalled = FALSE;//开关变量
                for(int i = elevator->Floor - 1; i >= BottomFloor; i--){
                    if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                        downcalled = TRUE;
                        break;
                    }
                }
                if(!downcalled){
                    Status upcalled = FALSE;//开关变量
                    for(int i = elevator->Floor + 1; i <= TopFloor; i++){
                        if(elevator->CallCar[i] == TRUE || f[i].CallUp == TRUE || f[i].CallDown == TRUE){
                            upcalled = TRUE;
                            break;
                        }
                    }
                    if(f[elevator->Floor].CallUp == TRUE){
                        upcalled = TRUE;
                    }
                    if(upcalled == TRUE){
                        elevator->State = Up;
                        addEvent(q, *newEvent(e->time + StartTime, E7, NULL));
                        //输出信息
                        printTime(e->time);
                        printf("\t\t\t\t\t\t\t\t\t\t电梯发现不需要下降了，状态改为上升\n");
                    }else{
                        elevator->State = Idle;
                        printTime(e->time);
                        printf("\t\t\t\t\t\t\t\t\t\t电梯发现不需要下降了，状态改为停侯\n");
                    }
                }
                else{
                    //电梯将在StartTime后进入E8，下降
                    addEvent(q, *newEvent(e->time + StartTime, E8, NULL));
                    //输出信息
                    printTime(e->time);
                    printf("\t\t\t\t\t\t\t\t\t\t电梯开始下降\n");
                }
            }
            break;

        //事件E7，电梯向上一层
        case E7:
            {
            printf("\t\t\t\t\t\t\t\t\t\t\t【↑↑↑↑↑↑↑↑】\n");
            elevator->Floor++;
            Status IsIdleCalled = TRUE;
            for(int i = BottomFloor; i <= TopFloor; i++){
                if(elevator->CallCar[i] == TRUE){
                    IsIdleCalled = FALSE;
                    break;
                }
            }
            if(elevator->IsBored == TRUE){
                IsIdleCalled = FALSE;
            }
            if(IsIdleCalled == TRUE){
                if(f[elevator->Floor].CallDown == TRUE || f[elevator->Floor].CallUp == TRUE){
                    addEvent(q, *newEvent(e->time + OneFloorTime + StopTime, E2, NULL));
                }
                else{
                    addEvent(q, *newEvent(e->time + OneFloorTime, E7, NULL));
                }
                break;
            }
            
            if(elevator->CallCar[elevator->Floor] == TRUE || f[elevator->Floor].CallUp == TRUE){//如果电梯内有人要在本层下，或者本层有人要上楼
                addEvent(q, *newEvent(e->time + OneFloorTime + StopTime, E2, NULL));
                elevator->IsBored = FALSE;//如果这时电梯在自动上升，取消IsBored以取消自动上升状态
            }
            else if(elevator->IsBored == TRUE && elevator->Floor == BaseFloor){
                addEvent(q, *newEvent(e->time + OneFloorTime + StopTime, E2, NULL));
            }
            else{
                addEvent(q, *newEvent(e->time + OneFloorTime, E7, NULL));
            }
            break;}

        //事件E8，电梯向下一层
        case E8:
            {
            printf("\t\t\t\t\t\t\t\t\t\t\t【↓↓↓↓↓↓↓↓】\n");
            elevator->Floor--;
            Status IsIdleCalled = TRUE;
            for(int i = BottomFloor; i <= TopFloor; i++){
                if(elevator->CallCar[i] == TRUE){
                    IsIdleCalled = FALSE;
                    break;
                }
            }
            if(elevator->IsBored == TRUE){
                IsIdleCalled = FALSE;
            }
            if(IsIdleCalled == TRUE){
                if(f[elevator->Floor].CallUp == TRUE || f[elevator->Floor].CallDown == TRUE){
                    addEvent(q, *newEvent(e->time + OneFloorTime + StopTime, E2, NULL));
                }
                else{
                    addEvent(q, *newEvent(e->time + OneFloorTime, E8, NULL));
                }
                break;
            }
            
            if(elevator->CallCar[elevator->Floor] == TRUE || f[elevator->Floor].CallDown == TRUE){//如果电梯内有人要在本层下，或者本层有人要下楼
                addEvent(q, *newEvent(e->time + OneFloorTime + StopTime, E2, NULL));
                elevator->IsBored = FALSE;//如果这时电梯在自动下降，取消IsBored以取消自动下降状态
            }
            else if(elevator->IsBored == TRUE && elevator->Floor == BaseFloor){
                addEvent(q, *newEvent(e->time + OneFloorTime + StopTime, E2, NULL));
            }
            else{
                addEvent(q, *newEvent(e->time + OneFloorTime, E8, NULL));
            }
            break;}

        //事件E9，电梯在非本垒层停侯太久的检查点
        case E9:
            if(elevator->State == Idle){
                elevator->IsBored = TRUE;//若电梯仍处于停侯状态，置IsBored为1
                if(elevator->Floor > BaseFloor){
                    elevator->State = Down;
                    addEvent(q, *newEvent(e->time + StartTime, E8, NULL));//若电梯在本垒层以上，电梯将在StartTime后开始自动下楼
                    //输出信息
                    printTime(e->time);
                    printf("\t\t\t\t\t\t\t\t\t\t电梯在第 %d 层停侯太久，开始自动下降\n", elevator->Floor);
                }else{
                    elevator->State = Up;
                    addEvent(q, *newEvent(e->time + StartTime, E7, NULL));//若电梯在本垒层以下，电梯将在StartTime后开始自动爬楼
                    //输出信息
                    printTime(e->time);
                    printf("\t\t\t\t\t\t\t\t\t\t电梯在第 %d 层停侯太久，开始自动上升\n", elevator->Floor);
                }
            }else{
                break;
            }
            break;

        //控制事件C1，电梯上下客完毕，置IsEating为0，置IsWaiting为1
        case C1:
            elevator->IsEating = FALSE;
            elevator->IsWaiting = TRUE;
            break;
    }
}

int main(){
    srand((unsigned)time(NULL));
    Floor f[FloorNum];
    Elevator* elevator;
    elevator = (Elevator*)malloc(sizeof(Elevator));
    EventQueue* q = createEventQueue();
    initBuilding(f, elevator);
    addEvent(q, *newEvent(0, M1, newPerson()));
    while(!isEmptyEvent(q) && Time < MAX_TIME){
        Event* e;
        e = (Event*)malloc(sizeof(Event));
        dequeueEvent(q, e);
        Time = e->time;
        handleEvent(e, f, elevator, q);
    }
    return 0;
}