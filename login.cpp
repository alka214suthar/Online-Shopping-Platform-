#include <iostream>
#include <mysql.h>
#include <string>
using namespace std;

int main() {
    MYSQL* conn;
    MYSQL_ROW row;
    MYSQL_RES* res;

    // Initialize MySQL connection
    conn = mysql_init(0);
    if (!conn) {
        cout << "❌ mysql_init() failed!" << endl;
        return 1;
    }

    conn = mysql_real_connect(conn, "localhost", "root", "Alka@315", "OnlineShoppingDB", 3306, NULL, 0);

    if (!conn) {
        cout << "❌ Connection failed: " << mysql_error(conn) << endl;
        return 1;
    }
    cout << "✅ Connected to OnlineShoppingDB" << endl;

    // Input credentials
    string email, password;
    cout << "Enter email: ";
    cin >> email;
    cout << "Enter password: ";
    cin >> password;

    // Build query
    string query = "SELECT user_id, first_name, last_name FROM Users WHERE email='" + email + "' AND password_hash='" + password + "'";

    if (mysql_query(conn, query.c_str())) {
        cout << "❌ Query execution failed: " << mysql_error(conn) << endl;
        mysql_close(conn);
        return 1;
    }

    res = mysql_store_result(conn);
    if (res && mysql_num_rows(res) > 0) {
        row = mysql_fetch_row(res);
        cout << "✅ Login successful! Welcome, " << row[1] << " " << row[2] << "." << endl;
    } else {
        cout << "❌ Invalid email or password!" << endl;
    }

    mysql_free_result(res);
    mysql_close(conn);
    return 0;
}
