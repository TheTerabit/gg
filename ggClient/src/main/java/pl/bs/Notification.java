package pl.bs;

import javafx.scene.control.Alert;

public class Notification implements Runnable {

    private String username;
    private String message;

    public Notification(String username, String message) {
        this.username = username;
        this.message = message;
    }

    @Override
    public void run() {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);

        alert.setHeaderText(username+": ");
        alert.setContentText(message);
        alert.show();
        try {
            Thread.sleep(3000);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        alert.close();

    }
}
