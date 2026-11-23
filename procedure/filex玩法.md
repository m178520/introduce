# filex用法

##usbx大容量存储
filex文件系统最终控制底层，还是需要控制驱动程序，usbx提供了此驱动层程序ux_host_class_storage_driver_entry.c
我们直接调用即可
// 直接用USBX原生自带的storage驱动！
            fx_status = fx_media_open(
                &g_usb_fx_media,
                "USB_DISK",
                _ux_host_class_storage_driver_entry,   // USBX原生FileX驱动入口
                g_storage_ptr,                         // 传给驱动的参数：storage实例
                fx_media_buffer,
                sizeof(fx_media_buffer)
            );

## sd卡
如果是使用的其他储存设备，比如sd卡、NAND FLASH等在File System Interfaces中选择

比如SD Interface 已经内置好不需要

可是这些都是软件如果与硬件相关必须要自己将硬件底层调用函数接入filex
NAND FLASH需要将底层port对接入上层 LevelX NAND custom是预留的接口，NOR FLASH同理

>具体详见cubemx的filex配置界面

