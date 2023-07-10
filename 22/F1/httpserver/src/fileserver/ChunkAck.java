package fileserver;

import java.io.Serializable;

public class ChunkAck implements Serializable {
    long fileID;
    int chunkOrder;

    public long getFileID() {
        return this.fileID;
    }

    public void setFileID(long fileID) {
        this.fileID = fileID;
    }

    public ChunkAck(long fileID, int chunkOrder) {
        this.fileID = fileID;
        this.chunkOrder = chunkOrder;
    }

    public int getChunkOrder() {
        return this.chunkOrder;
    }

    public void setChunkOrder(int chunkOrder) {
        this.chunkOrder = chunkOrder;
    }
}
