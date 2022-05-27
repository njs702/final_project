#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h> // 파일 처리를 위한 헤더파일
#include <unistd.h> // POSIX 운영체제 API 엑세스 제공 헤더파일

#include <pthread.h> // 쓰레드 헤더파일
#include <arpa/inet.h> // hton, ntoh , htons, ... etc

#include <sys/socket.h> // socket, bind, listen, ...
#include <sys/types.h>
#include <sys/ioctl.h>
#include <netinet/in.h> // sockaddr_in 
#include <net/if.h>

#include <stdint.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <signal.h>



/* ============== MACRO DEFINE ============== */
#define PORT 9000
#define IP "192.168.0.80"
/* ========================================== */



/* ============== GLOBAL custom struct & union ============== */
typedef struct{
	float humid;
	float temp; 
}temp_humid_data;

union temp_humid_union {
    temp_humid_data first;
    unsigned char second[8];
}; 
/* ========================================================== */



/* ============== GLOBAL variables ============== */
static int server_socket; // TCP socket
static struct sockaddr_in server_addr, client_addr;

static temp_humid_data temp_humid;

static int can_socket; // CAN socket
static struct sockaddr_can addr;
static struct ifreq ifr; // ifreq structure contaning the interface name
/* ============================================== */




int main(){
    
}