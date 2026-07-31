#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <ctime>
#include <limits>

using namespace std;

double balance = 0;

// input filters
int intFilter(const string& prompt) {
    int value;
    char next;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            next = cin.peek();

            if (next == '\n') {
                cin.get();
                return value;
            }
        }

        cout << "Invalid input.\n";
        cin.clear(); // Clear the fail state
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}
double doubleFilter(const string& prompt) {
    double value;
    char next;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            next = cin.peek();

            if (next == '\n') {
                cin.get();
                return value;
            }
        }

        cout << "Invalid input.\n";
        cin.clear(); // Clear the fail state
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

// Load current balance from file
void loadBalance() {
    ifstream file("balance.txt");
    if (file)
        file >> balance;
    file.close();
}

// Save current balance
void saveBalance() {
    ofstream file("balance.txt");
    file << fixed << setprecision(2) << balance;
    file.close();
}

// Add money
void addMoney() {
    double amount = doubleFilter("Enter amount: ");

    string reason;
    cout << "Reason: ";
    cin >> reason;

    balance += amount;
    saveBalance();

    ofstream file("expenses.txt", ios::app);
    file << "Added    : RM" << left << setw(10) << fixed << setprecision(2) << amount << "(" << reason << ")"
        << "\nBalance  : RM" << balance << endl;
}

// Record expenses
void recordExpense() {
    time_t now = time(nullptr);
    tm current;
    localtime_s(&current, &now);

    string today = string("Date: ") +
        (current.tm_mday < 10 ? "0" : "") + to_string(current.tm_mday) + "-" +
        (current.tm_mon + 1 < 10 ? "0" : "") + to_string(current.tm_mon + 1) + "-" +
        to_string(1900 + current.tm_year) + ", " + to_string(current.tm_wday);

    bool dateFound = false;
    string line;

    ifstream fileR("expenses.txt");
    while (getline(fileR, line)) {
        if (line == today) {
            dateFound = true;
            break;
        }
    }

    ofstream file("expenses.txt", ios::app);
    if (!dateFound) {
        file << "\n-------------------------------------------------------\n";
        file << today;
        file << "\n=======================================================\n";
    }

    while (true) {
        string item;
        cout << "\nExpense (0 to finish): ";
        cin >> item;

        if (item == "0") {
            break;
        }

        string category;
        do {
            int categoryChoice = intFilter("(1=food, 2=transport, 3=other, 0 to undo)\nCategory:");
            if (categoryChoice == 0) {
                break;
            }
            switch (categoryChoice) {
            case 1:
                category = "FOOD";
                break;
            case 2:
                category = "TRANSPORT";
                break;
            case 3:
                category = "OTHER";
                break;
            default:
                cout << "Invalid input.";
                continue;
            }
            break;
        } while (true);

        double amount = doubleFilter("Amount: ");
        balance -= amount;

        file << left << fixed << setprecision(2)
            << "Remaining: RM" << setw(8) << balance
            << setw(10) << category
            << setw(16) << item
            << "RM" << amount << endl;
    }

    saveBalance();
    file.close();
}

// Read expenses
void readExpenses() {
    ifstream file("expenses.txt");

    if (!file) {
        cout << "No records found.\n";
        return;
    }
    int year = intFilter("Enter year: ");
    string strYear = to_string(year);
    int month = intFilter("Enter month: ");
    string strMonth = (month < 10 ? "0" : "") + to_string(month);

    string line;
    bool print = false;
    double total = 0;
    double totalW = 0;
    double totalM = 0;
    int fileDay = -1;
    cout << "\n=======================================================\n";

    while (getline(file, line)) {
        if (line.find("Date:") == 0) {
            int fileDay = stoi(line.substr(18));
            if (print) {
                cout << right << setw(49) << "Total Expense    : RM" << left << setw(5) << fixed << setprecision(2) << total << endl;
                if (fileDay == 0) {
                    cout << right << setw(49) << "Total Expense (W): RM" << left << setw(5) << fixed << setprecision(2) << totalW << endl;
                }
                cout << "\n=======================================================\n";
            }

            string fileMonth = line.substr(9, 2);
            string fileYear = line.substr(12, 4);

            if (fileMonth == strMonth && fileYear == strYear) {
                print = true;
                total = 0;

                if (fileDay == 1) {
                    totalW = 0;
                }
            }
            else {
                print = false;
            }
        }

        if (print) {
            cout << line << endl;

            if (line.find("Remaining") != string::npos) {
                string amount = line.substr(line.rfind("RM") + 2);
                total += stod(amount);
                totalW += stod(amount);
                totalM += stod(amount);
            }
        }
    }
    if (print) {
        cout << "\n-------------------------------------------------------" << endl;
        cout << right << setw(49) << "Total Expense    : RM" << left << setw(5) << fixed << setprecision(2) << total << endl;
        if (fileDay == 0) {
            cout << right << setw(49) << "Total Expense (W): RM" << left << setw(5) << fixed << setprecision(2) << totalW << endl;
        }
        cout << right << setw(49) << "Total Expense (M): RM" << left << setw(5) << fixed << setprecision(2) << totalM << endl;
    }
    file.close();
}

