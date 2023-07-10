package fileserver;

import java.io.IOError;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;


public class UserList implements Serializable {
    public ArrayList<String> list = new ArrayList<String>();

    public UserList (HashMap<String, User> clientMap, NetworkUtil networkUtil) {
        System.out.println("creating userlst");
        clientMap.forEach((key, user) -> {
            System.out.println(key);
            if (user.isOnline()) list.add(key);
            else list.add(key + " (OFFLINE)");
        });
    } 
}
