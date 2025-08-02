#include <iostream>
#include <mysql.h>
#include <string>
using namespace std;

MYSQL* conn;
MYSQL_ROW row;
MYSQL_RES* res;

int currentUserId = -1; // stores logged in user id

void showProducts() {
    if (mysql_query(conn, "SELECT product_id, name, price, stock FROM Products")) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return;
    }
    res = mysql_store_result(conn);
    cout << "\n--- Available Products ---\n";
    while ((row = mysql_fetch_row(res))) {
        cout << "ID: " << row[0] << " | Name: " << row[1] << " | Price: " << row[2] 
             << " | Stock: " << row[3] << endl;
    }
    mysql_free_result(res);
}

void addToCart() {
    int productId, qty;
    cout << "Enter Product ID to add: ";
    cin >> productId;
    cout << "Enter Quantity: ";
    cin >> qty;

    string query = "SELECT cart_id FROM Cart WHERE user_id=" + to_string(currentUserId);
    mysql_query(conn, query.c_str());
    res = mysql_store_result(conn);
    string cartId;
    if ((row = mysql_fetch_row(res)))
        cartId = row[0];
    else {
        mysql_query(conn, ("INSERT INTO Cart(user_id) VALUES(" + to_string(currentUserId) + ")").c_str());
        cartId = to_string(mysql_insert_id(conn));
    }
    mysql_free_result(res);

    query = "INSERT INTO Cart_Items(cart_id, product_id, quantity) VALUES(" 
            + cartId + "," + to_string(productId) + "," + to_string(qty) + ")";
    if (mysql_query(conn, query.c_str()))
        cerr << "Error: " << mysql_error(conn) << endl;
    else
        cout << "✅ Added to cart!\n";
}

void viewCart() {
    string query = "SELECT p.name, ci.quantity, (ci.quantity*p.price) "
                   "FROM Cart_Items ci "
                   "JOIN Cart c ON ci.cart_id=c.cart_id "
                   "JOIN Products p ON ci.product_id=p.product_id "
                   "WHERE c.user_id=" + to_string(currentUserId);

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return;
    }

    res = mysql_store_result(conn);
    cout << "\n--- My Cart ---\n";
    double total = 0;
    while ((row = mysql_fetch_row(res))) {
        double subtotal = stod(row[2]);
        cout << "Product: " << row[0] << " | Qty: " << row[1] 
             << " | Subtotal: " << subtotal << endl;
        total += subtotal;
    }
    cout << "Total: " << total << endl;
    mysql_free_result(res);

    // Wait for user before returning to menu
    cout << "\nPress Enter to return to the menu...";
    cin.ignore();  
    cin.get();
}


void checkout() {
    // calculate total
    string totalQuery = "SELECT SUM(ci.quantity*p.price) "
                        "FROM Cart_Items ci "
                        "JOIN Cart c ON ci.cart_id=c.cart_id "
                        "JOIN Products p ON ci.product_id=p.product_id "
                        "WHERE c.user_id=" + to_string(currentUserId);

    mysql_query(conn, totalQuery.c_str());
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    if (!row || !row[0]) {
        cout << "Cart is empty!\n";
        mysql_free_result(res);
        return;
    }
    double total = stod(row[0]);
    mysql_free_result(res);

    // create order
    string query = "INSERT INTO Orders(user_id,status,total) VALUES("
                   + to_string(currentUserId) + ",'Completed'," + to_string(total) + ")";
    mysql_query(conn, query.c_str());
    int orderId = mysql_insert_id(conn);

    // insert order items
    query = "INSERT INTO Order_Items(order_id, product_id, quantity, price) "
            "SELECT " + to_string(orderId) + ", ci.product_id, ci.quantity, p.price "
            "FROM Cart_Items ci "
            "JOIN Cart c ON ci.cart_id=c.cart_id "
            "JOIN Products p ON ci.product_id=p.product_id "
            "WHERE c.user_id=" + to_string(currentUserId);
    mysql_query(conn, query.c_str());

    // payment simulation
    query = "INSERT INTO Payments(order_id, amount, method, status) VALUES("
            + to_string(orderId) + "," + to_string(total) + ",'UPI','Success')";
    mysql_query(conn, query.c_str());

    // clear cart
    query = "DELETE ci FROM Cart_Items ci "
            "JOIN Cart c ON ci.cart_id=c.cart_id "
            "WHERE c.user_id=" + to_string(currentUserId);
    mysql_query(conn, query.c_str());

    cout << "✅ Order placed successfully! Total: " << total << endl;
}

