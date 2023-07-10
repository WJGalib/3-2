package fileserver;

import java.io.Serializable;

public class FileChunk implements Serializable {
    long fileID;
    byte[] data;
    int chunkSize;
    int chunkOrder;
    String fileName;

    public String getFileName() {
        return this.fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    public long getFileID() {
        return this.fileID;
    }

    public void setFileID(long fileID) {
        this.fileID = fileID;
    }

    public byte[] getData() {
        return this.data;
    }

    public FileChunk(long fileID, byte[] data, int chunkSize, int chunkOrder) {
        this.fileID = fileID;
        this.data = data;
        this.chunkSize = chunkSize;
        this.chunkOrder = chunkOrder;
    }

    public void setData(byte[] data) {
        this.data = data;
    }

    public int getChunkSize() {
        return this.chunkSize;
    }

    public void setChunkSize(int chunkSize) {
        this.chunkSize = chunkSize;
    }

    public int getChunkOrder() {
        return this.chunkOrder;
    }

    public void setChunkOrder(int chunkOrder) {
        this.chunkOrder = chunkOrder;
    }
}
