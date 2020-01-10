package pl.bs;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.Stage;

import java.io.*;
import java.net.Socket;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;

public class Controller implements Initializable {
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

    @FXML
    TableView<User> tbl_users;


    TableColumn col_username = new TableColumn("username");

    TableColumn col_status = new TableColumn("status");

    Socket clientSocket;
    OutputStream os;
    InputStream is;
    PrintWriter writer;
    BufferedReader reader;
    //private List<User> users = new ArrayList<>();
    private ObservableList<User> users = FXCollections.observableArrayList();

    public void logIn(ActionEvent actionEvent) {

        String msg1 = "1&" + Integer.toString(txt_username1.getLength() + txt_password1.getLength() + 1);
        msg1 = fillMessage(msg1);
        String msg2 = txt_username1.getText() + "&" + txt_password1.getText()+"\n";
        try {
            clientSocket = new Socket("127.0.0.1", 1234);
            os = clientSocket.getOutputStream();
            is = clientSocket.getInputStream();
            writer = new PrintWriter(os, true);
            writer.println(msg1);
            writer.println(msg2);
            writer.print("");
            System.out.println("wyslane");
            reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
            String serverMessage = "";
            System.out.println("przed whilem");

            serverMessage = reader.readLine();

            String s = new String(serverMessage);
            System.out.println(s);

            if(s.endsWith("1")){
                serverMessage = reader.readLine();

                s = new String(serverMessage);

                System.out.println(s);
                serverMessage = reader.readLine();

                s = new String(serverMessage);
                getUsers(s);
                System.out.println(s);
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("done");
        openUserlist(actionEvent);
    }

    public void register(ActionEvent actionEvent) {

        if (txt_password2.getText().equals(txt_password3.getText())) {
            String msg1 = "0&" + Integer.toString(txt_username2.getLength() + txt_password2.getLength() + 1);
            msg1 = fillMessage(msg1);
            String msg2 = txt_username2.getText() + "&" + txt_password2.getText()+"\n";
            try {
                clientSocket = new Socket("127.0.0.1", 1234);
                os = clientSocket.getOutputStream();
                is = clientSocket.getInputStream();
                writer = new PrintWriter(os, true);
                writer.println(msg1);
                writer.println(msg2);
                writer.print("");
                System.out.println("wyslane");
                reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
                String serverMessage = "";
                System.out.println("przed whilem");

                serverMessage = reader.readLine();

                String s = new String(serverMessage);
                System.out.println(s);

                if(s.endsWith("1")){
                    serverMessage = reader.readLine();

                    s = new String(serverMessage);

                    System.out.println(s);
                    serverMessage = reader.readLine();

                    s = new String(serverMessage);
                    getUsers(s);
                    System.out.println(s);
                }


            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("done");
            openUserlist(actionEvent);
        } else
            System.out.println("Password mismatch");
    }
    private String fillMessage(String message){
        StringBuilder stringBuilder = new StringBuilder(message);
        while(stringBuilder.length()<49)
            stringBuilder.append(" ");
        return stringBuilder.toString();
    }

    private void getUsers(String s){
        String[] rows = s.split("&");
        for(int i=1;i< rows.length;i++){
            String[] attributes = rows[i].split("%");
            User user = new User(Integer.parseInt(attributes[0]),attributes[1],Boolean.parseBoolean(attributes[2]));
            users.add(user);
        }
    }
    private void openUserlist1(ActionEvent event){

        Parent userlist = null;
        try {
            userlist = FXMLLoader.load(getClass().getClassLoader().getResource("Userlist.fxml"));
        } catch (IOException e) {
            e.printStackTrace();
        }
        Scene userlistScene = new Scene(userlist);
        Stage window = (Stage) ((Node)event.getSource()).getScene().getWindow();
        window.setScene(userlistScene);
        window.setHeight(512);
        window.setWidth(306);
        //createTable();
        window.show();
        System.out.println(users.size());

    }
    private void openUserlist(ActionEvent event){

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
        window.setScene(userlistScene);
        window.setHeight(512);
        window.setWidth(306);
        window.show();
        System.out.println(users.size());

    }
    public void openChat(ActionEvent actionEvent) {

    }

    @Override
    public void initialize(URL location, ResourceBundle resources) {

    }
}
