package fileserver;

import java.io.Serializable;
import java.net.SocketAddress;

public class UserFileRequest implements Serializable {
    
    String username;
    SocketAddress clientAddress;

    public String getUsername() {
        return this.username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public SocketAddress getClientAddress() {
        return this.clientAddress;
    }

    public void setClientAddress(SocketAddress clientAddress) {
        this.clientAddress = clientAddress;
    }

    public UserFileRequest (String username, SocketAddress clientAddress) {
        this.username = username;
        this.clientAddress = clientAddress;
    }
}
