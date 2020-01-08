package pl.bs;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.TextField;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

public class Controller {
    @FXML
    TextField txt_username1;

    @FXML
    TextField txt_password1;
    @FXML
    TextField txt_username2;

    @FXML
    TextField txt_password2;


    @FXML
    TextField txt_password3;

    Socket clientSocket;
    OutputStream os;

    public void logIn(ActionEvent actionEvent) {

        String msg1 = "1&"+Integer.toString(txt_username1.getLength()+txt_password1.getLength()+1);
        String msg2 = txt_username1.getText()+"&" + txt_password1.getText();
        try {
            clientSocket = new Socket("127.0.0.1", 1234);
            os = clientSocket.getOutputStream();
            os.write(msg1.getBytes());
            os.write(msg2.getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("done");
    }
    public void register(ActionEvent actionEvent) {

        if(txt_password2.getText().equals(txt_password3.getText())){
            String msg1 = "0&"+Integer.toString(txt_username2.getLength()+txt_password2.getLength()+1) ;
            String msg2 = txt_username2.getText()+"&" + txt_password2.getText();
            try {
                clientSocket = new Socket("127.0.0.1", 1234);
                os = clientSocket.getOutputStream();
                os.write(msg1.getBytes());
                os.write(msg2.getBytes());
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("done");
        }
        else
            System.out.println("Password mismatch");
    }
}
