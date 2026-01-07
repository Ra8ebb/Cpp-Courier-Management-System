#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

// ================= STRUCTS =================
struct User {
    int id;
    string name, phone, address, email, password;
    int isAdmin;
    vector<int> trackingHistory;
};

struct City {
    int id;
    string name;
    double rate;
};

struct Parcel {
    int trackingNum;
    int senderID;
    string receiverName;
    int cityID;
    double weight;
    double price;
    int status; // 0-Pending, 1-Shipped, 2-Delivered, 3-Cancelled
};

// ================= GLOBALS =================
vector<User> users;
vector<City> cities;
vector<Parcel> parcels;

// ================= FILE HANDLING =================
void saveUsers() {
    ofstream file("users.dat", ios::binary);
    int size = users.size();
    file.write((char*)&size, sizeof(size));
    for (auto& u : users) {
        file.write((char*)&u.id, sizeof(u.id));
        int len;
        
        len = u.name.size(); file.write((char*)&len, sizeof(len)); file.write(u.name.c_str(), len);
        len = u.phone.size(); file.write((char*)&len, sizeof(len)); file.write(u.phone.c_str(), len);
        len = u.address.size(); file.write((char*)&len, sizeof(len)); file.write(u.address.c_str(), len);
        len = u.email.size(); file.write((char*)&len, sizeof(len)); file.write(u.email.c_str(), len);
        len = u.password.size(); file.write((char*)&len, sizeof(len)); file.write(u.password.c_str(), len);
        
        file.write((char*)&u.isAdmin, sizeof(u.isAdmin));
        
        int thSize = u.trackingHistory.size();
        file.write((char*)&thSize, sizeof(thSize));
        for (int t : u.trackingHistory) file.write((char*)&t, sizeof(t));
    }
    file.close();
}

void loadUsers() {
    ifstream file("users.dat", ios::binary);
    if (!file) return;
    int size;
    file.read((char*)&size, sizeof(size));
    users.resize(size);
    for (auto& u : users) {
        int len;
        file.read((char*)&u.id, sizeof(u.id));
        
        file.read((char*)&len, sizeof(len)); u.name.resize(len); file.read(&u.name[0], len);
        file.read((char*)&len, sizeof(len)); u.phone.resize(len); file.read(&u.phone[0], len);
        file.read((char*)&len, sizeof(len)); u.address.resize(len); file.read(&u.address[0], len);
        file.read((char*)&len, sizeof(len)); u.email.resize(len); file.read(&u.email[0], len);
        file.read((char*)&len, sizeof(len)); u.password.resize(len); file.read(&u.password[0], len);
        
        file.read((char*)&u.isAdmin, sizeof(u.isAdmin));
        
        int thSize;
        file.read((char*)&thSize, sizeof(thSize));
        u.trackingHistory.resize(thSize);
        for (int i = 0; i < thSize; i++) file.read((char*)&u.trackingHistory[i], sizeof(int));
    }
    file.close();
}

void saveCities() {
    ofstream file("cities.dat", ios::binary);
    int size = cities.size();
    file.write((char*)&size, sizeof(size));
    for (auto& c : cities) {
        int len = c.name.size();
        file.write((char*)&c.id, sizeof(c.id));
        file.write((char*)&len, sizeof(len));
        file.write(c.name.c_str(), len);
        file.write((char*)&c.rate, sizeof(c.rate));
    }
    file.close();
}

void loadCities() {
    ifstream file("cities.dat", ios::binary);
    if (!file) return;
    int size;
    file.read((char*)&size, sizeof(size));
    cities.resize(size);
    for (auto& c : cities) {
        int len;
        file.read((char*)&c.id, sizeof(c.id));
        file.read((char*)&len, sizeof(len));
        c.name.resize(len);
        file.read(&c.name[0], len);
        file.read((char*)&c.rate, sizeof(c.rate));
    }
    file.close();
}

void saveParcels() {
    ofstream file("parcels.dat", ios::binary);
    int size = parcels.size();
    file.write((char*)&size, sizeof(size));
    for (auto& p : parcels) {
        file.write((char*)&p, sizeof(Parcel));
    }
    file.close();
}

void loadParcels() {
    ifstream file("parcels.dat", ios::binary);
    if (!file) return;
    int size;
    file.read((char*)&size, sizeof(size));
    parcels.resize(size);
    for (auto& p : parcels) {
        file.read((char*)&p, sizeof(Parcel));
    }
    file.close();
}

// ================= UTILS & AUTH =================
string statusText(int s) {
    if (s == 1) return "Shipped";
    if (s == 2) return "Delivered";
    if (s == 3) return "Cancelled";
    return "Pending";
}

int generateTrackingNum() {
    return 1000 + parcels.size();
}

int login() {
    string email, pass;
    cout << "Email: "; cin >> email;
    cout << "Password: "; cin >> pass;
    for (int i = 0; i < users.size(); i++) {
        if (users[i].email == email && users[i].password == pass) return i;
    }
    cout << "Account does not exist.\n";
    return -1;
}

void registerUser(int admin) {
    User u;
    u.id = users.size() + 1;
    cin.ignore();
    cout << "Name: "; getline(cin, u.name);
    cout << "Phone: "; cin >> u.phone;
    cout << "Address: "; cin >> u.address;
    cout << "Email: "; cin >> u.email;
    cout << "Password: "; cin >> u.password;
    u.isAdmin = admin;
    users.push_back(u);
    saveUsers();
    cout << "Account created successfully.\n";
}

