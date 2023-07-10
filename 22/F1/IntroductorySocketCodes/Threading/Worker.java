package Threading;

import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.net.Socket;
import java.util.Date;

public class Worker extends Thread {
    Socket socket;

    public Worker(Socket socket)
    {
        this.socket = socket;
    }

    public void run()
    {
        // buffers
        try {
            ObjectOutputStream out = new ObjectOutputStream(this.socket.getOutputStream());
            ObjectInputStream in = new ObjectInputStream(this.socket.getInputStream());

            while (true)
            {
                Thread.sleep(1000);
                Date date = new Date();
                out.writeObject(date.toString());
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}
