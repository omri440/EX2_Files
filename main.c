#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_FILENAME_LENGTH 20
#define MAX_ATTEMPTS 3

// Customer struct
typedef struct {
    int id;
    char account_id[10];
    char name[20]; // Name now comes after account_id
    double balance;
    char pin[4];
} Customer;

// Bank array struct
typedef struct {
    Customer* customers;
    int len;   // Total allocated memory
    int used;  // Current number of customers
} BankArr;

// Function prototypes
FILE* getValidatedFile(const char* prompt);
void initializeBankArr(BankArr* bank);
void addCustomer(BankArr* bank, Customer customer);
void freeBankArr(BankArr* bank);
BankArr readCustomersFromFile(FILE* file);
void mergeBanks(BankArr* bank1, BankArr* bank2, BankArr* merged);
void writeCustomersToFile(BankArr* bank, const char* filename);
int compareCustomersById(const void* a, const void* b);
int compareCustomersByFirstName(const void* a, const void* b);
void sortAndPrintByFirstName(BankArr* bank);
void showMenu(BankArr* mergedBank);
void sortAndPrintByAccountID(BankArr* bank);
int compareCustomersByAccountID(const void* a, const void* b);
int compareCustomersByBalance(const void* a, const void* b);
void sortAndPrintByBalance(BankArr* bank);
void doTransaction();
bool isFileEmpty(const char* filename);
void DisplayAllTransactionsDetails();
void updateBankFileSortedByID(BankArr* bank);

int main() {
    // Step 1: Get and validate file names
    FILE* bank1File = getValidatedFile("Enter First File Name (Bank1.txt):\n");
    FILE* bank2File = getValidatedFile("Enter Second File Name (Bank2.txt):\n");

    if (!bank1File || !bank2File) {
        printf("Failed to open files. Exiting program.\n");
        return 1;
    }

    // Step 2: Initialize banks
    BankArr bank1, bank2, mergedBank;
    initializeBankArr(&bank1);
    initializeBankArr(&bank2);
    initializeBankArr(&mergedBank);

    // Step 3: Read customers from files
    bank1 = readCustomersFromFile(bank1File);
    bank2 = readCustomersFromFile(bank2File);

    fclose(bank1File);
    fclose(bank2File);

    // Step 4: Merge banks
    mergeBanks(&bank1, &bank2, &mergedBank);

    // Step 5: Sort merged customers by ID
    qsort(mergedBank.customers, mergedBank.used, sizeof(Customer), compareCustomersById);

    // Step 6: Write merged customers to Bank.txt
    writeCustomersToFile(&mergedBank, "Bank.txt");
    printf("Merged data written to Bank.txt (sorted by ID).\n");

    // Step 7: Create Transactions.txt
    FILE* transactionsFile = fopen("Transactions.txt", "w");
    if (transactionsFile) {
        printf("Transactions.txt created successfully.\n");
        fclose(transactionsFile);
    } else {
        perror("Error creating Transactions.txt");
    }

    // Step 8: Show menu
    showMenu(&mergedBank);

    // Free allocated memory
    freeBankArr(&bank1);
    freeBankArr(&bank2);
    freeBankArr(&mergedBank);

    return 0;
}

// Function to get a valid file pointer
FILE* getValidatedFile(const char* prompt) {
    char filename[MAX_FILENAME_LENGTH];
    FILE* file = NULL;


    while (1) {
        printf("%s", prompt);
        if (fgets(filename, sizeof(filename), stdin)) {
            filename[strcspn(filename, "\n")] = '\0'; // Remove trailing newline

            if (strcmp(filename, "Bank1.txt") == 0 || strcmp(filename, "Bank2.txt") == 0) {
                file = fopen(filename, "r");
                if (file) {
                    printf("File '%s' opened successfully!\n", filename);
                    return file;
                } else {
                    perror("Error opening file");
                }
            } else {
                printf("Invalid file name. Please enter 'Bank1.txt' or 'Bank2.txt'.\n");
            }
        }
    }
}

// Initialize the BankArr struct
void initializeBankArr(BankArr* bank) {
    bank->customers = NULL;
    bank->len = 0;
    bank->used = 0;
}

// Add a customer to the BankArr
void addCustomer(BankArr* bank, Customer customer) {
    if (bank->used == bank->len) {
        bank->len = (bank->len == 0) ? 10 : bank->len * 2;
        bank->customers = realloc(bank->customers, bank->len * sizeof(Customer));
        if (!bank->customers) {
            perror("Memory allocation failed");
            exit(1);
        }
    }
    bank->customers[bank->used++] = customer;
}

// Free the memory in BankArr
void freeBankArr(BankArr* bank) {
    free(bank->customers);
    bank->customers = NULL;
    bank->len = 0;
    bank->used = 0;
}

