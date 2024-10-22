#include <U8g2lib.h>
#include <WiFi.h>
#include <time.h>
#include <BleKeyboard.h>

// 屏幕相关配置
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 5, /* dc=*/ 16, /* reset=*/ 17);

// WiFi 配置
const char* ssid = "Your_ssid";
const char* password = "Your_password";

// NTP 时间服务器配置
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 0;

// 状态标志
bool lastSyncSuccess = false;  // 上一次同步是否成功
bool isTimeSynchronized = false;  //用于检测时间是否已同步
unsigned long lastSyncAttempt = 0;  // 上次同步尝试时间
unsigned long lastWiFiCheck = 0;    // 上次 WiFi 检查时间
const unsigned long syncInterval = 60 * 1000;  // 1分钟同步间隔
const unsigned long wifiCheckInterval = 60 * 1000;  // 1分钟WiFi检查间隔
int wifiAttempts = 0;
const int maxWiFiAttempts = 10; // 最大尝试次数

unsigned long wifiDisconnectedTime = 0; // 记录WiFi断开时的开始时间
unsigned long offlineSeconds = 0;  // 断开后的计时
unsigned long lastDisplayUpdate = 0;
unsigned long displayInterval = 1000;  // 每秒更新一次显示

// 7个状态
const char* states[] = {"氢", "锂", "钠", "钾", "铷", "铯", "钫"};
// 蓝牙键盘初始化（自定义蓝牙名称和制造商）
BleKeyboard bleKeyboard("N/A_key", "Anrieal", 100);
// 按键引脚定义（假设6个物理按键连接到以下GPIO引脚）
const int buttonPins[] = {32, 33, 25, 26, 12, 13};
const int numButtons = 6;
bool buttonState[6] = {false, false, false, false, false, false};

// 按键映射 F13 到 F18 键码
uint8_t keyMap[] = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB, 0xFA};  // F13-F18 键码

bool BTconnect = false;  // 初始化蓝牙连接状态

void connectToWiFi() {
    Serial.println("正在尝试连接 WiFi...");
    WiFi.begin(ssid, password);
    
    while (WiFi.status() != WL_CONNECTED && wifiAttempts < maxWiFiAttempts) {
        delay(1000);
        Serial.print(".");
        wifiAttempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("WiFi 已连接");
    } else {
        Serial.println("连接 WiFi 失败");
    }
}

// 尝试同步时间
void syncTime() {
    configTime(0, 0, ntpServer);
    Serial.println("时间同步尝试...");
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        lastSyncSuccess = true;
        isTimeSynchronized = true;  // 设置时间已同步标志
    } else {
        lastSyncSuccess = false;
    }
}


void setup() {
  Serial.begin(115200);

  // OLED 初始化
  u8g2.begin();
  u8g2.clearBuffer();

  // 连接到 WiFi
  connectToWiFi();

  // 同步时间
  syncTime();
  bleKeyboard.begin();

  // 初始化按键引脚为输入
  for (int i = 0; i < numButtons; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);  // 使用内部上拉电阻
  }
}


void loop() {
  // 每秒更新 OLED 显示内容
  if (millis() - lastDisplayUpdate >= displayInterval) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }

  // 检查是否需要进行时间同步 (每1分钟尝试)
  if (millis() - lastSyncAttempt >= syncInterval) {
    attemptTimeSync();
    lastSyncAttempt = millis();
  }

  // 每分钟检测 WiFi 状态
  if (millis() - lastWiFiCheck >= wifiCheckInterval) {
    checkWiFiStatus();
    lastWiFiCheck = millis();
  }

  // 蓝牙按键处理
  handleBluetoothKeyboard();

  // 检查蓝牙连接状态
  BTconnect = bleKeyboard.isConnected();  // 更新蓝牙连接状态
}

// 蓝牙键盘按键检测和处理函数
void handleBluetoothKeyboard() {
  if (bleKeyboard.isConnected()) {
    for (int i = 0; i < numButtons; i++) {
      bool isPressed = digitalRead(buttonPins[i]) == LOW;

      if (isPressed && !buttonState[i]) {
        // 处理按键按下事件
        if (keyMap[i] == 0xFF) {  // Ctrl+C
          bleKeyboard.press(KEY_LEFT_CTRL);
          bleKeyboard.press('c');
          bleKeyboard.release('c');
          bleKeyboard.release(KEY_LEFT_CTRL);
          Serial.println("Key pressed: Ctrl+C");
        } else if (keyMap[i] == 0xFE) {  // Ctrl+V
          bleKeyboard.press(KEY_LEFT_CTRL);
          bleKeyboard.press('v');
          bleKeyboard.release('v');
          bleKeyboard.release(KEY_LEFT_CTRL);
          Serial.println("Key pressed: Ctrl+V");
        } else if (keyMap[i] == 0xFD) {  // Ctrl+Z
          bleKeyboard.press(KEY_LEFT_CTRL);
          bleKeyboard.press('z');
          bleKeyboard.release('z');
          bleKeyboard.release(KEY_LEFT_CTRL);
          Serial.println("Key pressed: Ctrl+Z");
        } else if (keyMap[i] == 0xFC) {  // Ctrl+Y
          bleKeyboard.press(KEY_LEFT_CTRL);
          bleKeyboard.press('y');
          bleKeyboard.release('y');
          bleKeyboard.release(KEY_LEFT_CTRL);
          Serial.println("Key pressed: Ctrl+Y");
        } else if (keyMap[i] == 0xFB) {  // Ctrl+T
          bleKeyboard.press(KEY_LEFT_CTRL);
          bleKeyboard.press('t');
          bleKeyboard.release('t');
          bleKeyboard.release(KEY_LEFT_CTRL);
          Serial.println("Key pressed: Ctrl+T");
        } else if (keyMap[i] == 0xFA) {  // Alt+Tab
          bleKeyboard.press(KEY_LEFT_ALT);
          bleKeyboard.press(KEY_TAB);
          bleKeyboard.release(KEY_TAB);
          bleKeyboard.release(KEY_LEFT_ALT);
          Serial.println("Key pressed: Alt+Tab");
        } else {
          // 发送其他按键
          bleKeyboard.press(keyMap[i]);
          Serial.print("Key pressed: F");
          Serial.println(13 + i);
        }

        buttonState[i] = true;  // 更新按键状态
      } else if (!isPressed && buttonState[i]) {
        // 处理按键松开事件
        if (keyMap[i] != 0xFF && keyMap[i] != 0xFE && keyMap[i] != 0xFD && keyMap[i] != 0xFC && keyMap[i] != 0xFB && keyMap[i] != 0xFA) {
          bleKeyboard.release(keyMap[i]);
        }
        buttonState[i] = false;
      }
    }
  }
}


