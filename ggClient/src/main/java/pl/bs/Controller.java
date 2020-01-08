package pl.bs;

import javafx.event.ActionEvent;

import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;

public class Controller {
    public void logIn(ActionEvent actionEvent) {

        String msg = "some text";
        try {
            Socket clientSocket = new Socket("192.168.65.130", 1234,true);
            OutputStream os = clientSocket.getOutputStream();
            os.write(msg.getBytes());
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("hi");
    }
}