void readReport() {
    ifstream file("expenses.txt");

    if (!file) {
        cout << "No report found.";
        return;
    }

    int year = intFilter("Enter year: ");
    string strYear = to_string(year);

    string line;

    double ExpenseM[12][4] = { 0 };
    double totalExpenseY[4] = { 0 };
    double totalAddedM[12] = { 0 };
    double totalAddedY = 0;

    int fileMonth = 0;
    string fileYear = " ";
    while (getline(file, line)) {
        if (line.find("Date:") == 0) {
            fileMonth = stoi(line.substr(9, 2)) - 1;
            fileYear = line.substr(12, 4);
        }
        if (fileYear == strYear) {

            if (line.find("Remaining") != string::npos) {
                double amount = stod(line.substr(line.rfind("RM") + 2));
                ExpenseM[fileMonth][0] += amount;
                totalExpenseY[0] += amount;

                if (line.find("FOOD") != string::npos) {
                    ExpenseM[fileMonth][1] += amount;
                    totalExpenseY[1] += amount;
                }
                else if (line.find("TRANSPORT") != string::npos) {
                    ExpenseM[fileMonth][2] += amount;
                    totalExpenseY[2] += amount;
                }
                else if (line.find("OTHER") != string::npos) {
                    ExpenseM[fileMonth][3] += amount;
                    totalExpenseY[3] += amount;
                }
            }
            else if (line.find("Added") != string::npos) {
                size_t start = line.find("RM") + 2;
                size_t end = line.find("(");
                double amount = stod(line.substr(start, end - start));
                totalAddedM[fileMonth] += amount;
                totalAddedY += amount;
            }
        }
    }
    file.close();

    string monthNames[12] = {
        "Jan", "Feb", "Mar", "Apr",
        "May", "Jun", "Jul", "Aug",
        "Sep", "Oct", "Nov", "Dec"
    };
    cout << endl << strYear << " REPORT";
    cout << "\n- ----- - --------- - --------- - --------- - --------- - --------- - ---------- -"
        << "\n| Month |   Total   |   FOOD.   |   TRANS   |   Other   |   Added   |  CashFlow  |"
        << "\n- ----- - --------- - --------- - --------- - --------- - --------- - ---------- -" << endl;

    int width = 10;
    for (int i=0; i<12; i++) {
        cout << left << fixed << setprecision(2) << noshowpos;
        cout
            << "| " << setw(6) << monthNames[i]
            << "| " << setw(width) << ExpenseM[i][0]
            << "| " << setw(width) << ExpenseM[i][1]
            << "| " << setw(width) << ExpenseM[i][2]
            << "| " << setw(width) << ExpenseM[i][3]
            << "| " << setw(width) << totalAddedM[i]
            << "| " << setw(width + 1) << showpos << totalAddedM[i] - ExpenseM[i][0]
            << "|" << endl;
    }
    cout << "= ===== = ========= = ========= = ========= = ========= = ========= = ========== =" << endl;
    cout << left << fixed << setprecision(2) << noshowpos;
    cout
        << "| " << setw(6) << "All"
        << "| " << setw(width) << totalExpenseY[0]
        << "| " << setw(width) << totalExpenseY[1]
        << "| " << setw(width) << totalExpenseY[2]
        << "| " << setw(width) << totalExpenseY[3]
        << "| " << setw(width) << totalAddedY
        << "| " << setw(width + 1) << showpos << totalAddedY - totalExpenseY[0] << noshowpos
        << "|" << endl;
}

int main() {
    loadBalance();

    int choice;

    do {
        cout << "\n====== Expense Tracker ======\n";
        cout << "Current Balance: RM" << fixed << setprecision(2) << balance << endl;
        cout 
            << "1. Add Money\n"
            << "2. Record Expense\n"
            << "3. View Expenses\n"
            << "4. Report\n"
            << "0. Exit\n";
        choice = intFilter("Choice: ");

        switch (choice) {
        case 1:
            addMoney();
            break;
        case 2:
            recordExpense();
            break;
        case 3:
            readExpenses();
            break;
        case 4:
            readReport();
            break;
        case 0:
            break;
        default:
            cout << "Invalid input.";
            break;
        }
    } while (choice != 0);

    return 0;
}
