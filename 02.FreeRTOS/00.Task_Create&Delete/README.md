# 任务创建和删除

## 实验设计
创建四个任务，StartDefaultTask、task1、task2、task3
- StartDefaultTask 
  - 用来创建其他三个任务
- task1 
  - 实现 RedLED 500ms 闪烁一次
- task2 
  - 实现 GreenLED 500ms 闪烁一次
- task3 
  - 判断按键 KEY0 是否按下，按下则删掉 task1；
  - 判断按键 KEY1 是否按下，按下则输出消息 "Hello, World!"

## 坑点

StartDefaultTask() 函数会执行完再由调度器进行调度！！！

```c
void StartDefaultTask(void const* argument) {
    xTaskCreate((TaskFunction_t) Task1, "task1", 128, NULL, 1, &task1Handle);
    xTaskCreate((TaskFunction_t) Task2, "task2", 128, NULL, 2, &task2Handle);
    xTaskCreate((TaskFunction_t) Task3, "task3", 128, NULL, 3, &task3Handle);
    printf("FreeRTOS demo\r\n");
    vTaskDelete(NULL); // Delete the start task
}
```

此时先输出 FreeRTOS demo，再进行任务 task3，task2，task1 的执行。

因此，可以使用默认函数 `StartDefaultTask()` 创建开始任务 `startTask()`，然后将其删除。

之后，在 `startTask()` 中创建任务 task3，task2，task1，再将自身删除即可。
