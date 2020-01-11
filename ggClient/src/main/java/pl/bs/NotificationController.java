package pl.bs;

import javafx.fxml.FXML;
import javafx.scene.control.Label;

public class NotificationController {

    @FXML
    Label lbl_notification;

    public void initData(String username, String text) {
        lbl_notification.setText(username+": "+text);
    }
}
