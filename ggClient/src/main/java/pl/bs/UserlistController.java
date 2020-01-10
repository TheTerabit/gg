package pl.bs;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.Stage;

import java.io.*;
import java.net.Socket;
import java.net.URL;
import java.util.ResourceBundle;

public class UserlistController implements Initializable {

    @FXML
    TableView<User> tbl_users;

    TableColumn col_username = new TableColumn("username");

    TableColumn col_status = new TableColumn("status");

    private PrintWriter writer;
    private BufferedReader reader;
    private ObservableList<User> users = FXCollections.observableArrayList();

    public void initData(PrintWriter writer, BufferedReader reader, ObservableList<User> users) {
        this.writer = writer;
        this.reader = reader;
        this.users = users;
        tbl_users.setItems(users);

    }

    public void openChat(ActionEvent actionEvent) {

        User user = tbl_users.getSelectionModel().getSelectedItem();
        System.out.println(user.getUsername());
        System.out.println(user.getId());

        Stage stage = new Stage();
        Parent root = null;
        try {
            root = FXMLLoader.load(getClass().getClassLoader().getResource("Chat.fxml"));
        } catch (IOException e) {
            e.printStackTrace();
        }
        stage.setTitle(user.getUsername());
        stage.setScene(new Scene(root));
        stage.setResizable(false);
        stage.show();

    }

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        createTable();
    }
    private void createTable(){
        col_username.setPrefWidth(220);
        tbl_users.getColumns().addAll(col_username, col_status);
        PropertyValueFactory propertyValueFactory = new PropertyValueFactory<User, String>("username");
        col_username.setCellValueFactory(propertyValueFactory);
        col_status.setCellValueFactory(new PropertyValueFactory<User, String>("status"));

    }
}
