package fileserver;

import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.concurrent.ThreadLocalRandom;

public class Client {

    public Client(String serverAddress, int serverPort) {
        try {
            System.out.print("Enter name of the client: ");
            Scanner scanner = new Scanner(System.in);
            String clientName = scanner.nextLine();
            NetworkUtil networkUtil = new NetworkUtil(serverAddress, serverPort);
            networkUtil.write(clientName);
            new ReadThreadClient(networkUtil);
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    private ArrayList<Message> inbox;
    public static void main(String args[]) {
        String serverAddress = "127.0.0.1";
        int serverPort = 33333;
        String clientName = "";

        //System.out.println("Enter menu option: ");
        Scanner input = new Scanner(System.in);
        int opt = 0;

        try {

            NetworkUtil networkUtil = new NetworkUtil(serverAddress, serverPort);
            //WriteThreadClient write = new WriteThreadClient(networkUtil);
            System.out.println("Enter your name: ");
            clientName = input.nextLine();
            networkUtil.write(clientName);
            ReadThreadClient rtc = new ReadThreadClient(networkUtil);

            while (true) {
                System.out.println("Enter option: ");
                System.out.println("1) Send message to user");
                System.out.println("2) View user list");
                System.out.println("3) Upload file");
                System.out.println("4) My files");
                System.out.println("5) Files of other users");
                System.out.println("6) Read unread/old messages");
                System.out.println("7) Make file request");
                System.out.println("8) Upload requested file");
                String optStr = input.nextLine();
                opt = Integer.parseInt(optStr);
                if (opt == 1) {
                    if (clientName.equalsIgnoreCase("")) System.out.println("No client name found!");
                    else {
                        // System.out.println("Enter receiver name: ");
                        // String receiver = input.nextLine();
                        // write.setName(clientName);
                        // write.startThread();
                        System.out.print("Enter name of the user to send: ");
                        String to = input.nextLine();
                        System.out.print("Enter the message: ");
                        String text = input.nextLine();
                        Message message = new Message();
                        message.setFrom(clientName);
                        message.setTo(to);
                        message.setText(text);
                        networkUtil.write(message);
                    }
                } else if (opt == 2) {
                    networkUtil.write(new UserListRequest(clientName));
                } else if (opt == 3) {
                    System.out.println("File upload");
                    System.out.println("1) Private");
                    System.out.println("2) Public");
                    String optStr2 = input.nextLine();
                    int opt2 = Integer.parseInt(optStr2);
                    boolean ifPublic = (opt2 == 2);
                    System.out.println("Enter filepath:");
                    Path filePath = Paths.get(input.nextLine());
                    //byte[] bytes = Files.readAllBytes(filePath);
                    FileInfo fi = new FileInfo(filePath.getFileName().toString(), ifPublic, Files.size(filePath), clientName);
                    fi.setClientPath(filePath.toString());
                    networkUtil.write(fi);
                } else if (opt == 4) {
                    networkUtil.write (new UserFileRequest(clientName, networkUtil.getSocket().getLocalSocketAddress()));
                    Thread.sleep(500);
                    System.out.println("\nEnter any filename to download. Enter / to return to main menu :");
                    while (true) {
                        String fileName = input.nextLine();
                        if (fileName.equals("/")) break;
                        else {
                            networkUtil.write (new FileDownloadRequest(fileName, clientName, networkUtil.getSocket().getLocalSocketAddress()));
                        }
                    }
                } else if (opt == 5) {
                    System.out.print("Enter name of the user to check: ");
                    String targetName = input.nextLine();
                    networkUtil.write (new UserFileRequest(targetName, networkUtil.getSocket().getLocalSocketAddress()));
                    Thread.sleep(500);
                    System.out.println("\nEnter any filename to download. Enter / to return to main menu :");
                    while (true) {
                        String fileName = input.nextLine();
                        if (fileName.equals("/")) break;
                        else {
                            networkUtil.write (new FileDownloadRequest(fileName, targetName, networkUtil.getSocket().getLocalSocketAddress()));
                        }
                    }
                } else if (opt == 6) {
                    System.out.println("Unread/Old Messages");
                    System.out.println("===================");
                    rtc.getInbox().forEach(obj -> {
                        System.out.println("\n" + obj.getFrom() + " has sent you a message: " + obj.getText() +"\n");
                    });
                } else if (opt == 7) {
                    System.out.println("Enter file request description: ");
                    String desc = input.nextLine();
                    long id = ThreadLocalRandom.current().nextLong(1024, Long.MAX_VALUE);
                    networkUtil.write (new FileUploadRequest(clientName, desc, id));
                } else if (opt == 8) {
                    System.out.println("Enter file request ID: ");
                    long id = Long.parseLong(input.nextLine());
                    System.out.println("Enter filepath:");
                    Path filePath = Paths.get(input.nextLine());
                    //byte[] bytes = Files.readAllBytes(filePath);
                    FileInfo fi = new FileInfo(filePath.getFileName().toString(), true, Files.size(filePath), clientName);
                    fi.setClientPath(filePath.toString());
                    fi.setReqID(id);
                    networkUtil.write(fi);
                }
            }
        } catch (Exception e) {
            System.out.println(e);
        }

    }
}