void viewOrders() {
    string query = "SELECT order_id, order_date, total, status FROM Orders "
                   "WHERE user_id=" + to_string(currentUserId);

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return;
    }
    res = mysql_store_result(conn);
    cout << "\n--- My Orders ---\n";
    while ((row = mysql_fetch_row(res))) {
        cout << "Order ID: " << row[0] << " | Date: " << row[1] 
             << " | Total: " << row[2] << " | Status: " << row[3] << endl;
    }
    mysql_free_result(res);
}

void addReview() {
    int productId, rating;
    string comment;
    cout << "Enter Product ID to review: ";
    cin >> productId;
    cout << "Enter Rating (1-5): ";
    cin >> rating;
    cin.ignore();
    cout << "Enter Comment: ";
    getline(cin, comment);

    string query = "INSERT INTO Reviews(user_id, product_id, rating, comment) VALUES("
                   + to_string(currentUserId) + "," + to_string(productId) + ","
                   + to_string(rating) + ",'" + comment + "')";
    if (mysql_query(conn, query.c_str()))
        cerr << "Error: " << mysql_error(conn) << endl;
    else
        cout << "✅ Review added!\n";
}

void searchProducts() {
    string keyword;
    cin.ignore();
    cout << "Enter keyword to search: ";
    getline(cin, keyword);
    string query = "SELECT product_id, name, price FROM Products "
                   "WHERE name LIKE '%" + keyword + "%'";

    if (mysql_query(conn, query.c_str())) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return;
    }
    res = mysql_store_result(conn);
    cout << "\n--- Search Results ---\n";
    while ((row = mysql_fetch_row(res))) {
        cout << "ID: " << row[0] << " | " << row[1] << " | Price: " << row[2] << endl;
    }
    mysql_free_result(res);
}

void registerUser() {
    string fname, lname, email, pass, phone;
    cout << "Enter First Name: ";
    cin >> fname;
    cout << "Enter Last Name: ";
    cin >> lname;
    cout << "Enter Email: ";
    cin >> email;
    cout << "Enter Password: ";
    cin >> pass;
    cout << "Enter Phone: ";
    cin >> phone;

    string query = "INSERT INTO Users(role_id, first_name, last_name, email, password_hash, phone) "
                   "VALUES(2,'" + fname + "','" + lname + "','" + email + "','" + pass + "','" + phone + "')";
    if (mysql_query(conn, query.c_str()))
        cerr << "❌ Registration failed: " << mysql_error(conn) << endl;
    else
        cout << "✅ Registration successful!\n";
}

bool loginUser() {
    string email, pass;
    cout << "Enter Email: ";
    cin >> email;
    cout << "Enter Password: ";
    cin >> pass;

    string query = "SELECT user_id FROM Users WHERE email='" + email + "' AND password_hash='" + pass + "'";
    if (mysql_query(conn, query.c_str())) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return false;
    }
    res = mysql_store_result(conn);
    if ((row = mysql_fetch_row(res))) {
        currentUserId = stoi(row[0]);
        cout << "✅ Login successful!\n";
        mysql_free_result(res);
        return true;
    } else {
        cout << "❌ Invalid email or password.\n";
        mysql_free_result(res);
        return false;
    }
}

void logoutUser() {
    currentUserId = -1;
    cout << "👋 Logged out successfully!\n";
}

int main() {
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "Alka@315", "OnlineShoppingDB", 3306, NULL, 0);
    if (!conn) {
        cerr << "❌ Connection Failed: " << mysql_error(conn) << endl;
        return 1;
    }
    cout << "🚀 Connected to OnlineShoppingDB\n";

    int choice;
    
    while (true) {
        if (currentUserId == -1) {
            cout << "\n--- Welcome ---\n"
                 << "1. Register\n2. Login\n3. Exit\nChoose: ";
            cin >> choice;

            if (choice == 1) registerUser();
            else if (choice == 2) loginUser();
            
            else cout << "Invalid choice!\n";
        } else {
            cout << "\n--- Shopping Menu ---\n"
                 << "1. View Products\n2. Add to Cart\n3. View My Cart\n4. Checkout\n"
                 << "5. View My Orders\n6. Add Product Review\n7. Search Products\n8. Logout\nChoose: ";
            cin >> choice;

            switch (choice) {
                case 1: showProducts(); break;
                case 2: addToCart(); break;
                case 3: viewCart(); break;
                case 4: checkout(); break;
                case 5: viewOrders(); break;
                case 6: addReview(); break;
                case 7: searchProducts(); break;
                case 8: logoutUser(); break;
                default: cout << "Invalid option!\n";
            }
        }
    }

    mysql_close(conn);
    cout << "🏁 Program finished.\n";
    return 0;
}






