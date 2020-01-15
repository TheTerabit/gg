package pl.bs.chatWindow;

import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TextArea;
import javafx.scene.control.TextField;
import javafx.stage.Stage;
import pl.bs.model.User;

import java.io.BufferedReader;
import java.io.PrintWriter;

public class ChatController {

    private PrintWriter writer;
    private BufferedReader reader;
    private ObservableList<User> users;
    private User friend;
    private Stage stage;

    @FXML
    private TextArea txt_conversation;
    @FXML
    private TextField txt_message;

    public void initData(PrintWriter writer, BufferedReader reader, ObservableList<User> users, User friend, Stage stage) {
        this.reader = reader;
        this.writer = writer;
        this.users = users;
        this.friend = friend;
        this.stage = stage;
        Thread t1 = new Thread(new ConversationRefresh(txt_conversation, users, friend));
        t1.setDaemon(true);
        t1.start();
    }

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

        for(int i = 0; i < users.size(); i++) {
            if (users.get(i).getId() == friend.getId()) {
                users.get(i).saveMessage("ja: " + message + "\n");
            }
        }
    }

    private String fillMessage(String message){
        StringBuilder stringBuilder = new StringBuilder(message);
        while(stringBuilder.length()<48)
            stringBuilder.append(" ");
        return stringBuilder.toString();
    }

}
