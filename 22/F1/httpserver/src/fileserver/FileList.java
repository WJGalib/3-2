package fileserver;

import java.io.Serializable;
import java.util.ArrayList;

public class FileList implements Serializable {
    ArrayList<String> publicList;
    ArrayList<String> privateList;
    String username;

    public ArrayList<String> getPublicList() {
        return this.publicList;
    }

    public ArrayList<String> getPrivateList() {
        return this.privateList;
    }

    public String getUsername() {
        return this.username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public FileList (String username) {
        this.username = username;
        publicList = new ArrayList<String>();
        privateList = new ArrayList<String>();
    }

    public void addPublic (String file) {
        this.publicList.add(file);
    }

    public void addPrivate (String file) {
        this.privateList.add(file);
    }
}
