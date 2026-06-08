#include <iostream>
#include <memory>
#include <string>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

using namespace std;

sql::mysql::MySQL_Driver* driver = nullptr;
unique_ptr<sql::Connection> conn;

bool connectToDatabase() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();

        conn.reset(driver->connect(
            "tcp://127.0.0.1:3306",
            "root",
            "your_real_password_here"
        ));

        conn->setSchema("food_ordering_db");

        cout << "Connected to MySQL.\n";
        return true;
    }
    catch (sql::SQLException& e) {
        cerr << "Database connection failed: " << e.what() << '\n';
        return false;
    }
}

void registerUser() {
    string username, password;

    cout << "\n=== Register New User ===\n";
    cout << "Enter username: ";
    getline(cin, username);

    cout << "Enter password: ";
    getline(cin, password);

    if (username.empty() || password.empty()) {
        cout << "Username and password cannot be empty.\n";
        return;
    }

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "INSERT INTO users (username, password) VALUES (?, ?)"
            )
        );

        pstmt->setString(1, username);
        pstmt->setString(2, password);
        pstmt->executeUpdate();

        cout << "User registered successfully.\n";
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << '\n';
    }
}

void loginUser() {
    string username, password;

    cout << "\n=== User Login ===\n";
    cout << "Enter username: ";
    getline(cin, username);

    cout << "Enter password: ";
    getline(cin, password);

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement(
                "SELECT id FROM users WHERE username = ? AND password = ?"
            )
        );

        pstmt->setString(1, username);
        pstmt->setString(2, password);

        unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next()) {
            int userId = res->getInt("id");
            cout << "Login successful. Welcome, " << username
                << "! User ID: " << userId << '\n';
        }
        else {
            cout << "Invalid username or password.\n";
        }
    }
    catch (sql::SQLException& e) {
        cerr << "SQL Error: " << e.what() << '\n';
    }
}

void showMainMenu() {
    cout << "\n=== Main Menu ===\n";
    cout << "1. Register User\n";
    cout << "2. Login User\n";
    cout << "3. Exit\n";
    cout << "Choose an option: ";
}

int main() {
    if (!connectToDatabase()) {
        return 1;
    }

    while (true) {
        showMainMenu();

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            registerUser();
        }
        else if (choice == "2") {
            loginUser();
        }
        else if (choice == "3") {
            cout << "Goodbye!\n";
            break;
        }
        else {
            cout << "Invalid option. Try again.\n";
        }
    }

    return 0;
}