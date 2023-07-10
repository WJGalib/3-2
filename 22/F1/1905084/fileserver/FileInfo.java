package fileserver;

import java.io.Serializable;

public class FileInfo implements Serializable {
    
    // byte[] bytes;
    String name;
    String ownerName;
    String clientPath;

    long fileID;
    long fileSize;
    long chunkSize;
    boolean ifPublic;
    long reqID = -1;
    public String requester;

    public long getReqID() {
        return this.reqID;
    }

    public void setReqID(long reqID) {
        this.reqID = reqID;
    }
    
    public String getOwnerName() {
        return this.ownerName;
    }

    public void setOwnerName(String ownerName) {
        this.ownerName = ownerName;
    }

    public boolean getIfPublic() {
        return this.ifPublic;
    }

    public void setIfPublic(boolean ifPublic) {
        this.ifPublic = ifPublic;
    }

    public FileInfo (String name) {
        this.name = name;
    }

    public FileInfo (String name, long fileSize) {
        this.name = name;
        this.fileSize = fileSize;
        //this.bytes = bytes;
    }

    public FileInfo (String name, boolean ifPublic, long fileSize) {
        this.name = name;
        this.fileSize = fileSize;
        //this.bytes = bytes;
        this.ifPublic = ifPublic;
    }

    public FileInfo (String name, boolean ifPublic, long fileSize, String ownerName) {
        this.name = name;
        //this.bytes = bytes;
        this.fileSize = fileSize;
        this.ifPublic = ifPublic;
        this.ownerName = ownerName;
    }

    // public byte[] getBytes() {
    //     return this.bytes;
    // }

    // public void setBytes(byte[] bytes) {
    //     this.bytes = bytes;
    // }

    public String getName() {
        return this.name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public long getFileID() {
        return this.fileID;
    }

    public void setFileID(long fileID) {
        this.fileID = fileID;
    }

    public long getFileSize() {
        return this.fileSize;
    }

    public void setFileSize(long fileSize) {
        this.fileSize = fileSize;
    }

    public long getChunkSize() {
        return this.chunkSize;
    }

    public void setChunkSize(long chunkSize) {
        this.chunkSize = chunkSize;
    }

    public String getClientPath() {
        return this.clientPath;
    }

    public void setClientPath(String clientPath) {
        this.clientPath = clientPath;
    }
    

}
