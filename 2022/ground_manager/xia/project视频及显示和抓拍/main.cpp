#include <iostream>

#include "./aip-cpp-sdk-0.7.10/ocr.h"
#include "cJSON.h"
using namespace std;

/*
arm-linux-g++  -std=c++11   main.cpp   -o main
 -I /home/gec/libcurl/include/
  -I  /home/gec/libjson/include/
   -I /home/gec/libopenssl/include/
    -L /home/gec/libcurl/lib   -L
     /home/gec/libjson
       -L /home/gec/libopenssl/lib
        -lcurl -lcrypto  -ljsoncpp  -lssl

*/
int s;

extern "C"
{
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */

/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
//添加IP地址结构体头文件
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DEV_6818_PATH "/dev/video7"

    void creat_bmp(unsigned char *rgb, char *bmp_file);
    int yuyv2rgb0(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height);
    //用户自定义的图像缓存BUF
    struct buffer
    {
        void *start;
        unsigned int length;
    };
}

int rws;
char bufpic[1024 * 1024]; //存放图片的内容
char buf[20];             //装图片的大小
#include <stdio.h>

//定义BMP 的头数据
typedef struct /**** BMP file header structure ****/
{
    unsigned int bfSize;        /* Size of file */
    unsigned short bfReserved1; /* Reserved */
    unsigned short bfReserved2; /* ... */
    unsigned int bfOffBits;     /* Offset to bitmap data */
} MyBITMAPFILEHEADER;

typedef struct /**** BMP file info structure ****/
{
    unsigned int biSize;         /* Size of info header */
    int biWidth;                 /* Width of image */
    int biHeight;                /* Height of image */
    unsigned short biPlanes;     /* Number of color planes */
    unsigned short biBitCount;   /* Number of bits per pixel */
    unsigned int biCompression;  /* Type of compression to use */
    unsigned int biSizeImage;    /* Size of image data */
    int biXPelsPerMeter;         /* X pixels per meter */
    int biYPelsPerMeter;         /* Y pixels per meter */
    unsigned int biClrUsed;      /* Number of colors used */
    unsigned int biClrImportant; /* Number of important colors */
} MyBITMAPINFOHEADER;

// rgb to BMP
void creat_bmp(unsigned char *rgb, const char *bmp_file)
{
    char buf[480][640][3] = {0};
    int width = 640;
    int height = 480;
    MyBITMAPFILEHEADER bfh;
    MyBITMAPINFOHEADER bih;

    unsigned short bfType = 0x4d42;
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfSize = 2 + sizeof(MyBITMAPFILEHEADER) + sizeof(MyBITMAPINFOHEADER) + width * height * 3;
    bfh.bfOffBits = 0x36;

    bih.biSize = sizeof(MyBITMAPINFOHEADER);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = 0;
    bih.biXPelsPerMeter = 5000;
    bih.biYPelsPerMeter = 5000;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    FILE *file = fopen(bmp_file, "wb");
    if (!file)
    {
        printf("Could not write file\n");
        return;
    }
    int i, j, k;
    for (i = 479; i >= 0; i--)
    {
        for (j = 0; j < 640; j++)
        {
            for (k = 2; k >= 0; k--)
            {
                buf[i][j][k] = *rgb;
                rgb++;
            }
        }
    }

    fwrite(&bfType, sizeof(bfType), 1, file);
    fwrite(&bfh, sizeof(bfh), 1, file);
    fwrite(&bih, sizeof(bih), 1, file);

    fwrite(buf, width * height * 3, 1, file);
    fclose(file);
}

int yuyv2rgb(int y, int u, int v)
{
    unsigned int pixel24 = 0;
    unsigned char *pixel = (unsigned char *)&pixel24;
    int r, g, b;
    static int ruv, guv, buv;

    // 色度
    ruv = 1596 * (v - 128);
    guv = 391 * (u - 128) + 813 * (v - 128);
    buv = 2018 * (u - 128);

    // RGB
    r = (1164 * (y - 16) + ruv) / 1000;
    g = (1164 * (y - 16) - guv) / 1000;
    b = (1164 * (y - 16) + buv) / 1000;

    if (r > 255)
        r = 255;
    if (g > 255)
        g = 255;
    if (b > 255)
        b = 255;
    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;

    pixel[0] = r;
    pixel[1] = g;
    pixel[2] = b;

    return pixel24;
}

