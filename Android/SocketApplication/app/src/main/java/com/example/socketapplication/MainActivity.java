package com.example.socketapplication;

import androidx.appcompat.app.AppCompatActivity;

import android.icu.util.TimeUnit;
import android.os.Bundle;
import android.os.Handler;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.ObjectOutputStream;
import java.io.OutputStream;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {

    Button connect_btn, disconnect_btn,send_btn, recv_btn;
    Socket socket;
    TextView welcome_message;
    EditText send_to_server;
    OutputStream outputStream;
    String from_server;
    private Handler mHandler;

    String host = "192.168.0.80";
    int port = 9000;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        welcome_message = findViewById(R.id.from_server);
        connect_btn = findViewById(R.id.connect_button);
        disconnect_btn = findViewById(R.id.disconnect_button);
        send_btn = findViewById(R.id.send_button);
        recv_btn = findViewById(R.id.recv_button);

        send_to_server = findViewById(R.id.send_to_server);

        Handler handler = new Handler();



        // 쓰레드 사용해서 서버에 연결
        connect_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                ClientThread thread = new ClientThread();
                thread.start(); // 소켓 생성

            }
        });
        // 서버에서 연결 끊기
        disconnect_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                try{
                    //socket.close();
                    String close_message = "###";
                    SendThread sendThread = new SendThread(close_message);
                    sendThread.start();
                    outputStream.close();

                }catch (IOException e){
                    e.printStackTrace();
                }
            }
        });
        // 서버에 데이터 보내기
        send_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String text = send_to_server.getText().toString();
                Handler handler = new Handler();

                SendThread sendThread = new SendThread(text);
                sendThread.start();

                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {}
                },100);

                SendThread.interrupted();
            }
        });
        // 데이터 받기
        recv_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

        ClientThread thread = new ClientThread();
        thread.start(); // 소켓 생성

        // 소켓 생성 대기 시간 기다리기
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                RecvThread recvThread = new RecvThread();
                recvThread.start();
            }
        },5000);


    }

    // 소켓 생성 및 연결 쓰레드
    class ClientThread extends Thread {
        String original = "connected";
        byte[] byteArr = new byte[512];
        byte[] temp = original.getBytes();

        @Override
        public void run() {
            try {
                socket = new Socket(host, port);
                try {
                    OutputStream outstream = socket.getOutputStream();
                    InputStream inputStream = socket.getInputStream();

                    //mHandler = new Handler();

                    int readByteCount = inputStream.read(byteArr);
                    String from_server = new String(byteArr,0,readByteCount);
                    outstream.write(temp);
                    outstream.flush();
                    welcome_message.setText(from_server);
                    //mHandler.post(mUpdate);

                }catch (IOException e){
                    e.printStackTrace();
                }
                // socket.close();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    // 연결된 소켓을 통해 데이터 보내는 쓰레드
    class SendThread extends Thread{
        String message;

        public SendThread(String message){
            this.message = message;
        }

        @Override
        public void run(){
            System.out.println("a");
            try {
                byte[] temp = message.getBytes();
                outputStream = socket.getOutputStream();
                outputStream.write(temp);
            }catch (IOException e){
                e.printStackTrace();
            }
        }

    }

    // 연결된 소켓을 통해 데이터 받는 쓰레드
    class RecvThread extends Thread{
        byte[] byteArr_recv;

        public RecvThread(){
            byteArr_recv = new byte[512];
        }

        @Override
        public void run(){
            System.out.println("b");

            try {
                InputStream inputStream = socket.getInputStream();

                while(true){
                    int readByteCount = inputStream.read(byteArr_recv);
                    from_server = new String(byteArr_recv,0,readByteCount);
                    System.out.println(from_server);
                    Thread.sleep(3000);
                    //welcome_message.setText(from_server);
                }

            }catch (IOException | InterruptedException e){
                e.printStackTrace();
            }
        }

    }

}