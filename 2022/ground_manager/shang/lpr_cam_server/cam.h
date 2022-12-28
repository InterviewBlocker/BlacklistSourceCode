#ifndef __CAM_H__
#define	__CAM_H__

#define JPG_MAX_SIZE	(1024 * 1024 - sizeof (unsigned int))
#define SIZE 16

#define CAMERA_USB "/dev/video0"
#define W 320
#define H 240


struct jpg_buf_t {
	char jpg_buf[JPG_MAX_SIZE];
	unsigned int jpg_size;
};
extern int cam_fd;

struct jpg_buf_t *jpg;
int sync_flag;
pthread_mutex_t cam_mutex;

int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned int *size, unsigned int *ismjpeg);
int camera_start(int fd);
int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index);
int camera_eqbuf(int fd, unsigned int index);
int camera_stop(int fd);
int camera_exit(int fd);
int camera_on();
int get_pic();
#endif
