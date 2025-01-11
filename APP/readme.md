## Application

### Description

- sys: 系统初始化和相关系统任务
- msg: 通讯任务
- ins: 陀螺仪任务
- motor: 电机驱动任务
- gimbal: 云台任务
- chassis: 底盘任务

### Note

- 应用层代码应当只关注应用逻辑，而无需关注底层实现。

- 除 `system` 外，任何任务均应在函数最开始加上类似下面的代码，从而保证该任务在系统初始化结束后运行（包括但不限于各应用的初始化、陀螺仪初始化）。
```c++
while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);
```

- 在应用层的实现上，应尽可能保证代码风格和架构的统一。