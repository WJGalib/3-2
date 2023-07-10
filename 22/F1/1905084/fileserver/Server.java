package fileserver;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;

public class Server {

    private ServerSocket serverSocket;
    public HashMap<String, User> clientMap;

    Server() {
        clientMap = new HashMap<>();
        try {
            serverSocket = new ServerSocket(33333);
            while (true) {
                Socket clientSocket = serverSocket.accept();
                serve(clientSocket);
            }
        } catch (Exception e) {
            System.out.println("Server starts:" + e);
        }
    }

    public void serve(Socket clientSocket) throws IOException, ClassNotFoundException {
        NetworkUtil networkUtil = new NetworkUtil(clientSocket);
        // NetworkUtil activityChecker = new NetworkUtil(clientSocket);
        new ReadThreadServer(clientMap, networkUtil);
        (new Thread() {
            public void run() {
                clientMap.forEach((name, user) -> {
                    try {
                        user.getNetworkUtil().write("ALIVE?");
                        //user.setOnline(true);
                    } catch (IOException e) {
                        user.setOnline(false);
                    }
                });
            }
        }).start();
    }

    public static void main(String args[]) {
        Server server = new Server();
    }
}
