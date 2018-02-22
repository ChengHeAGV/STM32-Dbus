叉车AGV控制系统


1.UCOSIII中以下优先级用户程序不能使用，ALIENTEK将这些优先级分配给了UCOSIII的5个系统内    部任务。
  优先级0：中断服务服务管理任务 OS_IntQTask()
  优先级1：时钟节拍任务 OS_TickTask()
  优先级2：定时任务 OS_TmrTask()
  优先级OS_CFG_PRIO_MAX-2：统计任务 OS_StatTask()
  优先级OS_CFG_PRIO_MAX-1：空闲任务 OS_IdleTask()

2.系统包含5个任务
  任务1.
  任务2.地标检测任务，使用串口3与地标传感器进行通信
  任务3.
  任务4.
  任务5.

3.工控屏通信
  使用定时器3和串口2，使用Modbus协议通信，自动更新数据
4.电压测量，计算电量


西安泰赛科技