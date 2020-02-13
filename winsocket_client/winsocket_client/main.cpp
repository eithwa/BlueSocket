#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <string.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#define BUF_SIZE 1024
using namespace std;
void draw_bar(int percentage, int buffer_count, int file_size)
{
    string bar;
    bar+="[";
    for(int i=0; i<20; i++)
    {
        if(percentage/5>=i)
        {
            bar+="|";
        }
        else
        {
            bar+=" ";
        }
    }
    bar+="] ";
    printf("%s ",bar.c_str());
    printf("%3d%% (%d/%d) %.2lfm\n", percentage, (int)buffer_count/1024, (int)file_size, (double)file_size/(double)1024);
}
int main(int argc, char *argv[]){
    //先输入文件名，看文件是否能创建成功
    // char filename[100] = {0};  //文件名
    // printf("Input filename to save: ");
    // gets(filename);

    char *ip="127.0.0.1";
    char *filename="";  //文件名
    char *filesize="0";
    // printf("We have %d arguments:\n", argc);
    for (int i = 0; i < argc; ++i) {
		if(i>3)break;
        // printf("[%d] %s\n", i, argv[i]);
        if(i==1)
            ip=argv[i];
        else if(i==2)
            filename=argv[i];
        else if(i==3)
            filesize=argv[i];
    }
    int file_size =atoi(filesize);
    printf("IP: %s\n", ip);
    printf("File Directory: %s\n", filename);
    printf("File size: %d\n", file_size);
	//Sleep(10000);
    FILE *fp = fopen(filename, "wb");  //以二进制方式打开（创建）文件
    if(fp == NULL){
        printf("Cannot open file, press any key to exit!\n");
        system("pause");
        exit(0);
    }
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    sockaddr_in sockAddr;
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = PF_INET;
    // sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    sockAddr.sin_addr.s_addr = inet_addr(ip);
    sockAddr.sin_port = htons(1234);
    connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
    //循环接收数据，直到文件传输完毕
    char buffer[BUF_SIZE] = {0};  //文件缓冲区
    int nCount=-1;
    double buffer_count = 0;
    printf("Waiting to download file...\n");
    static clock_t start_time=clock();
    clock_t end_time=clock();
	
    int percentage=0;
    while( (nCount = recv(sock, buffer, BUF_SIZE, 0)) > 0 || nCount==-1)
    {
        if(nCount!=-1)
        {
            if(nCount>0)buffer_count+=nCount;
            fwrite(buffer, nCount, 1, fp);
            end_time=clock();
            if(end_time-start_time>500)
            {   
                // system("cls");
                start_time=end_time;
                // printf("buffer_count %d\n",buffer_count);
                
                percentage= buffer_count*100/1024/file_size;
                draw_bar(percentage, buffer_count, file_size);
            }
        }   
    }
    percentage= buffer_count*100/1024/file_size; 
    draw_bar(percentage, buffer_count, file_size);
    
    puts("File transfer success!");
    //文件接收完毕后直接关闭套接字，无需调用shutdown()
    fclose(fp);
    closesocket(sock);
    WSACleanup();
    Sleep(1000);
	//system("pause");
    return 0;
}