package fileserver;

import java.io.Serializable;

public class UserListRequest implements Serializable {
    private String from;

    public UserListRequest (String from) {
        this.from = from;
    }

    public String getFrom() {
        return from;
    }

    public void setFrom(String from) {
        this.from = from;
    }
}