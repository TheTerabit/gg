package pl.bs;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.stage.Stage;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;

public class ChatController {

    private PrintWriter writer;
    private BufferedReader reader;
    private ObservableList<User> users;
    private User friend;
    private Stage stage;

    @FXML
    TextArea txt_conversation;

    @FXML
    TextField txt_message;

    public void send(ActionEvent actionEvent) {
        User u = null;
        for(User i: users){
            if(i.getId()==friend.getId()){
                u = i;
            }
        }
        if(u!=null){
            if(u.getStatus().equals("")){
                stage.close();
                return;
            }
        }

        String message = txt_message.getText();
        txt_message.clear();
        String header = "2&"+(message.length()+1+String.valueOf(friend.getId()).length());
        header = fillMessage(header);
        writer.println(header);
        writer.println(friend.getId() + "&" + message);

        u = null;
        for(User i: users){
            if(i.getId()==friend.getId()){
                u = i;
            }
        }
        if(u!=null){
            users.remove(u);
        }
        u.saveMessage("ja: " + message + "\n");
        users.add(u);

        //txt_conversation.setText(u.getMessages());
    }

    private String fillMessage(String message){
        StringBuilder stringBuilder = new StringBuilder(message);
        while(stringBuilder.length()<48)
            stringBuilder.append(" ");
        return stringBuilder.toString();
    }

    public void initData(PrintWriter writer, BufferedReader reader, ObservableList<User> users, User friend, Stage stage) {
        this.reader = reader;
        this.writer = writer;
        this.users = users;
        this.friend = friend;
        this.stage = stage;
        Thread t1 = new Thread(new ConversationRefresh(txt_conversation, users, friend));
        //MessageReciever messageReciever = new MessageReciever(reader, users);
        t1.setDaemon(true);
        t1.start();
    }
}
