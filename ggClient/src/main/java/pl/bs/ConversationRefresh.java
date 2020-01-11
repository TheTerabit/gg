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
        while(true){
            User u = null;
            for(User i: users){
                if(i.getId()==friend.getId()){
                    u = i;
                }
            }
            textArea.setText(u.getMessages());
            textArea.setScrollTop(Double.MAX_VALUE);
            try {
                Thread.sleep(500);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }

        }
    }
}
