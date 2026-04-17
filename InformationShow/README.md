# InformationShow

`InformationShow` 是一个基于 Qt Widgets 的上位机监控项目，用于通过 TCP 与下位机通信，并展示环境监测数据。

## 功能简介

- 支持主机模式和客户机模式切换
- 支持 TCP 建立连接、接收数据和发送简单控制指令
- 实时显示温度、湿度、可燃气体、粉尘/面粉浓度、蜂鸣器/LED 状态、噪声值
- 使用 Qt Charts 绘制噪声曲线
- 提供调试窗口，便于查看收发数据
- 提供数据库窗口，可查看、筛选和清空历史数据

## 技术栈

- Qt 5
- Qt Widgets
- Qt Network
- Qt Charts
- Qt SQL
- MySQL

## 项目结构

- `main.cpp`：程序入口
- `mainwindow.*`：主界面、网络通信、数据解析、图表显示
- `deb.*`：调试窗口
- `database.*`：数据库窗口与查询逻辑
- `*.ui`：Qt Designer 界面文件
- `pic.qrc`、`img/`：资源文件
- `InformationShow.pro`：qmake 工程文件

## 数据格式

程序当前按如下格式解析下位机上传的数据：

```text
ESP8266{temp:xx humi:xx mq9:xx flour:xx led&&beep:xx db:xx}
```

其中主要字段包括：

- `temp`：温度
- `humi`：湿度
- `mq9`：可燃气体浓度
- `flour`：粉尘/面粉浓度
- `led&&beep`：蜂鸣器和 LED 状态
- `db`：噪声值

## 构建方式

### 1. 环境要求

- Qt 5.x
- MinGW 或与 Qt 对应的编译器
- MySQL 及 Qt 对应的 `QMYSQL` 驱动

### 2. 使用 Qt Creator

1. 用 Qt Creator 打开 `InformationShow.pro`
2. 选择可用的 Qt Kit
3. 直接构建并运行

### 3. 使用命令行

```powershell
qmake InformationShow.pro
mingw32-make
```

## 数据库配置

当前数据库连接参数写死在 `database.cpp` 中：

- Host: `127.0.0.1`
- Port: `3306`
- Database: `sensordata`
- User: `root`
- Password: `123123`

请先确保本地 MySQL 中已存在对应数据库和数据表，并且 Qt 能正确加载 `QMYSQL` 驱动。

## 运行说明

1. 启动程序
2. 选择主机模式或客户机模式
3. 设置 IP 和端口
4. 打开网络连接
5. 收到下位机数据后，界面会自动刷新图表与数值
6. 可通过调试窗口查看数据收发内容
7. 可通过数据库窗口查看历史记录

## 已知注意事项

- 当前数据库配置为硬编码，不适合直接用于生产环境
- 调试发送、数据解析和数据库查询逻辑仍有进一步优化空间
- 工程中存在 `.pro.user` 文件，这通常是本地 Qt Creator 配置，不建议作为协作时的核心配置文件依赖

## License

本项目暂未附带明确许可证；如需开源分发，建议补充 LICENSE 文件。