// YUYV to  RGB
int yuyv2rgb0(unsigned char *yuv, unsigned char *rgb, unsigned int width, unsigned int height)
{
    unsigned int in, out;
    int y0, u, y1, v;
    unsigned int pixel24;
    unsigned char *pixel = (unsigned char *)&pixel24;
    unsigned int size = width * height * 2;

    for (in = 0, out = 0; in < size; in += 4, out += 6)
    {
        // YUYV
        y0 = yuv[in + 0];
        u = yuv[in + 1];
        y1 = yuv[in + 2];
        v = yuv[in + 3];

        pixel24 = yuyv2rgb(y0, u, v); // RGB
        rgb[out + 0] = pixel[0];
        rgb[out + 1] = pixel[1];
        rgb[out + 2] = pixel[2];

        pixel24 = yuyv2rgb(y1, u, v); // RGB
        rgb[out + 3] = pixel[0];
        rgb[out + 4] = pixel[1];
        rgb[out + 5] = pixel[2];
    }
    return 0;
}

class video
{
public:
    video(const char *path)
    {
        // 1.open device.打开摄像头设备
        fd = open(path, O_RDWR, 0); //弄了好久 以阻塞模式打开摄像头  | O_NONBLOCK 非堵塞
        if (fd < 0)
        {
            printf("open device failed.\n");
        }
        printf("open device success.->fd=%d\n", fd);

        // 2.show all supported format.显示所有支持的格式
        struct v4l2_fmtdesc fmtdesc;
        fmtdesc.index = 0;                          // form number
        fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // frame type
        while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
        {
            printf("VIDIOC_ENUM_FMT success.->fmt.fmt.pix.pixelformat:%s\n", fmtdesc.description);
            fmtdesc.index++;
        }

        // 3.set or gain current frame.设置或查看当前格式
        struct v4l2_format fmt;
        memset(&fmt, 0, sizeof(fmt));
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        // V4L2_PIX_FMT_RGB32   V4L2_PIX_FMT_YUYV   V4L2_STD_CAMERA_VGA  V4L2_PIX_FMT_JPEG
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
        fmt.fmt.pix.width = 640;
        fmt.fmt.pix.height = 480;
        if (ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
        {
            printf("VIDIOC_S_FMT failed.\n");
            return;
        }
        printf("VIDIOC_S_FMT sucess.\n");

        //查看格式
        if (ioctl(fd, VIDIOC_G_FMT, &fmt) == -1)
        {
            printf("VIDIOC_G_FMT failed.\n");
            return;
        }
        printf("VIDIOC_G_FMT sucess.->fmt.fmt.width is %ld\nfmt.fmt.pix.height is %ld\n\
fmt.fmt.pix.colorspace is %ld\n",
               fmt.fmt.pix.width, fmt.fmt.pix.height, fmt.fmt.pix.colorspace);

        // 6.1 request buffers.申请缓冲区
        struct v4l2_requestbuffers req;
        req.count = 2; // frame count.帧的个数
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP; // automation or user define．自动分配还是自定义
        if (ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
        {
            printf("VIDIOC_REQBUFS map failed.\n");
            close(fd);
            exit(-1);
        }
        printf("VIDIOC_REQBUFS map success.\n");

        //为用户自定义的映射地址分配空间
        buffers = (struct buffer *)calloc(req.count, sizeof(*buffers)); //分配2块缓存

        unsigned int n_buffers = 0;
        struct v4l2_buffer buf;
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
        {

            //查询序号为n_buffers 的缓冲区，得到其起始物理地址和大小
            memset(&buf, 0, sizeof(buf));
            buf.index = n_buffers;
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
            {
                printf("VIDIOC_QUERYBUF failed.\n");
                close(fd);
                exit(-1);
            }
            printf("VIDIOC_QUERYBUF success.\n");

            // memory map   把2块缓存地址映射到用户空间
            buffers[n_buffers].length = buf.length;
            buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
            if (MAP_FAILED == buffers[n_buffers].start)
            {
                printf("memory map failed.\n");
                close(fd);
                exit(-1);
            }
            printf("memory map success.\n");

            // Queen buffer.将缓冲帧放入队列
            if (ioctl(fd, VIDIOC_QBUF, &buf) == -1)
            {
                printf("VIDIOC_QBUF failed.->n_buffers=%d\n", n_buffers);
                return;
            }
            printf("VIDIOC_QBUF.->Frame buffer %d: address=0x%x, length=%d\n",
                   n_buffers, (unsigned int)buffers[n_buffers].start, buffers[n_buffers].length);
        }
    };

    //启动摄像头抓取功能
    void video_star()
    {
        enum v4l2_buf_type type;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMON, &type) == -1)
        {
            printf("VIDIOC_STREAMON failed.\n");
            return;
        }
        printf("VIDIOC_STREAMON success.\n");
    }