// Read customers from a file into BankArr
BankArr readCustomersFromFile(FILE* file) {
    BankArr bank;
    initializeBankArr(&bank);

    Customer customer;
    while (fscanf(file, "%d,%[^,],%[^,],%lf,%s", &customer.id, customer.account_id,
                  customer.name, &customer.balance, customer.pin) == 5) {
        addCustomer(&bank, customer);
                  }

    return bank;
}

// Merge two banks into one
void mergeBanks(BankArr* bank1, BankArr* bank2, BankArr* merged) {
    for (int i = 0; i < bank1->used; i++) {
        addCustomer(merged, bank1->customers[i]);
    }

    for (int i = 0; i < bank2->used; i++) {
        int found = 0;
        for (int j = 0; j < bank1->used; j++) {
            if (bank2->customers[i].id == bank1->customers[j].id) {

                merged->customers[j].balance += bank2->customers[i].balance;
                found = 1;
                break;
            }
        }
        if (!found) {
            addCustomer(merged, bank2->customers[i]);
        }
    }
}

// Comparator function to sort customers by ID
int compareCustomersById(const void* a, const void* b) {
    Customer* customerA = (Customer*)a;
    Customer* customerB = (Customer*)b;
    return customerA->id - customerB->id;
}

// Comparator function to sort customers by first name
int compareCustomersByName(const void* a, const void* b) {
    const Customer* customerA = (const Customer*)a;
    const Customer* customerB = (const Customer*)b;

    return strcmp(customerA->name, customerB->name); // Compare names lexicographically
}

// Write customers to a file
void writeCustomersToFile(BankArr* bank, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    for (int i = 0; i < bank->used; i++) {
        Customer* customer = &bank->customers[i];
        fprintf(file, "%d,%s,%s,%.2f,%s\n", customer->id, customer->name,
                customer->account_id, customer->balance, customer->pin);
    }

    fclose(file);
}

// Sort and print customers by name
void sortAndPrintByName(BankArr* bank) {
    // Sort the array of customers by name
    qsort(bank->customers, bank->used, sizeof(Customer), compareCustomersByName);

    // Print the sorted customers
    printf("\nCustomers sorted by name:\n");
    for (int i = 0; i < bank->used; i++) {
        Customer* customer = &bank->customers[i];
        printf("%d, %s, %s, %.2f, %s\n", customer->id, customer->account_id,
               customer->name, customer->balance, customer->pin);
    }
}

void doTransaction(BankArr* bank) {
    char accountID[10], pin[5];  // PIN buffer should hold 4 characters + null terminator
    int attempts = 0, transactionType;
    double amount;

    while (attempts < MAX_ATTEMPTS) {
        // Flush input buffer
        while (getchar() != '\n');

        printf("Enter Account ID:\n");
        if (!fgets(accountID, sizeof(accountID), stdin)) {
            printf("Input error. Please try again.\n");
            continue;
        }
        accountID[strcspn(accountID, "\n")] = '\0'; // Remove trailing newline

        printf("Enter Account PIN:\n");
        if (!fgets(pin, sizeof(pin), stdin)) {
            printf("Input error. Please try again.\n");
            continue;
        }
        pin[strcspn(pin, "\n")] = '\0'; // Remove trailing newline

        int found = 0;
        for (int i = 0; i < bank->used; i++) {
            // printf("Debug: Comparing '%s' with '%s'\n", bank->customers[i].account_id, accountID);
            // printf("Debug: Comparing PIN '%s' with '%s'\n", bank->customers[i].pin, pin);

            if (strcmp(bank->customers[i].account_id, accountID) == 0 &&
                strcmp(bank->customers[i].pin, pin) == 0) {
                found = 1;

                printf("For Withdraw Press 0, For Deposit Press 1:\n");
                if (scanf("%d", &transactionType) != 1 || (transactionType != 0 && transactionType != 1)) {
                    printf("Invalid transaction type.\n");
                    while (getchar() != '\n'); // Clear buffer
                    return;
                }

                printf("Chose Transaction Amount:\n");
                if (scanf("%lf", &amount) != 1 || amount <= 0) {
                    printf("Invalid amount.\n");
                    while (getchar() != '\n'); // Clear buffer
                    return;
                }

                if (transactionType == 0) { // Withdraw
                    if (bank->customers[i].balance < amount) {
                        printf("Insufficient balance.\n");
                        return;
                    }
                    bank->customers[i].balance -= amount;
                    printf("Withdrawn %.2f successfully.\n", amount);
                } else { // Deposit
                    bank->customers[i].balance += amount;
                    printf("Deposited %.2f successfully.\n", amount);
                }

                // Log transaction
                FILE* transactionFile = fopen("Transactions.txt", "a");
                if (transactionFile) {
                    fprintf(transactionFile, "%s,%.2f,%d\n", accountID, amount, transactionType);
                    fclose(transactionFile);
                } else {
                    perror("Error opening transaction file.");
                }

                return;
            }
        }

        if (!found) {
            printf("One Of The Details Wrong!\n");
            attempts++;
        }
    }
    printf("You Tried Too Many Times, Please Try Again Later.\n");
}



