# ele 
`游戏服务器框架(c++11)`

```
  1. 支持 tcp,  http, websocket, udp, sudp
  2. 异步多线程 epoll, protobuf
  3, 帧同步， 状态同步
  4. 支持lua
  5. 支持 php，lua, python 写单元测试，提供机器人测试工具(c++)
  6. 支持mysql，redis
  7. 有网关（c++,tcp）(提供对client连接)
     会话(c++,tcp) (游戏逻辑服)
     战斗(c++,tcp) 
     gm代理服(c++,http) (gm查询)
     client代理服(c++,http) (client拉取服务器列表，网关列表)
     支付(nodejs,http) (支付回调)
     匹配(c++,tcp) (战斗匹配)
     日志收集(golang + kafka, socket(或者udp))
     日志分析(hadoop，java) 
     
  8. 有GM后台 (vue + php7),
	 1. 支持服务器管理(新增服，开服，关服，查询状态，更新服，清档，备份，合服，修改服务器时间)
	 2. 支持玩家管理(玩家信息查询，玩家信息修改，踢人，封号)
	 3 .支持游戏统计日志查询，支持系统操作日志查询
	 4. 支持游戏活动管理(活动新增，活动修改)
	 5. 支持gm用户权限管理
	 6. 礼包码管理
	 7. 支持多语言
	 8. gm账号管理
	 9. 发邮件
	 10. 客户端配置管理(客户端的配置) + http服务 (redis + golang)
	 
  9. 代码开源， 文档
  10. 只支持linux
  11. 支持docker，提供dockerfile 和使用文档
  12. 提供各种案例
  13. 支持zookeeper ,提供服务发现
```