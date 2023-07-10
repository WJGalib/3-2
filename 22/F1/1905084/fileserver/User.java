package fileserver;

import java.util.ArrayList;

public class User {
    NetworkUtil networkUtil;
    boolean ifOnline;
    ArrayList<FileInfo> files;
    ArrayList<Message> inbox;

    public User (NetworkUtil networkUtil) {
        this.inbox = new ArrayList<Message>();
        this.networkUtil = networkUtil;
        this.setOnline(true);
    }

    public NetworkUtil getNetworkUtil() {
        return networkUtil;
    }

    public void setNetworkUtil (NetworkUtil networkUtil) {
        this.networkUtil = networkUtil;
    }

    public boolean isOnline() {
        return ifOnline;
    }

    public void setOnline (boolean online) {
        this.ifOnline = online;
    }

    public void addFile (FileInfo file) {
        this.files.add(file);
    }

    public void addUnreadMessage (Message m) {
        this.inbox.add(m);
    }

    public ArrayList<Message> getInbox() {
        return this.inbox;
    }

    
}
