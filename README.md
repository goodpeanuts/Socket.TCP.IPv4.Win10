# 基于TCP协议WINDOWS下的IPv4简单socket网络通信传输文件实现

> 程序中的IP地址与端口请根据实际情况修改

## 服务端

###### 

- C/S约定使用版本2.2，短暂端口65035

- 服务端 循环运行，持续监听
- 服务端 先行提供可选择传输文件列表供客户端选择
- 服务端 根据 客户端 请求打开指定文件并读文件

## 客户端

- ###### 访问服务器地址 `130.33.40.2`

- 选择并发送需要接收的文件的文件名

- 客户端 收到后新建并写入相应文件 保存文件名格式为 `received_" + filename`