package Server;

import Server.Frame.ServerFrame;
import Server.Service.Server;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class MainServer {
    public static void main(String[] args) throws IOException {
        Server server = new Server();
        ServerFrame frame = new ServerFrame(server);
        try {
            ServerSocket serverSocket = new ServerSocket();
            serverSocket.bind(new InetSocketAddress("127.0.0.1",8086));
            System.out.println("�������Ѿ���������ַΪ:" + serverSocket.getLocalSocketAddress());
            new Thread(()-> { //�������߳̽��пͻ������Ӽ���
                while (true) {
                    Socket socket = null;
                    try {
                        socket = serverSocket.accept();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                    Socket finalSocket = socket;
                    new Thread(() -> { //�������߳�ȥ��������
                        server.processRequest(finalSocket);
                    }).start();
                }
            }).start();
        }catch (Exception e){
        	e.printStackTrace();
        }
    }
}
