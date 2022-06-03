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

struct Vector7i{
    uint16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;
};

union temp_humid_union {
    temp_humid_data first;
    unsigned char second[8];
}; 

union vector7i_union {
    Vector7i first;
    unsigned char second[16];
};

union distance_union{
    float first;
    unsigned char second[8];
};
/* ========================================================== */



/* ============== GLOBAL variables ============== */
static int server_socket; // TCP socket
static struct sockaddr_in server_addr, client_addr;

static temp_humid_data temp_humid;
static Vector7i v7i; // gyro data

static float ultra_distance; // ultrasonic data

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

static void* client_connection(void* arg){

    // int형 파일 디스크립터로 변환
    int csock = *((int*)arg);
    int read_size;
    char dataRecv;
    char message[BUFSIZ], client_message[BUFSIZ], client_ip[BUFSIZ];

    inet_ntop(AF_INET,&client_addr.sin_addr,client_ip,BUFSIZ);

    // Receive a message from client
    while(1){
        while((read_size = recv(csock, client_message, BUFSIZ, 0)) > 0){
            // end of string marker
            client_message[read_size] = '\0';
            printf("From client %s:%d: %s\n",client_ip,client_addr.sin_port,client_message);
        }
        // 연결 해제 신호 문자열 : ###
        if(strcmp(client_message,"###")){
            break;
        }
    }
    

    if(read_size == 0){
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1){
        puts("Client disconnected");
    }

    return 0;
} // 클라이언트 접속 시 수행하는 스레드함수

void init_bind_socket_can(){
    /* ======================  Create Socket using 3 parameters ======================
	domain/protocol family(PF_CAN), type of socket(raw or datagram),socket protocol */
	if((can_socket=socket(PF_CAN,SOCK_RAW,CAN_RAW)) < 0){
		perror("Socket");
		return;
	}

	/* Retrieve the interface index for the interface name(can0, can1, vcan0, etc ... ) */
	strcpy(ifr.ifr_name,"can0");

	/* 네트워크 관리 스택 접근을 위한 I/O controll, 사용자 영역에서 커널 영역의 데이터 접근(system call) using 3 parameters
	an open file descriptor(s), request code number(SIOCGIFINDEX), value */
	ioctl(can_socket,SIOCGIFINDEX,&ifr);

	// Bind Socket to the CAN interface
	memset(&addr,0,sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(can_socket,(struct sockaddr *)&addr, sizeof(addr)) < 0){
		perror("Bind");
		return;
	}

	printf("CAN Socket creation & bind success!\n");
} // CAN 소켓 생성 및 바인드 함수

static void* can_communication(void* arg){
    int nBytes;
	//struct sockaddr_can addr;
	struct can_frame recieve_frame;
	struct can_frame send_frame;

    struct can_filter rfilter[1];
	rfilter[0].can_id   = 0x550;
	rfilter[0].can_mask = 0xFF0;

    while(1){

        // Reading a frame
	    nBytes = read(can_socket,&recieve_frame,sizeof(struct can_frame));
        if (nBytes < 0){
            perror("Read");
            return NULL;
        }

        // 수신 id에 따른 분기 처리
        switch(recieve_frame.can_id){
            // 온습도 데이터 id값 수신
            case 0x11:
                static union temp_humid_union a;
                for (int i = 0; i < 8; ++i)
                    a.second[i] = recieve_frame.data[i];
                
                temp_humid.humid = a.first.humid;
                temp_humid.temp = a.first.temp;
                break;

            // 자이로 센서 앞 4개 데이터 수신
            case 0x12:
                static union vector7i_union b;
                for(int i=0;i<8;i++){
                    b.second[i] = recieve_frame.data[i];
                }
                v7i.AcX = b.first.AcX;
                v7i.AcY = b.first.AcY;
                v7i.AcZ = b.first.AcZ;
                v7i.Tmp = b.first.Tmp;
                break;
            
            // 자이로 센서 뒤 4개 데이터 수신
            case 0x13:
                static union vector7i_union c;
                for(int i=0;i<8;i++){
                    c.second[i+8] = recieve_frame.data[i];
                }
                v7i.GyX = c.first.GyX;
                v7i.GyY = c.first.GyY;
                v7i.GyZ = c.first.GyZ;
                break;
                
            case 0x14:
                static union distance_union d;
                for(int i=0;i<8;i++){
                    d.second[i] = recieve_frame.data[i];
                }
                ultra_distance = d.first;
                
                break;

            default:
                break;
        }

        /* printf("%f %f\n",temp_humid.humid,temp_humid.temp);
        printf("%d %d\n",v7i.AcX,v7i.AcY);
        printf("%d %d\n",v7i.GyX,v7i.GyY); */

        sleep(0.3);
    }

    if (close(can_socket) < 0) {
		perror("Close");
		return NULL;
	}

    return 0;
} // CAN 통신 전용 스레드함수

static void* send_message(void* arg){
    char temp_humid_message[BUFSIZ];
    int csock = *((int*)arg);
    while(1){
        sprintf(temp_humid_message,"%f %f",temp_humid.humid,temp_humid.temp);
        if(write(csock,temp_humid_message,strlen(temp_humid_message)) == -1){
            printf("write error(socket disconnected)\n");
            break;
        }
        else{
            printf("send message : %s\n",temp_humid_message);
            sleep(10);
        }
        
    }
    return 0;
} // 온,습도 정보 보내는 스레드함수
/* ============================================== */



int main(int argc, char **argv){
    pthread_t thread;
    pthread_t can_thread;
    pthread_t send_thread;

    struct sigaction new_actn, old_actn;
    new_actn.sa_handler = SIG_IGN;
    sigemptyset (&new_actn.sa_mask);
    new_actn.sa_flags = 0;
    sigaction (SIGPIPE, &new_actn, &old_actn);

    init_bind_socket_tcp();
    init_bind_socket_can();

    // 최대 10대의 클라이언트 동시 접속 처리를 위한 큐 생성
    if(listen(server_socket,10)==-1){
        perror("listen()");
        return -1;
    }

    // CAN 통신 처리용 쓰레드 생성
    pthread_create(&can_thread,NULL,can_communication,NULL);

    while(1){
        char mesg[BUFSIZ];
        int client_socket;

        // 클라이언트의 요청을 기다린다
        len = sizeof(client_addr);
        client_socket = accept(server_socket,(struct sockaddr*)&client_addr,&len);

        // 네트워크 주소를 문자열로 변경
        inet_ntop(AF_INET,&client_addr.sin_addr,mesg,BUFSIZ);
        printf("Client IP : %s:%d\n",mesg,ntohs(client_addr.sin_port));

        // 클라이언트의 요청이 들어오면 스레드를 생성하고 클라이언트의 요청 처리
        // 클라이언트 소켓을 매개변수로 넘겨서 스레드에서 해당 소켓으로 통신
        pthread_create(&thread,NULL,client_connection,&client_socket);
        pthread_create(&send_thread,NULL,send_message,&client_socket);
        // 쓰레드를 만들어서 온,습도 정보 데이터를 안드로이드로 보낸다

        /* pthread_join(thread,NULL);
        pthread_join(send_thread,NULL); */
    }

    pthread_join(thread,NULL);
    pthread_join(send_thread,NULL);
    pthread_join(can_thread,NULL);
    printf("CAN thread deleted!!!\n");
    return 0;
    
}