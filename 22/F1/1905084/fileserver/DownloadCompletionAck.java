package fileserver;

import java.io.Serializable;
import java.util.Objects;

public class DownloadCompletionAck implements Serializable {
    long fileID;

    public DownloadCompletionAck(long fileID) {
        this.fileID = fileID;
    }
    

    public long getFileID() {
        return this.fileID;
    }

    public void setFileID(long fileID) {
        this.fileID = fileID;
    }
}
