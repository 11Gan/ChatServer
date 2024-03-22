# ChatServer
基于mudou库的集群聊天服务器和客户端代码 nginx-tcp负载均衡 redis-订阅发布模式 
```text
# c++编译和构建工具
sudo apt install build-essential
# 查看g++版本 gcc --version

# 代码调试工具
sudo apt install gdb

# cmake
sudo apt install cmake

# boost
sudo apt-get install libboost-all-dev

# mysql库
sudo apt-get install libmysqlclient-dev

# redies库
sudo apt-get install libhiredis-dev
```

boost安装:https://blog.csdn.net/QIANGWEIYUAN/article/details/88792874

muduo安装:https://blog.csdn.net/QIANGWEIYUAN/article/details/89023980

mysql安装与使用:https://blog.csdn.net/hwx865/article/details/90287715

```
sudo apt-get install mysql-server
```
## 项目路径

bin -- 二进制

build -- 中间文件路径

cmake -- camke函数文件夹

CmakeLists.txt --cmake的定义文件

lib -- 库的输出路径

## 负载均衡

**nginx**

1. **把client的请求按照负载算法分发到具体的业务服务器上**
2. **能够和ChatServer保持心跳机制,监测ChatServer故障**
3. **能够发现新添加的ChatServer设备,方便扩展服务器数量**

高性能的网络设备

支持可配置的负载算法,与后台服务器保持心跳,动态监测ChatServer故障

不影响正在享受服务客服端的情况下,动态的添加新的服务器,而不将原业务服务器重启 

```
// 添加TCP配置
// 下载nginx包并解压
./configure --with-stream 
make && make install
默认安装在了/usr/local/nginx目录
可执行文件在sbin目录里面，配置文件在conf目录里面

nginx -s reload 重新加载配置文件启动
nginx -s stop 停止nginx服务

```
