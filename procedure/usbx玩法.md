# usbx玩法

cubemx中可以直接点选
使用usbx
coreSytem必选

分两条路 host和device
host模式必选 UX Host CoreStack
	      UX Host Controllers

根据需要功能分为以下可选
```text
UX Network Driver设备
HID设备（人机交互）   UX Host Class HID Core（必选）UX Host Class HID Keyboard  UX Host Class HID Mouse UX Host Class HID RCU多媒体遥控器
HUB（ 集线器类驱动）最多 5 级 Hub
STORAGE（大容量储存）  可配合FILEX
UX Host Class CDC ACM（USB 虚拟串口设备）对面也得是usb设备，不可直接接串口，可接ch340
UX Host Class CDC ECM	(USB 虚拟网卡)
UX Host Class AUDIO    USB 音频设备
UX Host Class VIDEO USB 摄像头
UX Host Class PRINTER USB 打印机驱动
```

device同理