// 更新显示内容
void updateDisplay() {
    u8g2.firstPage();  // 准备显示第一页
    do {
        u8g2.clearBuffer();  // 清空缓冲区
        u8g2.enableUTF8Print();
        u8g2.setFont(u8g2_font_wqy12_t_gb2312);  // 设置字体

        if (isTimeSynchronized) {
            // 获取本地时间并转换为 UNIX 时间戳
            struct tm timeinfo;
            if (getLocalTime(&timeinfo)) {
              time_t unixTime = mktime(&timeinfo);  // 获取 UNIX 时间戳

              // 计算节点年和日
              int epoch_seconds_per_day = 1200;  //一日对应的秒
              int epoch_days_per_year = 64;  //一节点对应的日数
              long total_days = unixTime / epoch_seconds_per_day;
              int year = total_days / epoch_days_per_year;  // 计算节点
              int day = total_days % epoch_days_per_year;   // 计算节点日
              int secondsToday = unixTime % epoch_seconds_per_day;   // 计算当日已经过的秒数

              // 计算当前状态
              const char* currentState = states[day % 7];  // 根据天数轮换状态

              // 在 OLED 上显示节点
              u8g2.clearBuffer();  // 清除屏幕内容
              u8g2.enableUTF8Print();
              u8g2.setFont(u8g2_font_wqy12_t_gb2312);  // 设置字体

              // 显示节点和状态
              u8g2.setCursor(0, 10);  // 设置光标位置
              u8g2.print("时间节点 ");
              u8g2.print(year);
              u8g2.print(" 束 ");
              u8g2.print(currentState);  // 显示状态

              // 显示日
              u8g2.setCursor(0, 25);  
              u8g2.print(day);
              u8g2.print(" 日");
              u8g2.print(secondsToday);

              // 显示断开时的秒数
              if (WiFi.status() != WL_CONNECTED) {
                offlineSeconds = (millis() - wifiDisconnectedTime) / 1000;  // 计算断开后的秒数
                u8g2.setCursor(64, 25);  // 将秒数显示在日的中间位置
                u8g2.print(offlineSeconds);
                u8g2.print(" <");
              }

              // 显示 UNIX 时间戳
              u8g2.setCursor(0, 40);
              u8g2.print("> ");
              u8g2.print(unixTime);


                
                        }
        } else {
            // 显示初始化状态
            u8g2.setCursor(0, 10);
            u8g2.print("基础状态  时间待同步");
            if (WiFi.status() == WL_CONNECTED) {
              u8g2.setCursor(0, 25);
              u8g2.print("等待时间初次同步");
            } else {
              u8g2.setCursor(0, 25);
              u8g2.print("等待接入数据总线");
            }
            u8g2.setCursor(0, 40);
            u8g2.print("近距通信可用");
        }

        // 显示 WiFi 和蓝牙状态
        if (WiFi.status() == WL_CONNECTED) {
            u8g2.setCursor(78, 55);
            u8g2.print("环星总线");
            isTimeSynchronized = lastSyncSuccess;  // 连接后更新同步状态
        } else {
            u8g2.setCursor(78, 55);
            u8g2.print("本地计算");
        }

        if (lastSyncSuccess) {
            u8g2.setCursor(64, 55);
            u8g2.print(" ");
        }else {
            u8g2.setCursor(64, 55);
            u8g2.print("△");
        }

        if (BTconnect) {
            u8g2.setCursor(0, 55);
            u8g2.print("近距通信");
        } else {
            u8g2.setCursor(0, 55);
            u8g2.print("准备就绪");
        }

    } while (u8g2.nextPage());  // 发送内容到显示器
}



// 尝试同步时间（1分钟执行一次，连续尝试5次）
void attemptTimeSync() {
  int attempts = 0;
  const int maxAttempts = 5;
  while (attempts < maxAttempts) {
    syncTime();
    if (lastSyncSuccess) {
      break;  // 如果同步成功，跳出循环
    }
    attempts++;
    delay(500);  // 每次尝试间隔
  }
}

// 检查 WiFi 状态并尝试重连
void checkWiFiStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    if (wifiDisconnectedTime == 0) {
      wifiDisconnectedTime = millis();  // 记录断开WiFi的时间
    }
    Serial.println("WiFi 断开，正在重连...");
    WiFi.reconnect();
  }
}
