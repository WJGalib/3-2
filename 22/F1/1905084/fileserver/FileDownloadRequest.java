package fileserver;

import java.io.Serializable;
import java.net.SocketAddress;

public class FileDownloadRequest implements Serializable {
    String filename;
    String username;
    SocketAddress clientAddress;


    public SocketAddress getClientAddress() {
        return this.clientAddress;
    }

    public void setClientAddress(SocketAddress clientAddress) {
        this.clientAddress = clientAddress;
    }

    public String getFilename() {
        return this.filename;
    }

    public void setFilename(String filename) {
        this.filename = filename;
    }

    public String getUsername() {
        return this.username;
    }

    public FileDownloadRequest(String filename, String username, SocketAddress clientAddress) {
        this.filename = filename;
        this.username = username;
        this.clientAddress = clientAddress;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public FileDownloadRequest(String filename, String username) {
        this.filename = filename;
        this.username = username;
    }

}
