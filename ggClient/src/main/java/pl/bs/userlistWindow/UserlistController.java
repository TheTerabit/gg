package pl.bs.userlistWindow;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.stage.Stage;
import pl.bs.model.User;
import pl.bs.chatWindow.ChatController;

import java.io.*;
import java.net.URL;
import java.util.ResourceBundle;

public class UserlistController implements Initializable {

    @FXML
    private TableView<User> tbl_users;

    private TableColumn col_username = new TableColumn("username");
    private TableColumn col_status = new TableColumn("status");
    private PrintWriter writer;
    private BufferedReader reader;
    private ObservableList<User> users = FXCollections.observableArrayList();

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

    public void initData(PrintWriter writer, BufferedReader reader, ObservableList<User> users) {
        this.writer = writer;
        this.reader = reader;
        this.users = users;
        tbl_users.setItems(users);
        Thread t1 = new Thread(new MessageReciever(reader, users, tbl_users));
        t1.setDaemon(true);
        t1.start();
    }

    @FXML
    private void openChat(ActionEvent event){
        User user = tbl_users.getSelectionModel().getSelectedItem();
        System.out.println(user.getUsername());
        System.out.println(user.getId());
        if(user.getStatus().equals("online")) {
            Stage stage = new Stage();
            Parent chat = null;
            try {
                FXMLLoader loader = new FXMLLoader(
                        getClass().getClassLoader().getResource(
                                "Chat.fxml"
                        )
                );
                chat = loader.load();
                ChatController chatController = loader.<ChatController>getController();
                chatController.initData(writer, reader, users, user, stage);
            } catch (IOException e) {
                e.printStackTrace();
            }
            stage.setTitle(user.getUsername());
            stage.setScene(new Scene(chat));
            stage.setResizable(false);
            stage.show();
        }
        else {
            Alert alert = new Alert(Alert.AlertType.INFORMATION);
            alert.setTitle("");
            alert.setHeaderText("Chosen user is offline");
            alert.setContentText("You can message active users only.");
            alert.showAndWait();
        }
    }

}
