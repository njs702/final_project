# MAIN PROJECT - CRC
## Communications Remote controlled Car

Project using socket &amp; bluetooth &amp; CAN with RPi4,Arduino UNO, Mega, ESP32

## 1. 프로젝트 개요

> 커넥티드카 기반 차량 데이터 모니터링, ECU 제어 및 연계 제어 시스템 구축

* 사물 인터넷 장치를 활용한 자동차 네트워크 형성
* 차량 내 전자 장치(EUC) 통신을 넘어선 개방형 모빌리티
* CAN(Controlled Area Network) 통신 프로토콜을 활용한 ECU간 데이터 교환
* 제어 시스템 간 연계 시스템 구축

## 2. 시스템 구조

<p align="center"><img src="./imgs/system_architecture.PNG"></p>

## 3. THREE MINI PROJECTS

* Windows socket 기반 채팅 프로그램 - [GITHUB LINK](https://github.com/njs702/project_oneclick)
* 차량 주변 기기 통신 프로젝트 - [GITHUB LINK](https://github.com/njs702/project_SCE)
* CAN BUS 데이터 모니터링 프로젝트 - [GITHUB LINK](https://github.com/njs702/CAN_BUS)

### 3.1 Windows socket 기반 채팅 프로그램

<p align="center"><img src="./imgs/minipr1_implementation.PNG"></p>

<p align="center">그림1 - 서버와 클라이언트</p>

* TCP/IP 기반 SOCKET 통신
* Multithread for multiple clients
* Singlethread for multiple clients - multiplexing

<hr/>

### 3.2 차량 주변 기기 통신 프로젝트

<p align="center"><img src="./imgs/accident_catch.PNG"></p>

<p align="center">그림2 - 사고 감지 및 알림 시스템</p>

<p align="center"><img src="./imgs/drowsy_catch.PNG"></p>

<p align="center">그림3 - 졸음운전 방지 시스템</p>

* TCP/IP 기반 MQTT 통신
* Google assistant + OPEN API (Python)
* MQTT 통신 기반 사고 알림 시스템
* 졸음운전 방지 시스템

<hr/>

### 3.3 CAN BUS 데이터 모니터링 프로젝트

<p align="center"><img src="./imgs/PCAN_data_monitoring.PNG"></p>

<p align="center">그림4 - CAN BUS 데이터 모니터링</p>

<p align="center"><img src="./imgs/can_3bus_result.jpg"></p>

<p align="center">그림5 - CAN_3BUS 결과</p>

* CAN communication
* 제어 시스템 간 연계 시스템 구축
* 다른 종류의 단말 간 통신(RPI, ARDUINO UNO)
* 3대 이상의 multiple devices

<hr/>

## 4. MAIN PROJECT

> CRC - Communications Remote Controlled Car

* CAN 통신을 활용한 제어 시스템 간 데이터 교환 & 제어 연계 시스템 구축
* TCP/IP 기반 SOCKET 통신을 활용한 데이터 수집 & 시각화
* TCP/IP 기반 MQTT 통신을 활용한 사고 알림 시스템
* Bluetooth 통신 기반 원격 제어(with ESP32 & Wii remote)

### 4.1 CAN 통신을 활용한 제어 시스템 간 데이터 교환 & 제어 연계 시스템 구축
**GOALS**

1. 서로 다른 장치 간 연계 시스템 구축하기
2. CAN 통신 프로토콜을 활용해 데이터 교환 & 자동 처리
3. 서로 다른 종류의 단말 간 데이터 교환(Arduino, RPI)

<p align="center"><img src="./imgs/4.1_architecture.PNG"></p>

<p align="center">그림6 - 구조도</p>

<p align="center"><img src="./imgs/4.1_implementation.PNG"></p>

<p align="center">그림5 - 조도 센서 with CAN 동작 화면</p>

**구현 원리**

* 3대의 디바이스 - 조도 센서 및 온/습도 센서 데이터 측정 단말(MEGA), LED 제어 단말(UNO), 온/습도 데이터 저장 단말(RPI)
* MEGA에서는 지속적으로 센서를 통해 데이터 수집
* 조도 센서 값이 특정 임계점을 넘는 순간 CAN BUS를 통해 데이터를 SEND
* UNO에서는 해당 trigger가 발생하면, interrupt를 발생시켜 LED를 켜게 된다.
* RPI는 CAN 통신 전용 스레드를 통해 MEGA로부터 각종 센서 데이터를 받고, 지속적으로 저장한다.


