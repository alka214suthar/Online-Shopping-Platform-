#include <iostream>
#include <mysql.h>
#include <string>
using namespace std;

int main() {
    MYSQL* conn;
    conn = mysql_init(0);

    if (!conn) {
        cout << "❌ mysql_init() failed!" << endl;
        return 1;
    }

    conn = mysql_real_connect(conn, "localhost", "root", "Alka@315", "OnlineShoppingDB", 3306, NULL, 0);

    if (!conn) {
        cout << "❌ Connection Failed. Error: " << mysql_error(conn) << endl;
        return 1;
    }

    cout << "✅ Connected to OnlineShoppingDB" << endl;

    string first_name, last_name, email, password, phone;
    int role_id = 2;  // default = Customer

    cout << "Enter first name: ";
    cin >> first_name;
    cout << "Enter last name: ";
    cin >> last_name;
    cout << "Enter email: ";
    cin >> email;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter phone (optional, press Enter to skip): ";
    cin.ignore();
    getline(cin, phone);

    string query = "INSERT INTO Users (role_id, first_name, last_name, email, password_hash, phone) "
                   "VALUES (" + to_string(role_id) + ", '" + first_name + "', '" + last_name + "', '" +
                   email + "', '" + password + "', '" + phone + "')";

    if (mysql_query(conn, query.c_str())) {
        cout << "❌ Registration failed: " << mysql_error(conn) << endl;
    } else {
        cout << "✅ User registered successfully!" << endl;
    }

    mysql_close(conn);
    return 0;
}

