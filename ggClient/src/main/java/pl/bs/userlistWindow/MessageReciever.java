package pl.bs.userlistWindow;

import javafx.collections.ObservableList;
import javafx.scene.control.TableView;
import pl.bs.model.User;

import java.io.BufferedReader;
import java.io.IOException;

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
            else if((s.length()==0)||(s.startsWith("6"))){
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

    private void addMessage(String message) {
        String id = message.split("&")[1];
        String text = null;
        try {
            text = message.split("&")[2];
        } catch (Exception e) {
            text = "";
        }

        //
        for(int i = 0; i < users.size(); i++) {
            if (users.get(i).getId() == Integer.parseInt(id)) {
                users.get(i).saveMessage(users.get(i).getUsername() + ": " + text + "\n");
            }
        }

        //Notifications.create().title(u.getUsername()+":").text(message).showConfirm();
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
        boolean addNew = true;
        for(int i = 0; i < users.size(); i++) {
            if (users.get(i).getId() == user.getId()) {
                users.get(i).setStringStatus(user.getStringStatus());
                addNew = false;
            }
        }
        if(addNew)
            users.add(user);

    }

    private void refreshTable(){
        tbl_users.refresh();
        System.out.println("refreshed");
    }
}
