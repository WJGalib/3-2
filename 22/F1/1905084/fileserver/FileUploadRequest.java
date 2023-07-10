package fileserver;

import java.io.Serializable;

public class FileUploadRequest implements Serializable {
    private String from;
    private String description;
    private long reqID;


    public FileUploadRequest(String from, String description, long reqID) {
        this.from = from;
        this.description = description;
        this.reqID = reqID;
    }

    public String getFrom() {
        return this.from;
    }

    public void setFrom(String from) {
        this.from = from;
    }

    public String getDescription() {
        return this.description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public long getReqID() {
        return this.reqID;
    }

    public void setReqID(long reqID) {
        this.reqID = reqID;
    }

}
