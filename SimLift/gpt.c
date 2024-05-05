#include <stdio.h>

// 定义电梯的状态
enum ElevatorState {
    GoingUp,
    GoingDown,
    Idle
};

// 定义乘客的结构体
struct Passenger {
    int currentFloor;
    int targetFloor;
    int waitTime;
};

// 定义电梯的结构体
struct Elevator {
    enum ElevatorState state;
    int currentFloor;
    struct Passenger passengers[10]; // 假设电梯最多可以容纳10个乘客
    int passengerCount;
};

// 创建电梯
struct Elevator elevator = {Idle, 1, {}, 0};

// 创建乘客队列
struct Passenger passengerQueue[50]; // 假设最多有50个乘客等待电梯
int passengerQueueCount = 0;

// 定义电梯静止的时间
int idleTime = 0;

// 处理乘客的行为
void handlePassengerBehavior() {
    // TODO: 实现乘客的行为，如进入电梯、离开电梯等
    // 处理乘客的行为
    // 乘客进入电梯
    for (int i = 0; i < passengerQueueCount; i++) {
        if (passengerQueue[i].currentFloor == elevator.currentFloor && elevator.passengerCount < 10) {
            elevator.passengers[elevator.passengerCount++] = passengerQueue[i];
            // 移除已经进入电梯的乘客
            for (int j = i; j < passengerQueueCount - 1; j++) {
                passengerQueue[j] = passengerQueue[j + 1];
            }
            passengerQueueCount--;
            i--;
        }
    }

    // 乘客离开电梯
    for (int i = 0; i < elevator.passengerCount; i++) {
        if (elevator.passengers[i].targetFloor == elevator.currentFloor) {
            // 移除已经离开电梯的乘客
            for (int j = i; j < elevator.passengerCount - 1; j++) {
                elevator.passengers[j] = elevator.passengers[j + 1];
            }
            elevator.passengerCount--;
            i--;
        }
    }

    // 处理乘客放弃等待的情况
    for (int i = 0; i < passengerQueueCount; i++) {
        passengerQueue[i].waitTime++;
        if (passengerQueue[i].waitTime > 300) {
            // 移除已经放弃等待的乘客
            for (int j = i; j < passengerQueueCount - 1; j++) {
                passengerQueue[j] = passengerQueue[j + 1];
            }
            passengerQueueCount--;
            i--;
        }
    }
}

// 处理电梯的行为
void handleElevatorBehavior() {
    // TODO: 实现电梯的行为，如开门、关门、上行、下行等
    // 处理电梯的行为
    // 电梯开门
    int doorOpen = 0;
    for (int i = 0; i < elevator.passengerCount; i++) {
        if (elevator.passengers[i].targetFloor == elevator.currentFloor) {
            doorOpen = 1;
            break;
        }
    }
    if (!doorOpen) {
        for (int i = 0; i < passengerQueueCount; i++) {
            if (passengerQueue[i].currentFloor == elevator.currentFloor) {
                doorOpen = 1;
                break;
            }
        }
    }

    // 电梯关门
    if (doorOpen) {
        handlePassengerBehavior();
        doorOpen = 0;
    }
    
    // 电梯上行和下行
    if (elevator.passengerCount > 0) {
        // 如果电梯内有乘客，根据乘客的目标楼层决定移动的方向
        if (elevator.passengers[0].targetFloor > elevator.currentFloor) {
            elevator.state = GoingUp;
            elevator.currentFloor++;
        } else if (elevator.passengers[0].targetFloor < elevator.currentFloor) {
            elevator.state = GoingDown;
            elevator.currentFloor--;
        }
    } else if (passengerQueueCount > 0) {
        // 如果电梯内没有乘客，根据等待乘客的当前楼层决定移动的方向
        if (passengerQueue[0].currentFloor > elevator.currentFloor) {
            elevator.state = GoingUp;
            elevator.currentFloor++;
        } else if (passengerQueue[0].currentFloor < elevator.currentFloor) {
            elevator.state = GoingDown;
            elevator.currentFloor--;
        }
    } else {
        // 如果电梯内没有乘客，且没有等待乘客，电梯保持空闲状态
        elevator.state = Idle;
    }

    // 处理电梯静止时间超过300t的情况
    if (elevator.state == Idle) {
        idleTime++;
        if (idleTime > 300) {
            printf("Warning: The elevator has been idle for more than 300t.\n");
        }
    } else {
        idleTime = 0;
    }
}

// 模拟电梯的运行
void simulateElevator() {
    while (1) {
        handlePassengerBehavior();
        handleElevatorBehavior();
    }
}

int main() {
    simulateElevator();
    return 0;
}