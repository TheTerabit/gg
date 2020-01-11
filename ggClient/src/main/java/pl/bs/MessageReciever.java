package pl.bs;

import javafx.collections.ObservableList;
import javafx.scene.control.TableView;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.List;

public class MessageReciever implements Runnable {

    private BufferedReader reader;
    private ObservableList<User> users;
    private TableView tbl_users;

    public MessageReciever(BufferedReader reader, ObservableList<User> users, TableView tbl_users) {
        this.reader = reader;
        this.users = users;
        this.tbl_users = tbl_users;
    }

    @Override
    public void run() {
        while(true){
            String serverMessage = null;
            try {
                serverMessage = reader.readLine();
            } catch (IOException e) {
                e.printStackTrace();
            }
            String s = new String(serverMessage);
            System.out.println("#"+s+"#");
            if(s.trim().startsWith("2")){
                addMessage(s);
            }
            else if(s.length()==0){
                System.out.println("skipped");
                continue;
            }
            else{
                User user = processMessage(s);
                updateUsers(user);
                refreshTable();
            }

        }
    }

    private void addMessage(String message){
        String id = message.split("&")[1];
        String text = message.split("&")[2];
        User u = null;
        for(User i: users){
            if(i.getId() == Integer.parseInt(id)){
                u=i;
            }
        }
        if(u!=null)
            users.remove(u);

        u.saveMessage(u.getUsername()+": "+text+"\n");

        users.add(u);
    }

    private User processMessage(String message){
        System.out.println(message.length());
        message = message.substring(1,message.length());
        String id = message.split("&")[1];
        String username = message.split("&")[2].split("%")[0];
        String status = message.split("&")[2].split("%")[1];
        Boolean b = null;
        if (status.equals("1"))
            b = true;
        else
            b = false;
        User user = new User(Integer.parseInt(id),username,b);
        return user;
    }

    private void updateUsers(User user){

        User u = null;
        for(User i: users){
            if(i.getId() == user.getId()){
                u=i;
            }
        }
        if(u!=null)
            users.remove(u);
        user.setStringBuffer(u.getStringBuffer());
        users.add(user);
    }

    private void refreshTable(){
        tbl_users.refresh();
        System.out.println("refreshed");
    }
}
