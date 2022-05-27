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



/* ============== GLOBAL functions ============== */
void init_bind_socket_tcp(){

    // 서버를 위한 소켓 생성
    // PF_INET - 프로토콜 체계(프로토콜 패밀리), SOCK_STREAM - 전송 타입(연결 지향형 소켓->tcp), IPPROTO_TCP - 프로토콜(연결 지향형)
    server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(server_socket == -1){
        perror("Socket()");
        return;
    }

    // 입력받는 포트 번호를 사용해 서비스를 운영체제에 등록
    memset(&server_addr,0,sizeof(server_addr));
    server_addr.sin_family = AF_INET; // AF_INET - 주소 체계(주소 패밀리)
    server_addr.sin_addr.s_addr = inet_addr(IP);
    //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);

    // 소켓 bind
    if(bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        printf("bind()");
        return;
    }
    printf("TCP Socket creation & bind success!\n");
} // TCP 소켓 생성 및 바인드 함수
/* ============================================== */



int main(){
    init_bind_socket_tcp();
}