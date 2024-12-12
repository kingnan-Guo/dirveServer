#include <iostream>
#include "stdio.h"
// 网络通信需要加载的 头文件和 需要加载的 库文件
#include <winsock2.h>
#include <ws2tcpip.h>  // 可选，提供更高层次的协议接口
#pragma comment(lib, "W2_32.LIB")// 它指示链接器链接 W2_32.LIB 库，这可能是一个与 Windows 相关的库（名字可能类似于标准的 Windows 库，如 User32.lib 或 Gdi32.lib，不过 W2_32.LIB 并不是一个标准的库，可能是一个自定义库）。

void error_die(const char *  str){
    perror(str);// 打印错误名
    exit(1);
}



/**
 * 初始化
 *
 * @param port  port 如果是 0  就自动分配一个可以用 端口
 * @return int 返回套接字
 *
 * 1、网络通信初始化 WSAStartup ; 参数 网络通信的 参数
 * 2、创建 套接字  socket；
 *        套接字分类：
 *              1、 网络套接字 : 插槽创建好之后 通过插槽 进行 互联网通信的是 网络套接字
 *              2、 文件套接字：  服务器电脑内部,；两个不同的程序 进行通信， 传输 通信，
 * 3、设置 套接字 属性端口可以 复用 setsockopt， 因为 多次启停服务的时候  可能端口 会被占用 导致服务启动不起来，所以 设置端口 可以复用
 * 4、 绑定 套接字 和网络地址，网络插槽
 *          1\配置服务器的网络地址
 *          2\设置 内存
 *          3\配置成员
 *          4\ 绑定 配置好 的地址 和  套接字
 *
 * 5、 动态分配一个 端口
 * 6、创建一个监听队列 ： 有多个 用户 同时访问，那么需要 排队
 *
 *
 */
int startup(unsigned short * port){
    // 1、 网络通信初始化
    WSADATA data;
    int res = WSAStartup(
            MAKEWORD(1, 1), // 1.1 版本的协议
            //初始化 结果
            &(data)
    );
    if(res){
        //return  -1;
        error_die("WSAStartup  error");
    }

    // 2、创建 套接字
    // 返回值 是  套接字
     int serve_socket = socket(
            PF_INET,// 套接字 类型
            SOCK_STREAM, // sock 协议 ； 数据流   ； 还以一种类型 是 数据报
            IPPROTO_TCP// 具体协议； 每一个 通信包裹 使用什么协议
    );
    if(serve_socket == -1){
        error_die("scoket  error");
    }

    // 3、设置端口可以复用
    int opt = 1;
    int setsockopt_res = setsockopt(serve_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));//
    if(setsockopt_res == -1){
        error_die("setsockopt_res  error");
    }



    // 4、 绑定 套接字 和网络地址
    //      1 配置服务器的网络地址
    struct sockaddr_in server_addr;
    //      2、设置 内存
    memset(
            &server_addr,
            0,
            sizeof(server_addr)
    );
    //      3、配置成员
    server_addr.sin_family = AF_INET;// 网络地址类型  AF_INET 对应上面  PF_INET; 套接字 类型里的 ；
    server_addr.sin_port = htons(*port);// 配置端口;  由于 字节序 可能不同 所以 绑定一个 类型转换  ； htons 本地网络字节序 转换
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// ip 地址; 这里是任意 IP 地址都可以访问？？？；； 绑定本机的所有 IP 地址

    //      4、套接字 和地址就绑定到了 一起
     int bind_res = bind(
            serve_socket,
            (struct sockaddr * ) &server_addr,
            sizeof (server_addr)
    );
    if(bind_res < 0){
        error_die("bind  error");
    }

    // 5、动态分配一个 端口
    int nameLenght = sizeof (server_addr);
    if(* port == 0){
        // 获取动态端口号
        int getsockname_res= getsockname(
                serve_socket,
                (struct sockaddr * ) &server_addr,// 把当前可以用的端口号 存储到  server_addr里面了
                &nameLenght
        );
        if(getsockname_res < 0){
            error_die("getsockname  error");
        }

        * port = server_addr.sin_port;


    }

    // 6、创建一个监听队列
    if(listen(serve_socket, 5)){
        error_die("listen  error");
    }


    return serve_socket;
}

int main() {
    std::cout << "start server!" << std::endl;

    unsigned short port = 0;// 再网络开发里面 端口是 无符号， 范围是 0~65536; 无符号类型只能存储非负数，因此与有符号类型相比，它能够表示更大的正整数范围
    int server_sock = startup(&port);
//    std::cout << "http server running ,port =" + server_sock   << std::endl;
    printf("http server running ,port = %d\r\n", port );
    return 0;
}
