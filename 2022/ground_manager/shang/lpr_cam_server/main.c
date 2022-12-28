#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "cam.h"
#include "convert.h"

#include<sys/types.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<arpa/inet.h>

/***********************/
//摄像头引用变量
/***********************/
int i;
int fd;
int ret;
unsigned int width;
unsigned int height;
unsigned int size;
unsigned int index_cam;
unsigned int ismjpeg;
char *yuv;
char *rgb;
/***********************/

int server_init();
int cam_on();
int get_pic();

/***********************/
//服务器引用变量
/***********************/
char bufpic[1024*1024];//存放图片的内容
char buf[20];//装图片的大小
int rws;
int s;
/***********************/
//main主函数
/***********************/
int main()
{	
	//开启摄像头
	cam_on();	
	//创建服务器
	s = server_init("192.168.190.128",8888);
	if(s < 0)
	{
		printf("server_init failed\n");
		return -1;
	}
	printf("s = %d\n", s);
	printf("wait for a client...\n");
	//等待连接 --> accept
	struct sockaddr_in c_addr;
	socklen_t c_len = sizeof(c_addr);   
	rws = accept(s, (struct sockaddr *)&c_addr, &c_len);
	if(rws < 0)
	{
		perror("accept");
		close(s);
		return -1;
	}
	printf("Connected\n");
	//发送数据	
	while(1)
	{
		//采集摄像头图片
		get_pic();
		//服务器发送图片大小和内容
		memset(bufpic, 0, 1024*1024);
		memset(buf, 0, 20);		
		int fd2 = open("1.jpg", O_RDONLY);
		if(fd2 < 0)
		{
			perror("open 83");
			close(s);
			return -1;
		}
		int len = 0;//保存图片大小
		//求图片大小
		while(1)
		{
			int ret = read(fd2, bufpic+len, 1024);
			if(ret <= 0)
				break;

			len = len + ret;
		}		
		//把图片大小写入buf
		sprintf(buf, "%d", len);	
		//发送图片大小
		write(rws, buf, 20);
		//再发图片内容
		write(rws, bufpic, len);
		printf("piclen = %d\n",len);
		len = 0;
		close(fd2);
	}
}

/*****************************************************************************/ 
//服务器初始化 返回套件字
/*****************************************************************************/ 
int server_init(char *ip,int port)
{
	//1.创建套接字 --> socket
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("socket");
		return -1;
	}
	//2.绑定ip和端口 --> bind
	struct sockaddr_in s_addr = {
		.sin_family  = AF_INET,
		.sin_port    = htons(port),
		.sin_addr    = {
			.s_addr  = inet_addr(ip), 
		},
		.sin_zero    = {0},
	};

	socklen_t s_len = sizeof(s_addr);

	if(bind(sockfd, (struct sockaddr *)&s_addr, s_len) < 0)
	{
		perror("bind");
		close(sockfd);
		return -1;
	}
	//3.设置监听数 --> listen
	ret = listen(sockfd,20);
	if(ret<0)
	{
		perror("listen");
		return -1;
	}
	return sockfd;
}
/*****************************************************************************/
//摄像头初始化
/*****************************************************************************/
int cam_on()
{
	/* A8的屏幕比较小，所以设了较低的像素 */
	width = W;
	height = H;
	fd = camera_init(CAMERA_USB, &width, &height, &size, &ismjpeg);
	if (fd == -1)
		return -1;
	ret = camera_start(fd);
	if (ret == -1)
		return -1;
	jpg = malloc(sizeof(struct jpg_buf_t));
	if (!jpg) {
		perror("malloc");
		return -1;
	}
	if (ismjpeg != 0) {
		printf("------yuyv------\n");
		rgb = malloc(width * height * 3);
		convert_rgb_to_jpg_init();
	}
	// 采集几张图片丢弃 
	for (i = 0; i < 8; i++) {
		ret = camera_dqbuf(fd, (void **)&yuv, &size, &index_cam);
		if (ret == -1)
			exit(EXIT_FAILURE);

		ret = camera_eqbuf(fd, index_cam);
		if (ret == -1)
			exit(EXIT_FAILURE);
	}

	fprintf(stdout, "init camera success\n");
}
/*****************************************************************************/
//循环采集图片
/*****************************************************************************/
int get_pic()
{
	ret = camera_dqbuf(fd, (void **)&yuv, &size, &index_cam);//从帧缓存中提取一张采集的图片到缓存区(出队)
	if (ret == -1)
		return -1;
	if (ismjpeg == 0) {
		pthread_mutex_lock(&cam_mutex);
		memcpy(jpg->jpg_buf, yuv, size);
		jpg->jpg_size = size;
		pthread_mutex_unlock(&cam_mutex);
	} else {
		convert_yuv_to_rgb(yuv, rgb, width, height, 24);
		pthread_mutex_lock(&cam_mutex);
		memset(jpg, 0, sizeof(jpg));
		jpg->jpg_size = convert_rgb_to_jpg_work(rgb, jpg->jpg_buf, width, height, 24, 80);
		pthread_mutex_unlock(&cam_mutex);
	}

	int fd1 = open("1.jpg", O_RDWR | O_CREAT, 0777);
	int count = 0;
	while(count <  jpg->jpg_size){
		int ret = write(fd1, jpg->jpg_buf + count, jpg->jpg_size - count);
		if (ret < jpg->jpg_size){
			printf("-----数据太少-----\n");
		}
		count += ret;
	}		
	close(fd1);

	ret = camera_eqbuf(fd, index_cam);// 投放数据缓冲区到队列(入队)
	if (ret == -1)
		return -1;
}
