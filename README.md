# Qt Workspace

这是一个个人 Qt 学习与项目实践仓库，包含多个练习项目、实验项目以及独立的小型桌面应用。

## 重点项目

### InformationShow

`InformationShow` 是一个基于 Qt Widgets 的环境监测上位机项目，用于通过 TCP 与下位机通信，并展示实时传感器数据。

主要功能：

- 支持主机模式和客户机模式切换
- 支持 TCP 连接、数据接收与简单控制发送
- 实时显示温度、湿度、可燃气体、粉尘/面粉浓度等数据
- 使用 Qt Charts 绘制噪声曲线
- 提供调试窗口查看收发内容
- 提供数据库窗口查看历史记录

项目目录：

- `InformationShow/`

项目说明文档：

- [InformationShow/README.md](InformationShow/README.md)

## 仓库说明

- 本仓库除 `InformationShow` 外，还包含其他 Qt 相关练习或测试项目
- 某些文件如 `.pro.user` 属于本地 Qt Creator 配置文件，可能因个人环境不同而变化
