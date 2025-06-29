

#include <iostream>
#include <memory>
#include <string>

// MySQL Connector headers
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

using namespace std;

sql::mysql::MySQL_Driver* driver;
unique_ptr<sql::Connection> conn;

void connectToDatabase() {
    driver = sql::mysql::get_mysql_driver_instance();
    conn.reset(driver->connect("tcp://127.0.0.1:3306", "root", "your_password_here"));
    conn->setSchema("food_ordering_db");
}

void registerUser() {
    string username, password;

    cout << "=== Register New User ===" << endl;
    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    try {
        unique_ptr<sql::PreparedStatement> pstmt(
            conn->prepareStatement("INSERT INTO users (username, password) VALUES (?, ?)")
        );
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        pstmt->executeUpdate();

        cout << "✅ User registered successfully." << endl;
    }
    catch (sql::SQLException& e) {
        cerr << "❌ SQL Error: " << e.what() << endl;
    }
}
void loginUser() {
    string username, password;

    cout << "=== User Login ===" << endl;
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
            cout << "✅ Login successful. Welcome, " << username << "!" << endl;
        }
        else {
            cout << "❌ Invalid username or password." << endl;
        }
    }
    catch (sql::SQLException& e) {
        cerr << "❌ SQL Error: " << e.what() << endl;
    }
}
int main() {
    connectToDatabase();
    cout << "Connected to MySQL." << endl;

    while (true) {
        cout << "\n=== Main Menu ===" << endl;
        cout << "1. Register User" << endl;
        cout << "2. Login User" << endl;
        cout << "3. Exit" << endl;
        cout << "Choose an option: ";

        string choice;
        getline(cin, choice);

        if (choice == "1") {
            registerUser();
        }
        else if (choice == "2") {
            loginUser();
        }
        else if (choice == "3") {
            cout << "Goodbye!" << endl;
            break;
        }
        else {
            cout << "Invalid option. Try again." << endl;
        }
    }

    return 0;
}
