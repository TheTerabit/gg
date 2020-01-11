package pl.bs;

import javafx.collections.ObservableList;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;

public class ConversationRefresh implements Runnable {

    private TextArea textArea;
    private ObservableList<User> users;
    private User friend;

    public ConversationRefresh(TextArea textArea, ObservableList<User> users, User friend) {
        this.textArea = textArea;
        this.users = users;
        this.friend = friend;
    }

    @Override
    public void run() {
        boolean online=false;
        while(true){
            User u = null;
            for(User i: users){
                if(i.getId()==friend.getId()){
                    u = i;
                }
            }
            if(u!=null){
                users.remove(u);
            }
            u.setNewMessage("");
            users.add(u);

            if(u.getStatus().equals("")) {
                if (online)
                    textArea.setText(u.getMessages() + u.getUsername() + " is offline.\n");
                online = false;
            }
            else {
                textArea.setText(u.getMessages());
                online = true;
            }
            textArea.setScrollTop(Double.MAX_VALUE);
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
    }
}
