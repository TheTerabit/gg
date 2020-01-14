package pl.bs.loginWindow;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.TextField;
import javafx.stage.Stage;
import pl.bs.model.User;
import pl.bs.userlistWindow.UserlistController;

import java.io.*;
import java.net.Socket;

public class Controller {
    @FXML
    private TextField txt_username1;
    @FXML
    private TextField txt_password1;
    @FXML
    private TextField txt_username2;
    @FXML
    private TextField txt_password2;
    @FXML
    private TextField txt_password3;

    private Socket clientSocket;
    private OutputStream os;
    private InputStream is;
    private PrintWriter writer;
    private BufferedReader reader;
    private ObservableList<User> users = FXCollections.observableArrayList();

    public void logIn(ActionEvent actionEvent) {

        String msg1 = "1&" + Integer.toString(txt_username1.getLength() + txt_password1.getLength() + 1);
        msg1 = fillMessage(msg1);
        String msg2 = txt_username1.getText() + "&" + txt_password1.getText();
        try {
           String s = contactServer(msg1, msg2);

            if(s.endsWith("1")){
                String serverMessage = reader.readLine();
                s = new String(serverMessage);
                System.out.println(s);
                getUsers(s);
                openUserlist(actionEvent, txt_username1.getText());
            }
            else{
                Alert alert = new Alert(Alert.AlertType.INFORMATION);
                alert.setTitle("");
                alert.setHeaderText("Unsuccessful login atempt");
                alert.setContentText("Check your username and password.");
                alert.showAndWait();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("done");

    }

    public void register(ActionEvent actionEvent) {

        if (txt_password2.getText().equals(txt_password3.getText())) {
            String msg1 = "0&" + Integer.toString(txt_username2.getLength() + txt_password2.getLength() + 1);
            msg1 = fillMessage(msg1);
            String msg2 = txt_username2.getText() + "&" + txt_password2.getText();
            try {
                String s = contactServer(msg1, msg2);
                if(s.endsWith("1")){
                    if(s.startsWith("6")) {
                        String serverMessage = reader.readLine();
                        s = new String(serverMessage);
                        System.out.println(s);
                        getUsers(s);
                    }
                    else {
                        getUsers("");
                    }
                    openUserlist(actionEvent, txt_username2.getText());
                }
                else{
                    Alert alert = new Alert(Alert.AlertType.INFORMATION);
                    alert.setTitle("");
                    alert.setHeaderText("Registration unsuccessful");
                    alert.setContentText("Type another username");
                    alert.showAndWait();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("done");

        } else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("");
            alert.setHeaderText("Password mismatch");
            alert.setContentText("Type the password once again.");
            alert.showAndWait();
            System.out.println("Password mismatch");
        }
    }

    private String contactServer(String msg1, String msg2) throws IOException {
        clientSocket = new Socket("127.0.0.1", 1234);
        os = clientSocket.getOutputStream();
        is = clientSocket.getInputStream();
        writer = new PrintWriter(os, true);
        writer.println(msg1);
        writer.println(msg2);
        //writer.print("");
        System.out.println("wyslane");
        reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        String serverMessage = "";
        System.out.println("przed whilem");
        serverMessage = reader.readLine();
        String s = new String(serverMessage);
        System.out.println(s);
        return s;
    }

    private String fillMessage(String message){
        StringBuilder stringBuilder = new StringBuilder(message);
        while(stringBuilder.length()<48)
            stringBuilder.append(" ");
        return stringBuilder.toString();
    }

    private void getUsers(String s){
        String[] rows = s.split("&");
        for(int i=2;i< rows.length;i++){
            String[] attributes = rows[i].split("%");
            Boolean b=null;
            if (attributes[2].equals("1"))
                b = true;
            else
                b = false;
            User user = new User(Integer.parseInt(attributes[0]),attributes[1],b);
            users.add(user);
        }
        System.out.println("users fetched");
    }

    private void openUserlist(ActionEvent event, String username){
        Parent userlist = null;
        try {
            FXMLLoader loader = new FXMLLoader(
                    getClass().getClassLoader().getResource(
                            "Userlist.fxml"
                    )
            );
            userlist = loader.load();
            UserlistController userlistController = loader.<UserlistController>getController();
            userlistController.initData(writer, reader, users);
        } catch (IOException e) {
            e.printStackTrace();
        }
        Scene userlistScene = new Scene(userlist);
        Stage window = (Stage) ((Node)event.getSource()).getScene().getWindow();
        window.setTitle("GG - " + username);
        window.setScene(userlistScene);
        window.setHeight(512);
        window.setWidth(306);
        window.show();
        System.out.println(users.size());
    }

}