// Update the menu to include option 3
void showMenu(BankArr* mergedBank) {
    int choice;

    do {
        printf("\n********** Main Menu **********\n");
        printf("1. Display Customers Sorted By Name\n");
        printf("2. Display Customers Sorted By Account ID\n");
        printf("3. Display Customers Sorted By Account Balance\n");
        printf("4. Do Transaction\n");
        printf("5. Display All Transactions Details.\n");
        printf("6. Update Bank File Sorted By Customer ID.\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                sortAndPrintByName(mergedBank);
            break;
            case 2:
                sortAndPrintByAccountID(mergedBank);
            break;
            case 3:
                sortAndPrintByBalance(mergedBank);
            break;
            case 4:
                doTransaction(mergedBank);
            break;
            case 5:
                DisplayAllTransactionsDetails();
            break;
            case 6:
                updateBankFileSortedByID(mergedBank);
            break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);
}

int compareCustomersByAccountID(const void* a, const void* b) {
    const Customer* customerA = (const Customer*)a;
    const Customer* customerB = (const Customer*)b;

    return strcmp(customerA->account_id, customerB->account_id);
}

void sortAndPrintByAccountID(BankArr* bank) {
    qsort(bank->customers, bank->used, sizeof(Customer), compareCustomersByAccountID);

    printf("\nCustomers sorted by account ID:\n");
    for (int i = 0; i < bank->used; i++) {
        Customer* customer = &bank->customers[i];
        printf("%d, %s, %s, %.2f, %s\n", customer->id, customer->account_id,
               customer->name, customer->balance, customer->pin);
    }
}


int compareCustomersByBalance(const void* a, const void* b) {
    const Customer* customerA = (const Customer*)a;
    const Customer* customerB = (const Customer*)b;

    // Compare balances
    if (customerA->balance > customerB->balance) {
        return -1; // customerA comes first
    } else if (customerA->balance < customerB->balance) {
        return 1;  // customerB comes first
    } else {
        return 0;  // Equal balances
    }
}

// Sort and print customers by account balance
void sortAndPrintByBalance(BankArr* bank) {
    qsort(bank->customers, bank->used, sizeof(Customer), compareCustomersByBalance);

    printf("\nCustomers sorted by account balance:\n");
    for (int i = 0; i < bank->used; i++) {
        Customer* customer = &bank->customers[i];
        printf("%d, %s, %s, %.2f, %s\n", customer->id, customer->account_id,
               customer->name, customer->balance, customer->pin);
    }
}

void DisplayAllTransactionsDetails() {
    FILE* transactionFile = fopen("Transactions.txt", "r");
    if (!transactionFile) {
        perror("Error opening transaction file.");
        return;
    }

    // Check if the file is empty
    if (isFileEmpty("Transactions.txt")) {
        printf("Transactions History Is Empty.\n");
        fclose(transactionFile);
        return;
    }

    // Variables to hold transaction data
    char accountID[10];
    double amount;
    int action;

    // Print the header
    printf("Display Transaction Details:\n");

    // Read each transaction from the file and display it
    while (fscanf(transactionFile, "%9[^,],%lf,%d\n", accountID, &amount, &action) == 3) {
        printf("Account ID: %s, Amount: %.2f, Action: %d\n", accountID, amount, action);
    }

    fclose(transactionFile);
}

bool isFileEmpty(const char* filename) {
    FILE* file = fopen(filename, "r"); // Open file in read mode
    if (!file) {
        perror("Error opening file");
        return false; // File does not exist or cannot be opened
    }

    // Move to the end of the file
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file); // Get the current position (size of the file)

    fclose(file); // Close the file

    return (fileSize == 0); // Return true if size is 0, false otherwise
}

void updateBankFileSortedByID(BankArr* bank) {
    // Step 1: Sort customers by ID
    qsort(bank->customers, bank->used, sizeof(Customer), compareCustomersById);

    // Step 2: Open Bank.txt for writing
    FILE* file = fopen("Bank.txt", "w");
    if (!file) {
        perror("Error opening Bank.txt for writing");
        return;
    }

    // Step 3: Write sorted customers back to Bank.txt
    for (int i = 0; i < bank->used; i++) {
        Customer* customer = &bank->customers[i];
        fprintf(file, "%d,%s,%s,%.2f,%s\n", customer->id, customer->account_id,
                customer->name, customer->balance, customer->pin);
    }

    fclose(file); // Close the file
    printf("Bank file updated and sorted by Customer ID.\n");
}
