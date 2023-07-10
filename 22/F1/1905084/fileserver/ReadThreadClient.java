package fileserver;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.nio.file.StandardOpenOption;
import java.util.ArrayList;

public class ReadThreadClient implements Runnable {
    private Thread thr;
    private NetworkUtil networkUtil;
    private ArrayList<Message> inbox;
    // private String upFilePath;

    public ReadThreadClient(NetworkUtil networkUtil) {
        // this.upFilePath = upFilePath;
        this.inbox = new ArrayList<Message>();
        this.networkUtil = networkUtil;
        this.thr = new Thread(this);
        thr.start();
    }

    public ArrayList<Message> getInbox() {
        return this.inbox;
    }

    public void run() {
        try {
            while (true) {
                Object o = networkUtil.read();
                if (o instanceof Message) {
                    Message obj = (Message) o;
                    System.out.println("\n" + obj.getFrom() + " has sent you a message: " + obj.getText() +"\n");
                } else if (o instanceof InvalidUsernameWarning) {
                    System.out.println("\nThis user is already logged in!");
                } else if (o instanceof UserList) {
                    System.out.println("\nReceived user list from server: ");
                    ArrayList<String> list = ((UserList) o).list;
                    list.forEach((u) -> System.out.println(u));
                } else if (o instanceof FileList) {
                    FileList list = (FileList) o;
                    System.out.println("\nReceived file list of " + list.getUsername() + " from server ");
                    System.out.println("=========================================");
                    System.out.println("\nPrivate files:");
                    System.out.println("——————————————");
                    list.getPrivateList().forEach((f) -> System.out.println(f));
                    System.out.println("\n——————————————");
                    System.out.println("Public files:");
                    System.out.println("——————————————");
                    list.getPublicList().forEach((f) -> System.out.println(f));
                } else if (o instanceof FileInfo) {
                    FileInfo fi = (FileInfo) o;
                    System.out.println("Server has assigned fileID " + fi.getFileID() +
                                        " and chunkSize " + fi.getChunkSize() + " for " + fi.getClientPath());
                    long cs = fi.getChunkSize(), fs = fi.getFileSize();
                    long nChunk = (fs + cs - 1) / cs;
                    System.out.println("Creating " + nChunk + " chunks");  
                    byte[] bytes = Files.readAllBytes(Paths.get(fi.getClientPath()));
                    for (int i=0; i<nChunk; i++) {
                        byte[] cBytes;
                        if ((i+1)*cs > fs) {
                            cBytes = new byte[(int)(fs%cs)];
                            System.arraycopy(bytes, (int)(i*cs), cBytes, 0, (int)(fs%cs));
                        } else {
                            cBytes = new byte[(int)cs];
                            System.arraycopy(bytes, (int)(i*cs), cBytes, 0, (int)cs);
                        }
                        // Path chPath = Paths.get(".chunk_" + i + "_" + fi.getFileID());
                        // Files.write (chPath, cBytes);
                        FileChunk fc = new FileChunk(fi.getFileID(), cBytes, (int)cs, i);
                        networkUtil.write(fc);
                        
                        ChunkAck ack = (ChunkAck)(networkUtil.read());
                        if (ack.getFileID() == fc.getFileID() && ack.getChunkOrder() == fc.getChunkOrder()) {
                            System.out.println("Valid Acknoweldgement received for chunk " + i);
                            if (ack.getChunkOrder() == nChunk - 1) {
                                System.out.println("Last chunk of " + fi.getName() + " sent, sending completion ack");
                                networkUtil.write(new UploadCompletionAck(ack.getFileID()));
                            }
                        } else {
                            System.out.println("Acknowledgement mismatch, aborting upload");
                            break;
                        }
                        //System.out.println(networkUtil.read());
                    }
                } else if (o instanceof UploadSuccessAck) {
                    System.out.println("Server confirmed successful upload for " + ((UploadSuccessAck) o).getFileID());
                } else if (o instanceof UploadFailureAck) {
                    System.out.println("Upload failed: Server reported size mismatch for " + ((UploadSuccessAck) o).getFileID());
                } else if (o instanceof FileChunk) {
                    FileChunk fc = (FileChunk) o;
                    (new File("downloads")).mkdirs();
                    Path filePath = Paths.get("downloads/" + fc.getFileID() + "_" + fc.getFileName());
                    if (fc.getChunkOrder()==0) Files.write (filePath, fc.getData());
                    else Files.write (filePath, fc.getData(), StandardOpenOption.APPEND);
                    System.out.println("Received chunk " + fc.getChunkOrder() + " of " + fc.getFileID() + " (" + fc.getFileName() + ")");
                } else if (o instanceof DownloadCompletionAck) {
                    System.out.println("Server finished sending " + ((DownloadCompletionAck) o).getFileID());
                } else if (o instanceof ArrayList<?>) {
                    ArrayList<?> list = (ArrayList<?>) o;
                    list.forEach(m -> inbox.add((Message)m));
                }
            }
        } catch (Exception e) {
            System.out.println(e);
        } finally {
            try {
                networkUtil.closeConnection();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
}



