package pl.bs;

import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;

public class User {
    private SimpleStringProperty username;
    private int id;
    private SimpleStringProperty status;

    public User(int id, String username, Boolean status) {
        this.username = new SimpleStringProperty(username);
        this.id = id;
        setStatus(status);
    }

    public String getUsername() {
        return username.get();
    }

    public void setUsername(String username) {
        this.username = new SimpleStringProperty(username);
    }

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
    }

    public String getStatus() {
        return status.get();
    }

    public void setStatus(boolean status) {
        if(status)
            this.status = new SimpleStringProperty("online");
        else
            this.status = new SimpleStringProperty("");
    }
}
