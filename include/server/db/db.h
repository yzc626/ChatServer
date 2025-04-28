#ifndef DB_H
#define DB_H

#include <mysql/mysql.h>
#include <string>
using namespace std;

// 数据库操作类
class MySQL{
public:
    // 初始化连接
    MySQL();
    // 释放资源
    ~MySQL();
    // 连接
    bool connect();
    // 更新
    bool update(string sql);
    // 查询
    MYSQL_RES *query(string sql);
    // 获取连接
    MYSQL* getConnection();
private:
    MYSQL *_conn;
};

#endif