    //停止摄像头抓取功能
    void video_stop()
    {
        enum v4l2_buf_type type;
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
        {
            printf("VIDIOC_STREAMON failed.\n");
            return;
        }
        printf("VIDIOC_STREAMOFF success.\n");
    }

    //获取RGB数据
    unsigned char *get_rgb()
    {
        // 8.DQBUF.取出一帧
        struct v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        if (ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
        {
            printf("VIDIOC_DQBUF failed.->fd=%d\n", fd);
            return NULL;
        }

        yuyv2rgb0((unsigned char *)buffers[buf.index].start, rgb_buf, 640, 480); //把YUV 转成 RGB

        // 10.QBUF.把一帧放回去
        if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
        {
            printf("VIDIOC_QBUF failed.->fd=%d\n", fd);
            return NULL;
        }

        return rgb_buf;
    }

    //保存图片
    void save_pic(const char *pic_path)
    {
        creat_bmp(rgb_buf, pic_path);
    }

private:
    int fd; //摄像头描述符
    unsigned char rgb_buf[640 * 480 * 3];
    struct buffer *buffers; //指向两个帧缓存
};
const char *get_string(cJSON *json, const char *key)
{
    cJSON *value = cJSON_GetObjectItem(json, key);

    return cJSON_GetStringValue(value);
}
//摄像头全局对象
video *v;

void *Capture_task(void *arg)
{   
    int fd=open("./pipe",O_RDWR);
    if(fd < 0)
        {
            perror("打开管道文件失败\n");
            return nullptr;
        }else 
        {
            printf("打开管道文件成功1\n");
        }
    
    int fd1=open("./pipe1",O_RDWR);
    if(fd1 < 0)
        {
            perror("打开管道文件失败\n");
            return nullptr;
        }else 
        {
            printf("打开管道文件成功2\n");
        }
  
    while (1)
    {
        //2.读取管道文件中的数据
        char buf[1024]={0}; 
        read(fd,buf,1024); 
        
        cout<<"等待操作\n";
        if (strcmp(buf,"ok")==0)
        {
            v->save_pic("1.bmp");

            int fd2 = open("1.bmp", O_RDONLY);
            if (fd2 < 0)
            {
                perror("open 83");
                close(s);
                return NULL;
            }
            int len = 0; //保存图片大小
            //求图片大小
            while (1)
            {
                int ret = read(fd2, bufpic + len, 1024);
                if (ret <= 0)
                    break;

                len = len + ret;
            }
            //把图片大小写入buf
            sprintf(buf, "%d", len);
            //发送图片大小
            write(rws, buf, 20);
            //再发图片内容
            write(rws, bufpic, len);
            printf("piclen = %d\n", len);
            len = 0;
            close(fd2);
            // 1.新建一个客户端
            //  设置APPID/AK/SK
            std::string app_id = "27937117";
            std::string api_key = "tbfvc2GGzVlHhpaDWqhr2N1p";
            std::string secret_key = "0Be0RMAKgXq8xz40Fh0p5t1N9urzZj9e";
            aip::Ocr client(app_id, api_key, secret_key);

        Json::Value result;
        std::string image;
        aip::get_file_content("1.bmp", &image);
        // 2.调用通用文字识别（含位置高精度版）
        result = client.accurate(image, aip::null);

        cJSON *json = cJSON_Parse(result.toStyledString().c_str());
        if (json == NULL) //判断是否解析成功
        {
            //获取错误信息
            const char *error_ptr =
                cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                //输出错误信息
                fprintf(stderr, "Error before %s\n", error_ptr);
            }
        }
        else
        {
            printf("解析成功\n");
            //通过小对象 forecast 获取数组
            cJSON *arry = cJSON_GetObjectItem(json, "words_result");
            //获取数组中的元素
            int size = cJSON_GetArraySize(arry);
            printf("当前数组大小 %d\n", size);
            cJSON   *obj = cJSON_GetArrayItem(arry,0);
            
            write(rws,get_string(obj,"words"),strlen(get_string(obj,"words")));
            printf("值:%d",strlen(get_string(obj,"words")));

        }

        std::cout << result << std::endl;


        }else if(strcmp(buf,"no")==0)
        {
            v->save_pic("2.bmp");

            int fd2 = open("2.bmp", O_RDONLY);
            if (fd2 < 0)
            {
                perror("open 83");
                close(s);
                return NULL;
            }
            int len = 0; //保存图片大小
            //求图片大小
            while (1)
            {
                int ret = read(fd2, bufpic + len, 1024);
                if (ret <= 0)
                    break;

                len = len + ret;
            }
            //把图片大小写入buf
            sprintf(buf, "%d", len);
            //发送图片大小
            write(rws, buf, 20);
            //再发图片内容
            write(rws, bufpic, len);
            printf("piclen = %d\n", len);
            len = 0;
            close(fd2);

                    // 1.新建一个客户端
        //  设置APPID/AK/SK
        std::string app_id = "27937117";
        std::string api_key = "tbfvc2GGzVlHhpaDWqhr2N1p";
        std::string secret_key = "0Be0RMAKgXq8xz40Fh0p5t1N9urzZj9e";
        aip::Ocr client(app_id, api_key, secret_key);

        Json::Value result;
        std::string image;
        aip::get_file_content("1.bmp", &image);
        // 2.调用通用文字识别（含位置高精度版）
        result = client.accurate(image, aip::null);

        cJSON *json = cJSON_Parse(result.toStyledString().c_str());
        if (json == NULL) //判断是否解析成功
        {
            //获取错误信息
            const char *error_ptr =
                cJSON_GetErrorPtr();
            if (error_ptr != NULL)
            {
                //输出错误信息
                fprintf(stderr, "Error before %s\n", error_ptr);
            }
        }
        else
        {
            printf("解析成功\n");
            //通过小对象 forecast 获取数组
            cJSON *arry = cJSON_GetObjectItem(json, "words_result");
            //获取数组中的元素
            int size = cJSON_GetArraySize(arry);
            printf("当前数组大小 %d\n", size);
            cJSON   *obj = cJSON_GetArrayItem(arry,0);
            
            write(rws,get_string(obj,"words"),strlen(get_string(obj,"words")));
            printf("值:%d",strlen(get_string(obj,"words")));
            
        }

            std::cout << result << std::endl;
        }
        else if(strcmp(buf,"ok1")==0)
        {   
            char bb[100]={0};
            char bg[1024]={0};
            read(fd,bg,1024);
            write(rws, bg, strlen(bg));
            write(fd1, bg, strlen(bg));
            
           
            
                
                
            
                
            
            
        }    
    
    }
}


