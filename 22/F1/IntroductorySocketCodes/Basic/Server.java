package Basic;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.ServerSocket;
import java.net.Socket;

public class Server {
    public static void main(String[] args) throws IOException, ClassNotFoundException {
        ServerSocket welcomeSocket = new ServerSocket(6666);

        // while loop, so that server can connect to another client
        // after serving the current one
        while(true) {
            System.out.println("Waiting for connection...");
            Socket socket = welcomeSocket.accept();
            System.out.println("Connection established");
            System.out.println("Remote port: " + socket.getPort());
            System.out.println("Local port: " + socket.getLocalPort());

            // output buffer and input buffer
            ObjectOutputStream out = new ObjectOutputStream(socket.getOutputStream());
            ObjectInputStream in = new ObjectInputStream(socket.getInputStream());

            // send message to client
            out.writeObject("Hello from server");

            // read message from client
            String msg = (String) in.readObject();
            System.out.println(msg);
        }

    }
}
