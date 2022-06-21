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