// ================= ADMIN FEATURES =================
void addCity() {
    City c;
    c.id = cities.size() + 1;
    cin.ignore();
    cout << "City Name: "; getline(cin, c.name);
    cout << "Rate per KG: "; cin >> c.rate;
    cities.push_back(c);
    saveCities();
}

void viewAllShipments() {
    if (parcels.size() == 0) cout << "There are NO shipments\n";
    for (auto& p : parcels) {
        cout << "Tracking: " << p.trackingNum 
             << " | Status: " << statusText(p.status) 
             << " | Price: $" << p.price << endl;
    }
}

void updateParcelStatus() {
    int t, s;
    cout << "Tracking Number: "; cin >> t;
    cout << "New Status (1=Shipped, 2=Delivered): "; cin >> s;
    for (auto& p : parcels) {
        if (p.trackingNum == t) {
            p.status = s;
            saveParcels();
            cout << "Status updated.\n";
            return;
        }
    }
    cout << "Parcel not found.\n";
}

// ================= CUSTOMER FEATURES =================
void printReceipt(const Parcel& p, const City& c) {
    cout << "\n========== RECEIPT ==========\n";
    cout << "Tracking #: " << p.trackingNum << endl;
    cout << "Receiver: " << p.receiverName << endl;
    cout << "City: " << c.name << endl;
    cout << "Weight: " << p.weight << " KG\n";
    cout << "Rate: $" << c.rate << "/KG\n";
    cout << "Total Price: $" << fixed << setprecision(2) << p.price << endl;
    cout << "Status: " << statusText(p.status) << endl;
    cout << "=============================\n";
}

void sendParcel(int userIndex) {
    Parcel p;
    p.trackingNum = generateTrackingNum();
    p.senderID = users[userIndex].id;
    cin.ignore();
    cout << "Receiver Name: "; getline(cin, p.receiverName);
    for (auto& c : cities) cout << c.id << ". " << c.name << " ($" << c.rate << "/KG)\n";
    cout << "City ID: "; cin >> p.cityID;
    cout << "Weight (KG): "; cin >> p.weight;

    for (auto& c : cities) {
        if (c.id == p.cityID) {
            p.price = p.weight * c.rate;
            printReceipt(p, c);
        }
    }
    p.status = 0;
    parcels.push_back(p);
    users[userIndex].trackingHistory.push_back(p.trackingNum);
    saveParcels();
    saveUsers();
}

void trackParcel() {
    int t;
    cout << "Tracking Number: "; cin >> t;
    for (auto& p : parcels)
        if (p.trackingNum == t)
            cout << "Status: " << statusText(p.status) << endl;
}

void cancelShipment() {
    int t;
    cout << "Tracking Number: "; cin >> t;
    for (auto& p : parcels) {
        if (p.trackingNum == t && p.status == 0) {
            p.status = 3;
            saveParcels();
            cout << "Shipment cancelled.\n";
            return;
        }
    }
    cout << "Cannot cancel shipment.\n";
}

void viewHistory(int userIndex) {
    for (int t : users[userIndex].trackingHistory) {
        for (auto& p : parcels)
            if (p.trackingNum == t)
                cout << "Tracking: " << t << " | Status: " << statusText(p.status) << " | Price: $" << p.price << endl;
    }
}

// ================= MENUS =================
void adminMenu(int idx) {
    cout << "======" << users[idx].name << "======\n";
    int ch;
    do {
        cout << "\n1.Add City\n2.View Shipments\n3.Update Status\n0.Logout\nChoice: ";
        cin >> ch;
        if (ch == 1) addCity();
        if (ch == 2) viewAllShipments();
        if (ch == 3) updateParcelStatus();
    } while (ch != 0);
}

void userMenu(int idx) {
    cout << "=======" << users[idx].name << "=======\n";
    int ch;
    do {
        cout << "\n1.Send Parcel\n2.Track Parcel\n3.Cancel Shipment\n4.View History\n0.Logout\nChoice: ";
        cin >> ch;
        if (ch == 1) sendParcel(idx);
        if (ch == 2) trackParcel();
        if (ch == 3) cancelShipment();
        if (ch == 4) viewHistory(idx);
    } while (ch != 0);
}

// ================= EXPORT DATA =================
void exportUsers() {
    ofstream file("Users_Data.csv");
    file << "ID,Name,Phone,Address,Email,Password,IsAdmin\n";
    for (int i = 0; i < users.size(); i++) {
        file << users[i].id << "," << users[i].name << "," << users[i].phone << "," << users[i].address << ","
             << users[i].email << "," << users[i].password << "," << users[i].isAdmin << "\n";
    }
    file.close();
}

void exportParcels() {
    ofstream file("Parcels.csv");
    file << "Tracking Number,City,Price,Status\n";
    for (int i = 0; i < parcels.size(); i++) {
        file << parcels[i].trackingNum << "," << parcels[i].cityID << "," << parcels[i].price << "," << parcels[i].status << "\n";
    }
    file.close();
}

// ================= MAIN =================
int main() {
    loadUsers();
    loadCities();
    loadParcels();
    int choice;
    do {
        cout << "\n======Team23======\n";
        cout << "1.Admin Login\n2.Customer Login\n3.Register Admin\n4.Register Customer\n0.Exit\nChoice: ";
        cin >> choice;

        if (choice == 1 || choice == 2) {
            int idx = login();
            if (idx != -1) {
                if (users[idx].isAdmin) adminMenu(idx);
                else userMenu(idx);
            }
        }
        else if (choice == 3) registerUser(1);
        else if (choice == 4) registerUser(0);
        else if (choice != 0) cout << "Invalid choice, try again please :) ";

    } while (choice != 0);
    exportUsers();
    exportParcels();
    return 0;
}
