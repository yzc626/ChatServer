/*
muduo网络库给用户提供两个类
TcpServer ： 编写服务器程序
TcpClient ： 编写客户端程序

epoll + 线程池
好处：能够把网络I/O的代码和业务代码区分开
                        用户的连接和断开，用户的可读写事件
*/
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <iostream>
#include <functional>
#include <string>
using namespace std;
using namespace muduo;
using namespace muduo::net;
using namespace placeholders;

/*基于muduo网络库开发服务器程序
1.组合TcpServer对象
2.创建EventLoop事件循环对象的指针
3.明确TcpServer构造函数的参数，输出ChatServer的构造函数
4.在服务器类的构造函数中，注册处理连接的回调函数和处理读写时间的回调函数
5.设置服务端线程数量，muduo库自动分配I/O线程和worker线程
*/
class ChatServer{
public:
    ChatServer(EventLoop *loop,               // 事件循环
               const InetAddress &listenAddr, // IP+Port端口
               const string &nameArg)         // 服务器名
        : _server(loop, listenAddr, nameArg), _loop(loop)
    {
        // 4.给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

        // 4.给服务器注册用户读写事件回调
        _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

        // 5.设置服务器端线程数量 1个I/O线程 ，3个worker线程
        _server.setThreadNum(4);
    }

    void start(){
        _server.start();
    }

private:
    // 处理用户的连接、创建、断开  epoll listenfd accept
    void onConnection(const TcpConnectionPtr &conn){
        if (conn->connected()){
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:online" << endl;
        }else{
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << " state:offline" << endl;
            conn->shutdown(); // close(fd)断开
        }
    }

    // 专门处理用户的读写事件
    void onMessage(const TcpConnectionPtr &conn, // 连接
                   Buffer *buffer,               // 缓冲区
                   Timestamp time)               // 接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << " time:" << time.toFormattedString() << endl;
        conn->send(buf); //返回
    }

    TcpServer _server; // 1
    EventLoop *_loop;  // 2 
};

int main(){
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");

    server.start(); // listenfd epoll_ctl=>epoll
    loop.loop();    // epoll_wait以阻塞方式等待新用户连接，已连接用户的读写事件等
}