这是 带有屏幕的蓝牙小键盘
六个按键 开机默认显示 蓝牙名称 状态名称 历法(默认焉虚历)(需要和unix时间戳有换算关系) 可自行定义 
记得改好自己的wifi名称与密码
使用ESP32 Dev Module两侧各15针脚 共30 <你永远不知道拼夕夕商家卖的什么货> 请确认针脚就在那里
按照打孔 逆时针方向读取 
3V3 GND D15 D2 D4 D16 D17 D5 D18 D19 D21 RXD TXD D22 D23 EN VP VN D334 D35 D32 D33 D25 D26 D27 D14 D12 D13 GND VIN
pcb的文件是 嘉立创EDA_Pro 的文件 一个
源文件
直接EDA打开 然后直接选中pcb 然后下单 原理图哪里没花 因为是直接借用了一个针脚位置相近的ESP32模块
一个导出的Gerber文件
不会用 你研究下?试试嘉立创EDA的导入zip看看?

关于明明是蓝牙却为什么叫近场通信
是这样的你可以改成近距通信 一番讨论下来我们可以认为它属于近场通信(

只是借用了针脚 焊接时让ESP32没有串口那一端的针脚尽量靠近OLED显示屏后焊接 忽略靠近边缘的4对焊点
OLED是0.96寸的SPI接口的 
记得向arduinoIDE导入lib里面的文件 那个ESP32_BLE_Keyboard是改过的 网络上找到的源文件不能直接用 这是源
https://github.com/T-vK/ESP32-BLE-Keyboard
版本是
https://github.com/T-vK/ESP32-BLE-Keyboard/releases/tag/0.3.0


祝你顺利
