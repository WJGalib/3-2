package fileserver;

import java.io.Serializable;

public class UploadSuccessAck implements Serializable {
    long fileID;

    public long getFileID() {
        return this.fileID;
    }

    public void setFileID(long fileID) {
        this.fileID = fileID;
    }

    public UploadSuccessAck(long fileID) {
        this.fileID = fileID;
    }
}
