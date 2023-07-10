package fileserver;

public class UploadFailureAck {
    long fileID;

    public long getFileID() {
        return this.fileID;
    }

    public void setFileID(long fileID) {
        this.fileID = fileID;
    }

    public UploadFailureAck(long fileID) {
        this.fileID = fileID;
    }
}