int server_init(const char *ip, int port)
{
    // 1.创建套接字 --> socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }
    // 2.绑定ip和端口 --> bind
    struct sockaddr_in s_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {
            .s_addr = inet_addr(ip),
        },
        .sin_zero = {0},
    };

    socklen_t s_len = sizeof(s_addr);
    int on = 1;
    int r = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    if (bind(sockfd, (struct sockaddr *)&s_addr, s_len) < 0)
    {
        perror("bind");
        close(sockfd);
        return -1;
    }
    // 3.设置监听数 --> listen
    int ret1 = listen(sockfd, 20);
    if (ret1 < 0)
    {
        perror("listen");
        return -1;
    }
    return sockfd;
}

int main()
{
    // 1.打开LCD 设备
    int fd = open("/dev/fb0", O_RDWR);

    //映射LCD 设备
    int *lcd = (int *)mmap(NULL, 800 * 480 * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    //定义一个摄像头对象
    v = new video("/dev/video7");

    s = server_init("192.168.64.192", 8888);
    if (s < 0)
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
    if (rws < 0)
    {
        perror("accept");
        close(s);
        return -1;
    }
    
    //开启摄像头
    v->video_star();

    // unsigned char rgb_buf[640*480*3];

    //开启一个线程
    pthread_t tid;
    pthread_create(&tid, NULL, Capture_task, NULL);

    while (1)
    {
        //采集帧数据
        unsigned char *rgb = v->get_rgb();

        for (int y = 0; y < 480; y++)
            for (int x = 0; x < 640; x++)
            {
                //转换为一个32位色
                unsigned char r = *rgb++;
                unsigned char g = *rgb++;
                unsigned char b = *rgb++;

                int color = r << 16 | g << 8 | b;

                *(lcd + 800 * y + x) = color;
            }
    }
}