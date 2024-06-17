#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#ifdef _WIN64
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#define MAX_STUDENTS 800

__declspec(dllexport) void Scanfint(int *input){
	char buffer[100];
	if(fgets(buffer, sizeof(buffer), stdin) != NULL){
		if(sscanf(buffer, "%d", input) != 1){
			*input = -100;
		}	
	}
	fflush(stdin);
}
//input string
__declspec(dllexport) void Scanfstr(char *str) {
	fflush(stdin);
	scanf(" %[^\n]", str);
	fflush(stdin);
}
__declspec(dllexport) int isValidName(const char *name);
//===================================================================================
__declspec(dllexport) void mainDatasiswa();
//TREE LOGIN REGISTER
// Structure for a teacher node
__declspec(dllexport) struct Teacher {
    char codeGuru[100];
    char fullname[100];
    char email[100];
};

// AVL tree1 node
__declspec(dllexport) struct AVLNode {
    struct Teacher *teacher;
    int height;
    struct AVLNode *left;
    struct AVLNode *right;
};

// Function to create a new teacher node
__declspec(dllexport) struct Teacher* createTeacher(char codeGuru[], char fullname[], char email[]) {
    struct Teacher *newTeacher = (struct Teacher*)malloc(sizeof(struct Teacher));
    strcpy(newTeacher->codeGuru, codeGuru);
    strcpy(newTeacher->fullname, fullname);
    strcpy(newTeacher->email, email);
    return newTeacher;
}

// Function to get the maximum of two integers
__declspec(dllexport) int max1(int a, int b) {
    return (a > b) ? a : b;
}

// Function to get the height of a node
__declspec(dllexport) int height(struct AVLNode *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to perform a left rotation
__declspec(dllexport) struct AVLNode* rotateLeft(struct AVLNode *y) {
    struct AVLNode *x = y->right;
    struct AVLNode *T2 = x->left;

    x->left = y;
    y->right = T2;

    // Update heights
    y->height = max1(height(y->left), height(y->right)) + 1;
    x->height = max1(height(x->left), height(x->right)) + 1;

    return x;
}

// Function to perform a right rotation
__declspec(dllexport) struct AVLNode* rotateRight(struct AVLNode *x) {
    struct AVLNode *y = x->left;
    struct AVLNode *T2 = y->right;

    y->right = x;
    x->left = T2;

    // Update heights
    x->height = max1(height(x->left), height(x->right)) + 1;
    y->height = max1(height(y->left), height(y->right)) + 1;

    return y;
}

// Function to get the balance factor of a node
__declspec(dllexport) int getBalance(struct AVLNode *node) {
    if (node == NULL)
        return 0;
    return height(node->left) - height(node->right);
}

// Function to insert a teacher into AVL tree
__declspec(dllexport) struct AVLNode* insertTeacherAVL(struct AVLNode *root, struct Teacher *teacher) {
    if (root == NULL) {
        struct AVLNode *newNode = (struct AVLNode*)malloc(sizeof(struct AVLNode));
        newNode->teacher = teacher;
        newNode->height = 1;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }

    int compare;
    if (root->teacher == NULL) {
        compare = -1; // Assume the teacher's codeGuru is smaller than any non-NULL value
    } else {
        compare = strcmp(teacher->codeGuru, root->teacher->codeGuru);
    }

    if (compare < 0)
        root->left = insertTeacherAVL(root->left, teacher);
    else if (compare > 0)
        root->right = insertTeacherAVL(root->right, teacher);
    else {
        free(teacher);
        return root;
    }

    // Update height
    root->height = 1 + max1(height(root->left), height(root->right));
    int balance = getBalance(root);

    // LLC
    if (balance > 1 && (root->left != NULL) && strcmp(teacher->codeGuru, root->left->teacher->codeGuru) < 0)
        return rotateRight(root);

    // RRC
    if (balance < -1 && (root->right != NULL) && strcmp(teacher->codeGuru, root->right->teacher->codeGuru) > 0)
        return rotateLeft(root);

    // LRC
    if (balance > 1 && (root->left != NULL) && strcmp(teacher->codeGuru, root->left->teacher->codeGuru) > 0) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }

    // RLC
    if (balance < -1 && (root->right != NULL) && strcmp(teacher->codeGuru, root->right->teacher->codeGuru) < 0) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}


// Function to search for a teacher by name
__declspec(dllexport) struct Teacher* search(struct AVLNode *root, char codeGuru[]) {
    if (root == NULL || strcmp(root->teacher->codeGuru, codeGuru) == 0)
        return root ? root->teacher : NULL;

    if (strcmp(codeGuru, root->teacher->codeGuru) < 0)
        return search(root->left, codeGuru);
    else
        return search(root->right, codeGuru);
}

__declspec(dllexport) struct Teacher* searchfullname(struct AVLNode *root, char fullname[]) {
    if (root == NULL || strcmp(root->teacher->fullname, fullname) == 0)
        return root ? root->teacher : NULL;

    if (strcmp(fullname, root->teacher->fullname) < 0)
        return search(root->left, fullname);
    else
        return search(root->right, fullname);
}

__declspec(dllexport) struct AVLNode* loadTeachersFromFile(struct AVLNode *root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return root;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *fullname = strtok(line, ",");
        char *codeGuru = strtok(NULL, ",");
        char *email = strtok(NULL, "\n");

        if (fullname && codeGuru && email) {
            struct Teacher *newTeacher = createTeacher(codeGuru, fullname, email);
            root = insertTeacherAVL(root, newTeacher);
        }
    }

    fclose(file);
    return root;
}
__declspec(dllexport) struct Teacher* searchByEmail(struct AVLNode *root, char email[]) {
    if (root == NULL) return NULL;

    if (strcmp(root->teacher->email, email) == 0) {
        return root->teacher;
    }

    struct Teacher *result = searchByEmail(root->left, email);
    if (result != NULL) return result;

    return searchByEmail(root->right, email);
}
// Function to search for a teacher by codeGuru
__declspec(dllexport) struct Teacher* search3(struct AVLNode *root, char codeGuru[]) {
    if (root == NULL || strcmp(root->teacher->codeGuru, codeGuru) == 0)
        return root ? root->teacher : NULL;

    if (strcmp(codeGuru, root->teacher->codeGuru) < 0)
        return search3(root->left, codeGuru);
    else
        return search3(root->right, codeGuru);
}
//===================================================================================
// AVL TREE 2
//TREE DATA SISWA
// Structure for a data siswa

// Structure for a student
__declspec(dllexport) struct DataSiswa {
    char studentID[20];
    char fullname[100];
    int nilaiMat;
    int nilaiFis;
    int nilaiKim;
    int nilaiBio;
    int nilaiBindo;
    int kelas;
    double rata2;
    char grade;
};

// AVL tree node
__declspec(dllexport) struct AVLSiswa {
    struct DataSiswa *data;
    int height2;
    struct AVLSiswa *left;
    struct AVLSiswa *right;
};

__declspec(dllexport) struct StudentArray {
    struct DataSiswa *students[MAX_STUDENTS];
    int count;
};

__declspec(dllexport) void displayStudents(struct AVLSiswa **node);
__declspec(dllexport) int search2(struct AVLSiswa *root, char *name);
__declspec(dllexport) struct AVLSiswa* insertDataSiswa(struct AVLSiswa *node, struct DataSiswa *data);
__declspec(dllexport) const char* generateExportName(const char* filename);

// Inisialisasi seed untuk generator angka acak sekali saja
__declspec(dllexport) void initializeRandom() {
    srand(time(NULL));
}

__declspec(dllexport) void generateID(char fullname[], char* studentID) {
    // Pastikan fullname tidak kosong
    if (fullname[0] == '\0') {
        printf("Nama lengkap tidak boleh kosong.\n");
        return;
    }
    // Karakter pertama dari fullname
    studentID[0] = fullname[0];
    // Menghasilkan 9 angka acak
    for (int i = 1; i < 10; i++) {
        studentID[i] = '0' + rand() % 10;
    }
    // Menambahkan null terminator untuk string studentID
    studentID[10] = '\0';
}
// Fungsi untuk format nama
__declspec(dllexport) void formatname(char *input) {
    int i = 0;
    int newWord = 1;  // Flag untuk cek apakah di awal kata baru

    while (input[i] != '\0') {
        if (newWord && isalpha((unsigned char)input[i])) {
            input[i] = toupper((unsigned char)input[i]);
            newWord = 0;
        } else {
            input[i] = tolower((unsigned char)input[i]);
        }
        if (input[i] == ' ') {
            newWord = 1;  // Karakter berikutnya adalah awal kata baru
        }
        i++;
    }
}

__declspec(dllexport) char generateGrade(double rata2) {
    char grade;

    if (rata2 >= 90) {
        grade = 'A';
    } else if (rata2 >= 80) {
        grade = 'B';
    } else if (rata2 >= 70) {
        grade = 'C';
    } else if (rata2 >= 50) {
        grade = 'D';
    } else if (rata2 >= 30) {
        grade = 'E';
    } else {
        grade = 'F';
    }
    return grade;
}

// Function to create a new student node
__declspec(dllexport) struct DataSiswa* createDataSiswa(char fullname[], int nilaiMat, int nilaiFis, int nilaiKim, int nilaiBio, int nilaiBindo, int kelas) {
    struct DataSiswa *newSiswa = (struct DataSiswa*)malloc(sizeof(struct DataSiswa));
    // Inisialisasi seed untuk generator angka acak
    initializeRandom();
    //format nama
    formatname(fullname);
    //generate student id
    char studentID[20];
    generateID(fullname, studentID);
    strcpy(newSiswa->studentID, studentID);
    strcpy(newSiswa->fullname, fullname);
    newSiswa->nilaiMat = nilaiMat;
    newSiswa->nilaiFis = nilaiFis;
    newSiswa->nilaiKim = nilaiKim;
    newSiswa->nilaiBio = nilaiBio;
    newSiswa->nilaiBindo = nilaiBindo;
    newSiswa->kelas = kelas;
    newSiswa->rata2 = (nilaiMat + nilaiFis + nilaiKim + nilaiBio + nilaiBindo)/5.0;
    newSiswa->grade = generateGrade(newSiswa->rata2);
    return newSiswa;
}

// Function to get the maximum of two integers
__declspec(dllexport) int max(int a, int b) {
    return (a > b) ? a : b;
}

// Function to get the height2 of a node
__declspec(dllexport) int height2(struct AVLSiswa *node) {
    if (node == NULL)
        return 0;
    return node->height2;
}

// Function to perform a left rotation
__declspec(dllexport) struct AVLSiswa* rotateLeft2(struct AVLSiswa *y) {
    struct AVLSiswa *x = y->right;
    struct AVLSiswa *T2 = x->left;

    x->left = y;
    y->right = T2;

    // Update 
    y->height2 = max(height2(y->left), height2(y->right)) + 1;
    x->height2 = max(height2(x->left), height2(x->right)) + 1;

    return x;
}

// Function to get the balance factor of a node
__declspec(dllexport) int getBalance2(struct AVLSiswa *node) {
    if (node == NULL)
        return 0;
    return height2(node->left) - height2(node->right);
}

// Function to perform a right rotation
__declspec(dllexport) struct AVLSiswa* rotateRight2(struct AVLSiswa *x) {
    struct AVLSiswa *y = x->left;
    struct AVLSiswa *T2 = y->right;

    y->right = x;
    x->left = T2;

    // Update 
    x->height2 = max(height2(x->left), height2(x->right)) + 1;
    y->height2 = max(height2(y->left), height2(y->right)) + 1;

    return y;
}

// Function to search22 for a student by fullname
__declspec(dllexport) int search2(struct AVLSiswa *root, char *fullname) {
    if (root == NULL)
        return 1;//belum ada
    if (strcmp(fullname, root->data->fullname) < 0)
        return search2(root->left, fullname);
    else if (strcmp(fullname, root->data->fullname) > 0)
        return search2(root->right, fullname);
    else {
        return 0;//udah ada
    }
}

// Function to insert a student into AVL tree
__declspec(dllexport) struct AVLSiswa* insertDataSiswa(struct AVLSiswa *node, struct DataSiswa *data) {
    if (node == NULL) {
        struct AVLSiswa *newNode = (struct AVLSiswa*)malloc(sizeof(struct AVLSiswa));
        newNode->data = data;
        newNode->height2 = 1;
        newNode->left = NULL;
        newNode->right = NULL;
        return newNode;
    }

    int compare = strcmp(data->studentID, node->data->studentID);

    if (compare < 0)
        node->left = insertDataSiswa(node->left, data);
    else if (compare > 0)
        node->right = insertDataSiswa(node->right, data);
    else {
        printf("Error Student ID is duplicate!");
        return NULL;
    }

    // Update height2 
    node->height2 = 1 + max(height2(node->left), height2(node->right));
    int balance = getBalance2(node);

    // LLC
    if (balance > 1 && strcmp(data->studentID, node->left->data->studentID) < 0)
        return rotateRight2(node);

    // RRC
    if (balance < -1 && strcmp(data->studentID, node->right->data->studentID) > 0)
        return rotateLeft2(node);

    // LRC
    if (balance > 1 && strcmp(data->studentID, node->left->data->studentID) > 0) {
        node->left = rotateLeft2(node->left);
        return rotateRight2(node);
    }

    // RLC
    if (balance < -1 && strcmp(data->studentID, node->right->data->studentID) < 0) {
        node->right = rotateRight2(node->right);
        return rotateLeft2(node);
    }

    return node;
}



// Function to search22 for a student by ID
__declspec(dllexport) struct DataSiswa* searchID(struct AVLSiswa *root, char *studentID) {
    if (root == NULL || strcmp(root->data->studentID, studentID) == 0)
        return root ? root->data : NULL;

    if (strcmp(studentID, root->data->studentID) < 0)
        return searchID(root->left, studentID);
    else
        return searchID(root->right, studentID);
}



// Function to find the node with the minimum value
__declspec(dllexport) struct AVLSiswa* minValueNode(struct AVLSiswa *node) {
    struct AVLSiswa *current = node;

    while (current && current->left != NULL)
        current = current->left;

    return current;
}

// Function to delete a student node
__declspec(dllexport) struct AVLSiswa* deleteDataSiswa(struct AVLSiswa *node, char *studentID) {
    if (node == NULL)
        return node;

    if (strcmp(studentID, node->data->studentID) < 0)
        node->left = deleteDataSiswa(node->left, studentID);
    else if (strcmp(studentID, node->data->studentID) > 0)
        node->right = deleteDataSiswa(node->right, studentID);
    else {
        if (node->left == NULL) {
            struct AVLSiswa *temp = node->right;
            free(node);
            return temp;
        } else if (node->right == NULL) {
            struct AVLSiswa *temp = node->left;
            free(node);
            return temp;
        }

        struct AVLSiswa *temp = minValueNode(node->right);
        node->data = temp->data;
        node->right = deleteDataSiswa(node->right, temp->data->studentID);
    }

    // Update 
    node->height2 = 1 + max(height2(node->left), height2(node->right));
    int balance = getBalance2(node);

    // LLC
    if (balance > 1 && getBalance2(node->left) >= 0)
        return rotateRight2(node);

    // LRC
    if (balance > 1 && getBalance2(node->left) < 0) {
        node->left = rotateLeft2(node->left);
        return rotateRight2(node);
    }

    // RRC
    if (balance < -1 && getBalance2(node->right) <= 0)
        return rotateLeft2(node);

    // RLC
    if (balance < -1 && getBalance2(node->right) > 0) {
        node->right = rotateRight2(node->right);
        return rotateLeft2(node);
    }

    return node;
}



//----------------------------------------FUNCTION SELAIN AVL TREE----------------------------------------------------------

//---------------------------------------------LOGIN/REGISTER---------------------------------------------------------------
// Function to clear the screen
__declspec(dllexport) void ClearScreen() {
    printf("Press enter to continue...");
    getchar();
    system("cls"); // Clear the screen
}
__declspec(dllexport) void animatePrint(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        putchar(str[i]);
        fflush(stdout);
        usleep(280000);
    }
}

__declspec(dllexport) void appendTeacherToFile(const char *filename, struct Teacher *teacher) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    fprintf(file, "%s,%s,%s\n", teacher->fullname, teacher->codeGuru, teacher->email);
    fclose(file);
}

__declspec(dllexport) void generateVerificationCode() {
    FILE *fp;
    int i, code[10];

    // Seed the random number generator
    srand(time(NULL));

    // Generate a random 10-digit code
    for (i = 0; i < 10; i++) {
        code[i] = rand() % 10;
    }

    // Open the file in write mode, overwriting it if it already exists
    fp = fopen("verif.csv", "w");
    if (fp == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    // Write the code to the file
    for (i = 0; i < 10; i++) {
        fprintf(fp, "%d", code[i]);
    }

    // Close the file
    fclose(fp);

    printf("Verification code generated and written to verif.csv.\n");
}

// Function to verify the code entered by the user
__declspec(dllexport) int verifyCode(char *code) {
    FILE *fp;
    char storedCode[11];
    struct AVLNode *root = NULL;
    root = loadTeachersFromFile(root, "DataGuru.csv");

    // Open the file for reading
    fp = fopen("verif.csv", "r");
    if (fp == NULL) {
        printf("Verification file not found.\n");
        return 0;
    }

    // Read the stored code from the file
    fscanf(fp, "%s", storedCode);

    // Close the file
    fclose(fp);

    // Compare the entered code with the stored code
    if (strcmp(code, storedCode) == 0) {
        return 1;
    } else {
        return 0;
    }
}

// Function to display login page
__declspec(dllexport) void loginPage() {
    struct AVLNode *root = NULL;
    root = loadTeachersFromFile(root, "DataGuru.csv");

    while (1) {
        printf("Login Page\n");
        printf("===========\n\n");
        char codeGuru[100];
        char fullname[100];
        printf("Enter your Teacher ID: ");
        scanf("%s", codeGuru);
        printf("Enter your fullname: ");
        scanf("%s", fullname);

        struct Teacher *result = search(root, codeGuru);
        if (result != NULL && strcmp(result->codeGuru, codeGuru) == 0 && strcmp(result->fullname, fullname) == 0) {
            

            // Generate and display verification code
            generateVerificationCode();
            printf("Please enter the verification code: ");
            char verificationCode[11];
            scanf("%s", verificationCode);

            // Verify the code
            if (verifyCode(verificationCode)) {
			    printf("Verification successful!\n");
			    printf("Login successful! Welcome, %s.\n", result->fullname);
			    printf("\nGo to Our Homepage System ==>\n");
			    getchar(); // Clear remaining input buffer
			    ClearScreen();
			    
			    mainDatasiswa();
			    return;
			} else {
			    printf("Verification failed! Incorrect code.\n");
			    printf("Press [1] to go back to login page.\n");
			    int choice;
			    scanf("%d", &choice);
			
			    // Clear input buffer
			    while (getchar() != '\n');
			
			    ClearScreen();
			    if (choice == 1) {
			        continue;
			    }
			}

        } else {
            printf("Login failed! Check your ID or fullname. Please try again.\n");
            printf("Press [1] to go back or [2] to retry: ");
            int choice;
            scanf("%d", &choice);

            // Clear input buffer
            while (getchar() != '\n');

            ClearScreen();
            if (choice == 1) {
                return;
            }
        }
    }
}


// For registration
__declspec(dllexport) void registerPage(struct AVLNode **root) {
    // Load teachers from file before registration
    *root = loadTeachersFromFile(*root, "DataGuru.csv");

    printf("Register Page\n");
    printf("==============\n\n");

    char codeGuru[100];
    char fullname[100];
    char email[100];

    // Input email
    printf("Enter your email: ");
    scanf("%s", email);

	// Validate email format
	int emailLength = strlen(email);
	if (emailLength < 6 || strchr(email, '@') == NULL) {
	    int comIndex = emailLength - 4;
	    int idIndex = emailLength - 3;
	    if ((comIndex < 0 || strncmp(&email[comIndex], ".com", 4) != 0) &&
	        (idIndex < 0 || strncmp(&email[idIndex], ".id", 3) != 0)) {
	        printf("Invalid email format! Email must contain '@' symbol and end with '.com' or '.id'.\n");
	        getchar(); // Clear remaining input buffer
	        ClearScreen();
	        return;
	    }
	}

    // Check if email already exists
    struct Teacher *emailCheck = searchByEmail(*root, email);
    if (emailCheck != NULL) {
        printf("User with this email already exists. Please use another email.\n");
        getchar(); // Clear remaining input buffer
        ClearScreen();
        return;
    }

    // Input Teacher ID
    printf("Enter your Teacher ID (10 digits): ");
    scanf("%s", codeGuru);
    
    // Validate Teacher ID format
    int codeGuruLength = strlen(codeGuru);
    if (codeGuruLength != 10 || !isdigit(codeGuru[0])) {
        printf("Invalid Teacher ID format! Teacher ID must have exactly 10 digits of number.\n");
        getchar(); // Clear remaining input buffer
        ClearScreen();
        return;
    }

    // Check if Teacher ID already exists
    struct Teacher *result = search(*root, codeGuru);
    if (result != NULL) {
        printf("User with this ID already exists. Please choose another ID.\n");
        getchar(); // Clear remaining input buffer
        ClearScreen();
        return;
    }

    // Input fullname
    while (1) {
        printf("Input fullname[2-90]: ");
        getchar(); // Clear remaining input buffer
        fgets(fullname, sizeof(fullname), stdin);
        fullname[strcspn(fullname, "\n")] = '\0'; 
        if (strlen(fullname) < 2 || strlen(fullname) > 90 || !isValidName(fullname)) {
            printf("Fullname must be alfabet between 2 and 90 characters!\n");
            continue;
        }else 
            break;
    }

    // Create new teacher and insert into AVL tree
    struct Teacher *newTeacher = createTeacher(codeGuru, fullname, email);
    *root = insertTeacherAVL(*root, newTeacher);
    // Append new teacher to file
    appendTeacherToFile("DataGuru.csv", newTeacher);

    printf("Registration successful! \n\nPlease login to continue.\n\n");
    fflush(stdin); // Clear remaining input buffer
    ClearScreen();
    loginPage(*root);
}

//---------------------------------------------DATA SISWA---------------------------------------------------------
// Fungsi untuk memeriksa apakah string hanya berisi huruf dan spasi

__declspec(dllexport) void writeStudentsToFile(struct AVLSiswa *root, const char *filename);
__declspec(dllexport) int isValidName(const char *name) {
    for (int i = 0; name[i] != '\0'; i++) {
        if (!isalpha((unsigned char)name[i]) && name[i] != ' ') {
            return 0; // Nama tidak valid
        }
    }
    return 1; // Nama valid
}
__declspec(dllexport) void insertData(struct AVLSiswa **node) {
    char fullname[100];
    int nilaiMat, nilaiFis, nilaiKim, nilaiBio, nilaiBindo, kelas;
    //fullname
    while (1) {
        printf("Input fullname[2-90]: ");
        fgets(fullname, sizeof(fullname), stdin);
        fullname[strcspn(fullname, "\n")] = '\0'; 
        if (strlen(fullname) < 2 || strlen(fullname) > 90 || !isValidName(fullname)) {
            printf("Fullname must be alfabet between 2 and 90 characters!\n");
            continue;
        }else 
            break;
    }

    printf("\n");
    //kelas
    while(1){
        printf("Input the class [ 10 | 11 | 12 ]: ");
        Scanfint(&kelas);
        if(kelas != 10 && kelas != 11 && kelas != 12) {
            printf("Error please Input the class with number 10 or 11 or 12!\n");	
            continue;
        }else{
            break;
        }
    }
    //nilai
    printf("\n");
    while(1){
        printf("Input the mathematics score [0-100]: ");
        Scanfint(&nilaiMat);
        if(nilaiMat < 0 || nilaiMat > 100) {
            printf("Error please Input the mathematics score with number 0 to 100!\n");	
            continue;
        }
        break;
    }
    printf("\n");
    while(1){
        printf("Input the physics score [0-100]: ");
        Scanfint(&nilaiFis);
        if(nilaiFis < 0 || nilaiFis > 100) {
            printf("Error please Input the physics score with number 0 to 100!\n");	
            continue;
        }
        break;
    }
    printf("\n");
    while(1){
        printf("Input the chemistry score [0-100]: ");
        Scanfint(&nilaiKim);
        if(nilaiKim < 0 || nilaiKim > 100) {
            printf("Error please Input the chemistry score with number 0 to 100!\n");
            continue;
        }
        break;
    }
    printf("\n");
    while(1){
        printf("Input the biology score [0-100]: ");
        Scanfint(&nilaiBio);
        if(nilaiBio < 0 || nilaiBio > 100) {
            printf("Error please Input the biology score with number 0 to 100!\n");	
            continue;
        }
        break;
    }
    printf("\n");
    while(1){
        printf("Input the language score [0-100]: ");
        Scanfint(&nilaiBindo);
        if(nilaiBindo < 0 || nilaiBindo > 100) {
            printf("Error please Input the language score with number 0 to 100!\n");	
            continue;
        }
        break;
    }
    printf("\n");
    //pastiin anak ga keduplikat
    if(search2(*node, fullname) == 0){
        printf("The fullname already exists. Are you sure you want to continue?\nClick [1] to not insert the data, and check the student list: ");
        int status;
        Scanfint(&status);
        if(status == 1){
            displayStudents(&(*node));
        }
    }
    struct DataSiswa *newStudent = createDataSiswa(fullname, nilaiMat, nilaiFis, nilaiKim, nilaiBio, nilaiBindo, kelas);
    struct AVLSiswa *newAVLSiswa = insertDataSiswa(*node, newStudent);
    if(newAVLSiswa == NULL) {
        printf("Student data that you had input is not added, continue to homepage!\n");
        ClearScreen();
    }else{
        *node = newAVLSiswa;
        const char *filename = "sma_students_data1.csv";
        writeStudentsToFile(*node, filename);
        printf("Student data successfully added!\n");
        ClearScreen();
    }
}



__declspec(dllexport) void formatNameDisplay(char *input, char *output) {
    int input_len = strlen(input);
    int output_index = 0;
    int word_start = 0;
    int word_count = 0;
    int total_len = 0;
    
    for (int i = 0; i <= input_len; i++) {
        if (input[i] == ' ' || input[i] == '\0') {
            int word_len = i - word_start;
            if (word_count == 0) {
                if (total_len + word_len > 17) {
                    strncpy(output + output_index, input + word_start, 17 - total_len);
                    output[17] = '.';
                    output[18] = '\0';
                    return;
                } else {
                    strncpy(output + output_index, input + word_start, word_len);
                    output_index += word_len;
                    total_len += word_len;
                    if (input[i] != '\0') {
                        output[output_index++] = ' ';
                        total_len++;
                    }
                }
            } else if (word_count == 1) {
                if (total_len + word_len + 1 > 18) { // +1 for the space
                    output[output_index - 1] = '.';
                    output[output_index] = '\0';
                    return;
                } else {
                    strncpy(output + output_index, input + word_start, word_len);
                    output_index += word_len;
                    total_len += word_len;
                    if (input[i] != '\0') {
                        output[output_index++] = ' ';
                        total_len++;
                    }
                }
            } else {
                if (total_len + 2 > 18) {
                    output[output_index - 1] = '.';
                    output[output_index] = '\0';
                    return;
                } else {
                    output[output_index++] = input[word_start];
                    output[output_index++] = '.';
                    total_len += 2;
                    if (input[i] != '\0') {
                        output[output_index++] = ' ';
                        total_len++;
                    }
                }
            }
            word_start = i + 1;
            word_count++;
        }
    }
    output[output_index] = '\0';
}
//Function to print a row of the table
__declspec(dllexport) void printTableRow(struct DataSiswa *data) {
    char formatname[50]; // Adjust size according to fullname size
    formatNameDisplay(data->fullname, formatname);
    printf("| %-9s | %-18s | %-5d | %-10d | %-6d | %-5d | %-5d | %-12d | %-9.2f | %-5c |\n",
           data->studentID, formatname, data->kelas, data->nilaiMat, data->nilaiFis,
           data->nilaiKim, data->nilaiBio, data->nilaiBindo, data->rata2, data->grade);
    printf("----------------------------------------------------------------------------------------------------------------------\n");
}
//------------------------------------------------------------------------------------------------------------------------------------------
//In-Order Traversal 
__declspec(dllexport) typedef struct {
    char studentID[100];
    char name[100];
    char kelas[10];
    int nilai_matematika;
    int nilai_fisika;
    int nilai_kimia;
    int nilai_biologi;
    int nilai_bahasa_indonesia;
    double rata_rata;
    char grade;
} Student;

__declspec(dllexport) void printTableRowArray(Student students[], int count) {
    char formatname[20];
    for (int i = 0; i < count; i++) {
        formatNameDisplay(students[i].name, formatname);
        printf("| %s | %-18s | %-5s | %-10d | %-6d | %-5d | %-7d | %-12d | %-9.2f | %-5c |\n", 
               students[i].studentID, 
               formatname, 
               students[i].kelas, 
               students[i].nilai_matematika, 
               students[i].nilai_fisika, 
               students[i].nilai_kimia, 
               students[i].nilai_biologi, 
               students[i].nilai_bahasa_indonesia, 
               students[i].rata_rata, 
               students[i].grade);
        printf("----------------------------------------------------------------------------------------------------------------------\n");
    }
    
}

__declspec(dllexport) void exportToCSV(Student students[], int count, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    // Write the CSV header
    fprintf(file, "ID,Nama,Kelas,Nilai Matematika,Nilai Fisika,Nilai Kimia,Nilai Biologi,Nilai Bahasa Indonesia,Rata-rata,Grade\n");

    // Write the student data
    for (int i = 0; i < count; i++) {
        fprintf(file, "%s,%s,%d,%d,%d,%d,%d,%s,%.2f,%c\n", 
                students[i].studentID, 
                students[i].name, 
                students[i].nilai_matematika, 
                students[i].nilai_fisika, 
                students[i].nilai_kimia, 
                students[i].nilai_biologi, 
                students[i].nilai_bahasa_indonesia, 
                students[i].kelas, 
                students[i].rata_rata, 
                students[i].grade);
    }
    fclose(file);
}

// Compare
__declspec(dllexport) int compareNameAsc(const void *a, const void *b) {
    return strcmp(((Student *)a)->name, ((Student *)b)->name);
}

__declspec(dllexport) int compareNameDesc(const void *a, const void *b) {
    return strcmp(((Student *)b)->name, ((Student *)a)->name);
}

__declspec(dllexport) int compareMathAsc(const void *a, const void *b) {
    float diff = ((Student *)a)->nilai_matematika - ((Student *)b)->nilai_matematika;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareMathDesc(const void *a, const void *b) {
    float diff = ((Student *)b)->nilai_matematika - ((Student *)a)->nilai_matematika;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int comparePhysicsAsc(const void *a, const void *b) {
    float diff = ((Student *)a)->nilai_fisika - ((Student *)b)->nilai_fisika;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int comparePhysicsDesc(const void *a, const void *b) {
    float diff = ((Student *)b)->nilai_fisika - ((Student *)a)->nilai_fisika;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareChemistryAsc(const void *a, const void *b) {
    float diff = ((Student *)a)->nilai_kimia - ((Student *)b)->nilai_kimia;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareChemistryDesc(const void *a, const void *b) {
    float diff = ((Student *)b)->nilai_kimia - ((Student *)a)->nilai_kimia;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareBiologyAsc(const void *a, const void *b) {
    float diff = ((Student *)a)->nilai_biologi - ((Student *)b)->nilai_biologi;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareBiologyDesc(const void *a, const void *b) {
    float diff = ((Student *)b)->nilai_biologi - ((Student *)a)->nilai_biologi;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareIndonesianAsc(const void *a, const void *b) {
    float diff = ((Student *)a)->nilai_bahasa_indonesia - ((Student *)b)->nilai_bahasa_indonesia;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareIndonesianDesc(const void *a, const void *b) {
    float diff = ((Student *)b)->nilai_bahasa_indonesia - ((Student *)a)->nilai_bahasa_indonesia;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareClassAsc(const void *a, const void *b) {
    return strcmp(((Student *)a)->kelas, ((Student *)b)->kelas);
}

__declspec(dllexport) int compareClassDesc(const void *a, const void *b) {
    return strcmp(((Student *)b)->kelas, ((Student *)a)->kelas);
}

__declspec(dllexport) int compareGradeAsc(const void *a, const void *b) {
    return strcmp(&((Student *)a)->grade, &((Student *)b)->grade);
}

__declspec(dllexport) int compareGradeDesc(const void *a, const void *b) {
    return strcmp(&((Student *)b)->grade, &((Student *)a)->grade);
}

__declspec(dllexport) int compareRataRataAsc(const void *a, const void *b) {
    float diff = ((Student *)a)->rata_rata - ((Student *)b)->rata_rata;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int compareRataRataDesc(const void *a, const void *b) {
    float diff = ((Student *)b)->rata_rata - ((Student *)a)->rata_rata;
    return (diff > 0) - (diff < 0);
}

__declspec(dllexport) int isValidNumber(char *input) {
    for (int i = 0; input[i] != '\0'; i++) {
        if (!isdigit(input[i])) {
            return 0;
        }
    }
    return 1;
}
//end compare function

//-------------------------------------------------------------------------------------------------------------------------------------------

__declspec(dllexport) void inorderTraversal(struct AVLSiswa *node) {
    if (node != NULL) {
        inorderTraversal(node->left);
        printTableRow(node->data);
        inorderTraversal(node->right);
    }
}

__declspec(dllexport) void searchByName(struct AVLSiswa *root, const char *nameSubstring, struct StudentArray *result) {
    if (root == NULL) return;

    // Check if the current node's full name contains the substring
    if (strstr(root->data->fullname, nameSubstring) != NULL) {
        result->students[result->count] = root->data;
        result->count++;
    }

    // Recursively search in left and right subtrees
    searchByName(root->left, nameSubstring, result);
    searchByName(root->right, nameSubstring, result);
}

__declspec(dllexport) void displaySearchResults(struct StudentArray *result) {
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    printf("| ID Siswa   | %-18s | Kelas | Matematika | Fisika | Kimia | Biologi | B. Indonesia | Rata-Rata | Grade |\n", "Nama Lengkap");
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    char formatname[50]; // Adjust size according to fullname size
    for (int i = 0; i < result->count; i++) {
        struct DataSiswa *student = result->students[i];
        formatNameDisplay(student->fullname, formatname);
        printf("| %-10s | %-18s | %-5d | %-10d | %-6d | %-5d | %-7d | %-12d | %-9.2f | %-5c |\n",
               student->studentID,
               formatname,
               student->kelas,
               student->nilaiMat,
               student->nilaiFis,
               student->nilaiKim,
               student->nilaiBio,
               student->nilaiBindo,
               student->rata2,
               student->grade);
        printf("----------------------------------------------------------------------------------------------------------------------\n");
    }
    printf("\n");
}
__declspec(dllexport) void searchByFullName(struct AVLSiswa *node) {
    char nameSubstring[100];
    printf("Enter the full name or part of the name to search: ");
    scanf(" %[^\n]", nameSubstring);

    struct StudentArray searchResults = { .count = 0 };
    searchByName(node, nameSubstring, &searchResults);

    if (searchResults.count > 0) {
        displaySearchResults(&searchResults);
    } else {
        printf("No students found with the name containing \"%s\".\n", nameSubstring);
    }
    printf("Press any key to continue...");
    getchar(); // Wait for user to press a key
}

__declspec(dllexport) int searchIDContaining(struct AVLSiswa *root, char *studentIDSubstring, struct DataSiswa **foundStudents, int *count) {
    if (root == NULL)
        return 0;

    int totalFound = 0;

    if (strstr(root->data->studentID, studentIDSubstring) != NULL) {
        foundStudents[*count] = root->data;
        (*count)++;
        totalFound++;
    }

    totalFound += searchIDContaining(root->left, studentIDSubstring, foundStudents, count);
    totalFound += searchIDContaining(root->right, studentIDSubstring, foundStudents, count);

    return totalFound;
}

// Function to search and display results
__declspec(dllexport) void searchByID(struct AVLSiswa *node) {
    char studentId[25];
    printf("Enter the student ID to search: ");
    scanf(" %[^\n]", studentId);

    // Allocate an array to store found students
    struct DataSiswa *foundStudents[MAX_STUDENTS];
    int count = 0;

    // Search for students and accumulate results
    int totalFound = searchIDContaining(node, studentId, foundStudents, &count);

    if (totalFound > 0) {
        printf("Students found:\n");
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        printf("| ID Siswa   | %-18s | Kelas | Matematika | Fisika | Kimia | Biologi | B. Indonesia | Rata-Rata | Grade |\n", "Nama Lengkap");
        printf("----------------------------------------------------------------------------------------------------------------------\n");
        for (int i = 0; i < count; i++) {
            char formatname[100]; // Adjust size according to fullname size
            formatNameDisplay(foundStudents[i]->fullname, formatname);
            printf("| %-10s | %-18s | %-5d | %-10d | %-6d | %-5d | %-7d | %-12d | %-9.2f | %-5c |\n",
                   foundStudents[i]->studentID,
                   formatname,
                   foundStudents[i]->kelas,
                   foundStudents[i]->nilaiMat,
                   foundStudents[i]->nilaiFis,
                   foundStudents[i]->nilaiKim,
                   foundStudents[i]->nilaiBio,
                   foundStudents[i]->nilaiBindo,
                   foundStudents[i]->rata2,
                   foundStudents[i]->grade);
            printf("----------------------------------------------------------------------------------------------------------------------\n");
        }
        
    } else {
        printf("No student found with ID containing \"%s\".\n", studentId);
    }

    printf("Press any key to continue...");
    getchar(); // Wait for user to press a key
}


__declspec(dllexport) void searchByClassHelper(struct AVLSiswa *node, int kelas, struct StudentArray *results) {
    if (node == NULL) {
        return;
    }
    if (node->data->kelas == kelas) {
        results->students[results->count++] = node->data;
    }
    searchByClassHelper(node->left, kelas, results);
    searchByClassHelper(node->right, kelas, results);
}
__declspec(dllexport) void searchByClass(struct AVLSiswa *node, int kelas) {
    struct StudentArray searchResults = { .count = 0 };
    searchByClassHelper(node, kelas, &searchResults);

    if (searchResults.count > 0) {
        displaySearchResults(&searchResults);
    } else {
        printf("No students found in class %d.\n", kelas);
    }
    printf("Press any key to continue...");
    getchar(); // Wait for user to press a key
}


__declspec(dllexport) void viewarr(Student students[], int count){
    system("cls");
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    printf("| ID Siswa   | %-18s | Kelas | Matematika | Fisika | Kimia | Biologi | B. Indonesia | Rata-Rata | Grade |\n", "Nama Lengkap");
    printf("----------------------------------------------------------------------------------------------------------------------\n");
    printTableRowArray(students, count);
    
    printf("\n");
}

__declspec(dllexport) void eksportarr(Student students[], int count, char namaDepanFile[]){
    const char *eksport = generateExportName(namaDepanFile);
    exportToCSV(students, count, eksport);
    animatePrint("Exporting students data...\n");
    printf("Success file exported in %s\n", eksport);
}

__declspec(dllexport) void sortNama(Student students[], int count) {    
    int pilih = 0;
    int pilih1 = 0;
    while(1){
        system("cls");
        printf("Sorting Berdasarkan Nama\n");
	    printf("=========================\n");
        printf("\n1. Ascending\n2. Descending\n");
        fflush(stdin);
        Scanfint(&pilih);
        fflush(stdin);
        if(pilih == 1 || pilih == 2){
            break;
        }
    }
    while(1){
        system("cls");
        printf("Sorting Berdasarkan Nama");
        if(pilih == 1){
            printf(" Ascending\n");
            qsort(students, count, sizeof(Student), compareNameAsc);
        }
        else if(pilih == 2){
            printf(" Descending\n"); 
            qsort(students, count, sizeof(Student), compareNameDesc);
        }
	    printf("=========================\n");
        printf("\n1. View\n2. Eksport\n");
        fflush(stdin);
        Scanfint(&pilih1);
        fflush(stdin);
        if(pilih1 == 1 || pilih1 == 2){
            break;
        }
    }
    if(pilih1 == 1){
        viewarr(students, count);
    }else{
        eksportarr(students, count,"ByName_");
    }
    ClearScreen();
}

__declspec(dllexport) void sortNilai(Student students[], int count) {
    int choice;
    int pilih = 0;
    int pilih1 = 0;

    while(1){
        printf("Sorting Berdasarkan Nilai\n");
	    printf("=========================\n");
        printf("\n1. Matematika\n2. Fisika\n3. Kimia\n4. Biologi\n5. B. Indonesia\n6. Back\nEnter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Matematika\n");
                    printf("=========================\n");
                    printf("\n1. Ascending\n2. Descending\n");
                    fflush(stdin);
                    Scanfint(&pilih);
                    fflush(stdin);
                    if(pilih == 1 || pilih == 2){
                        break;
                    }
                }
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Matematika");
                    if(pilih == 1){
                        printf(" Ascending\n");
                        qsort(students, count, sizeof(Student), compareMathAsc);
                    }
                    else if(pilih == 2){
                        printf(" Descending\n"); 
                        qsort(students, count, sizeof(Student), compareMathDesc);
                    }
                    printf("=========================\n");
                    printf("\n1. View\n2. Eksport\n");
                    fflush(stdin);
                    Scanfint(&pilih1);
                    fflush(stdin);
                    if(pilih1 == 1 || pilih1 == 2){
                        break;
                    }
                }
                if(pilih1 == 1){
                    viewarr(students, count);
                }else{
                    eksportarr(students, count,"ByMatematika_");
                }
                ClearScreen();
                break;
            case 2:
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Fisika\n");
                    printf("=========================\n");
                    printf("\n1. Ascending\n2. Descending\n");
                    fflush(stdin);
                    Scanfint(&pilih);
                    fflush(stdin);
                    if(pilih == 1 || pilih == 2){
                        break;
                    }
                }
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Fisika");
                    if(pilih == 1){
                        printf(" Ascending\n");
                        qsort(students, count, sizeof(Student), comparePhysicsAsc);
                    }
                    else if(pilih == 2){
                        printf(" Descending\n"); 
                        qsort(students, count, sizeof(Student), comparePhysicsAsc);
                    }
                    printf("=========================\n");
                    printf("\n1. View\n2. Eksport\n");
                    fflush(stdin);
                    Scanfint(&pilih1);
                    fflush(stdin);
                    if(pilih1 == 1 || pilih1 == 2){
                        break;
                    }
                }
                if(pilih1 == 1){
                    viewarr(students, count);
                }else{
                    eksportarr(students, count,"ByFisika_");
                }
                ClearScreen();
                break;
            case 3:
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Kimia\n");
                    printf("=========================\n");
                    printf("\n1. Ascending\n2. Descending\n");
                    fflush(stdin);
                    Scanfint(&pilih);
                    fflush(stdin);
                    if(pilih == 1 || pilih == 2){
                        break;
                    }
                }
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Kimia");
                    if(pilih == 1){
                        printf(" Ascending\n");
                        qsort(students, count, sizeof(Student), compareChemistryAsc);
                    }
                    else if(pilih == 2){
                        printf(" Descending\n"); 
                        qsort(students, count, sizeof(Student), compareChemistryDesc);
                    }
                    printf("=========================\n");
                    printf("\n1. View\n2. Eksport\n");
                    fflush(stdin);
                    Scanfint(&pilih1);
                    fflush(stdin);
                    if(pilih1 == 1 || pilih1 == 2){
                        break;
                    }
                }
                if(pilih1 == 1){
                    viewarr(students, count);
                }else{
                    eksportarr(students, count,"ByKimia_");
                }
                ClearScreen();
                break;
            case 4:
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Biologi\n");
                    printf("=========================\n");
                    printf("\n1. Ascending\n2. Descending\n");
                    fflush(stdin);
                    Scanfint(&pilih);
                    fflush(stdin);
                    if(pilih == 1 || pilih == 2){
                        break;
                    }
                }
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai Biologi");
                    if(pilih == 1){
                        printf(" Ascending\n");
                        qsort(students, count, sizeof(Student), compareBiologyAsc);
                    }
                    else if(pilih == 2){
                        printf(" Descending\n"); 
                        qsort(students, count, sizeof(Student), compareBiologyDesc);
                    }
                    printf("=========================\n");
                    printf("\n1. View\n2. Eksport\n");
                    fflush(stdin);
                    Scanfint(&pilih1);
                    fflush(stdin);
                    if(pilih1 == 1 || pilih1 == 2){
                        break;
                    }
                }
                if(pilih1 == 1){
                    viewarr(students, count);
                }else{
                    eksportarr(students, count,"ByBiologi_");
                }
                ClearScreen();
                break;
            case 5:
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai B. Indonesia\n");
                    printf("=========================\n");
                    printf("\n1. Ascending\n2. Descending\n");
                    fflush(stdin);
                    Scanfint(&pilih);
                    fflush(stdin);
                    if(pilih == 1 || pilih == 2){
                        break;
                    }
                }
                while(1){
                    system("cls");
                    printf("Sorting Berdasarkan Nilai B. Indonesia");
                    if(pilih == 1){
                        printf(" Ascending\n");
                        qsort(students, count, sizeof(Student), compareIndonesianAsc);
                    }
                    else if(pilih == 2){
                        printf(" Descending\n"); 
                        qsort(students, count, sizeof(Student), compareIndonesianDesc);
                    }
                    printf("=========================\n");
                    printf("\n1. View\n2. Eksport\n");
                    fflush(stdin);
                    Scanfint(&pilih1);
                    fflush(stdin);
                    if(pilih1 == 1 || pilih1 == 2){
                        break;
                    }
                }
                if(pilih1 == 1){
                    viewarr(students, count);
                }else{
                    eksportarr(students, count,"ByBindo_");
                }
                ClearScreen();                   
                break;
            case 6:
                return;
                break;
            default:
                printf("Invalid choice. Please enter the number again [1-6]!\n");
                ClearScreen();
        }
    }
}

__declspec(dllexport) void sortRanks(Student students[], int count) {
    int pilih = 0;
    int pilih1 = 0;
    while(1){
        system("cls");
        printf("Sorting Berdasarkan Rangking\n");
	    printf("=========================\n");
        printf("\n1. Ascending\n2. Descending\n");
        fflush(stdin);
        Scanfint(&pilih);
        fflush(stdin);
        if(pilih == 1 || pilih == 2){
            break;
        }
    }
    while(1){
        system("cls");
        printf("Sorting Berdasarkan Rangking");
        if(pilih == 1){
            printf(" Ascending\n");
            qsort(students, count, sizeof(Student), compareRataRataAsc);
        }
        else if(pilih == 2){
            printf(" Descending\n"); 
            qsort(students, count, sizeof(Student), compareRataRataDesc);
        }
	    printf("=========================\n");
        printf("\n1. View\n2. Eksport\n");
        fflush(stdin);
        Scanfint(&pilih1);
        fflush(stdin);
        if(pilih1 == 1 || pilih1 == 2){
            break;
        }
    }
    if(pilih1 == 1){
        viewarr(students, count);
    }else{
        eksportarr(students, count,"ByRank_");
    }
    ClearScreen();
}

__declspec(dllexport) void sortKelas(Student students[], int count) {
     int pilih = 0;
    int pilih1 = 0;
    while(1){
        system("cls");
        printf("Sorting Berdasarkan Kelas\n");
	    printf("=========================\n");
        printf("\n1. Ascending\n2. Descending\n");
        fflush(stdin);
        Scanfint(&pilih);
        fflush(stdin);
        if(pilih == 1 || pilih == 2){
            break;
        }
    }
    while(1){
        system("cls");
        printf("Sorting Berdasarkan Kelas");
        if(pilih == 1){
            printf(" Ascending\n");
            qsort(students, count, sizeof(Student), compareClassAsc);
        }
        else if(pilih == 2){
            printf(" Descending\n"); 
            qsort(students, count, sizeof(Student), compareClassDesc);
        }
	    printf("=========================\n");
        printf("\n1. View\n2. Eksport\n");
        fflush(stdin);
        Scanfint(&pilih1);
        fflush(stdin);
        if(pilih1 == 1 || pilih1 == 2){
            break;
        }
    }
    if(pilih1 == 1){
        viewarr(students, count);
    }else{
        eksportarr(students, count,"ByKelas_");
    }
    ClearScreen();
}

//------------------------------------------------------------------------------------------------
//baca file simpan ke Struct Student
// Read CSV
__declspec(dllexport) void readStudentsFromCSV(Student students[], int *count, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    char line[256];
    *count = 0;

    // Skip the header line
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        sscanf(line, "%99[^,],%99[^,],%d,%d,%d,%d,%d,%9[^,],%lf,%c",  
               students[*count].studentID, 
               students[*count].name, 
               &students[*count].nilai_matematika, 
               &students[*count].nilai_fisika, 
               &students[*count].nilai_kimia, 
               &students[*count].nilai_biologi, 
               &students[*count].nilai_bahasa_indonesia, 
               students[*count].kelas, 
               &students[*count].rata_rata, 
               &students[*count].grade);
        (*count)++;
    }

    fclose(file);
}

//Function to display all student using In-Order traversal
__declspec(dllexport) void displayStudents(struct AVLSiswa **node) {
    system("cls");
    if (*node == NULL) {
        printf("Data siswa belum diisi!\n\n");
        ClearScreen();
        return;
    }
    //--------------------------------------------------------------------
    Student students[MAX_STUDENTS];
    // Populate students from CSV
    int count = 0;
    readStudentsFromCSV(students, &count, "sma_students_data1.csv");

    if (count == 0) {
        printf("No students read from file or file not found.\n");
        return;
    }
    //--------------------------------------------------------------------
    int choice;
    while (1) {
        system("cls");
        printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        printf("|                    Welcome To Homepage                        |\n");
        printf("|                                                               |\n");
        printf("|             HIGH SCHOOL TEACHER ASSESSMENT SYSTEM             |\n");
        printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        printf("|                                                               |\n");
        printf("|           Tools Menu for Assessment:                          |\n");
        printf("|                                                               |\n");
        printf("|    1. Sorting Nama                                            |\n");
        printf("|    2. Sorting Nilai                                           |\n");
        printf("|    3. Sorting Ranking                                         |\n");
        printf("|    4. Sorting Kelas                                           |\n");
        printf("|    5. View All Student                                        |\n");
        printf("|    6. Search students                                         |\n");
        printf("|    7. Export Grades (Unsorted)                                |\n");
        printf("|    8. Back                                                    |\n");
        printf("|                                                               |\n");
        printf("/////////////////////////////////////////////////////////////////\n");
        printf("|\n");
        printf(" ===> Choose an Option from the Tools Menu [1-8]: ");
        // Clear input buffer
        if (scanf(" %d", &choice) != 1) {
            // Handle invalid input
            while (getchar() != '\n'); // Clear invalid input
            choice = -1; // Invalid choice to trigger default case
        }
        system("cls");
        switch (choice) {
            case 1:
                sortNama(students, count);
                break;
            case 2:
                sortNilai(students, count);
                break;
            case 3:
                sortRanks(students, count);
                break;
            case 4:
                sortKelas(students, count);
                break;
            case 5:
                inorderTraversal(*node);
                viewarr(students, count);
                printf("\n");
                getchar(); // Wait for user to press a key
                ClearScreen();
                break;
            case 6:
                ClearScreen();
                int pilih;
                printf("Search by: \n");
                printf("1. Full name \n");
                printf("2. ID \n");
                printf("3. Class \n");
                printf("Choose an option [1-3]: ");
                if (scanf("%d", &pilih) != 1) {
                    while (getchar() != '\n'); // Clear invalid input
                    pilih = -1; // Invalid choice to trigger default case
                }
                system("cls");
                switch (pilih) {
                    case 1:
                        searchByFullName(*node);
                        getchar();
                        break;
                    case 2:
                        searchByID(*node);
                        getchar();
                        break;
                    case 3:
                        printf("Enter the class to search: ");
                        int kelas;
                        scanf("%d", &kelas);
                        searchByClass(*node, kelas);
                        getchar();
                        break;
                    default:
                        printf("Invalid choice! Please enter a number between 1 and 3.\n\n");
                        ClearScreen();
                }
                break;
            case 7:
                
                animatePrint("Exporting students data...\n");
                const char *eksport = generateExportName("Unsorted_");
                writeStudentsToFile(*node, eksport);
                // Print success message
                printf("Success file exported in %s\n", eksport);
                printf("\nPress enter to  continue... ");
                getchar();
                getchar();
                system("cls");
                break;
            case 8:
                mainDatasiswa();
            default:
                printf("Invalid choice! Please enter a number between 1 and 8.\n\n");
                ClearScreen();
        }
    }
}




__declspec(dllexport) void updateStudent(struct AVLSiswa **node) {
    if (*node == NULL) {
        printf("There is no student in data, please input student data type [1] in homepage menu!\n");
        return;
    }

    char upfullname[100];
    int upkelas, upMat, upFis, upKim, upBio, upBindo;

    struct DataSiswa *update = NULL;

    while (1) {
        while (1) {
            printf("Masukan nama siswa: ");
            char nameSubstring[100];
            scanf(" %[^\n]", nameSubstring);

            struct StudentArray searchResults = { .count = 0 };
            searchByName(*node, nameSubstring, &searchResults);

            if (searchResults.count > 0) {
                displaySearchResults(&searchResults);
                break;
            } else {
                printf("No students found with the name containing \"%s\".\n", nameSubstring);
                printf("Press 1 to go back or 2 to try again: ");
                int choice;
                scanf("%d", &choice);

                if (choice == 1) {
                    return;
                } else if (choice != 2) {
                    printf("Invalid choice. Going back.\n");
                    return;
                }
            }
        }

        printf("Masukan Id Siswa: ");
        char studentId[25];
        scanf(" %s", studentId);
        update = searchID(*node, studentId);

        if (update != NULL) {
            break;
        } else {
            printf("Forget the student Id? Go to the view page to search the student id\nType [Y/y] if you want to go to the view page: ");
            char input;
            scanf(" %c", &input);

            if (input == 'y' || input == 'Y') {
                displayStudents(node);
            }
        }
    }

    int menu;
    while(1) {
        system("cls");
        char output[6]; // Array to store the first 4 characters + period + null terminator
		snprintf(output, 6, "%.4s.", update->fullname);
        printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        printf("|                      Welcome To Update                        |\n");
        printf("|                                                               |\n");
        printf("|             HIGH SCHOOL TEACHER ASSESSMENT SYSTEM             |\n");
        printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        printf("|                                                               |\n");
        printf("|           Menu for Update:                                    |\n");
        printf("|                                                               |\n");
        printf("|    1. Update nama lengkap\t\t(%s)\t\t\t|\n", output);
        printf("|    2. Update kelas\t\t\t(%d)\t\t\t|\n", update->kelas);
        printf("|    3. Update nilai matematika\t\t(%d)\t\t\t|\n", update->nilaiMat);
        printf("|    4. Update nilai fisika\t\t(%d)\t\t\t|\n", update->nilaiFis);
        printf("|    5. Update nilai kimia\t\t(%d)\t\t\t|\n", update->nilaiKim);
        printf("|    6. Update nilai biologi\t\t(%d)\t\t\t|\n", update->nilaiBio);
        printf("|    7. Update nilai B. Indonesia\t(%d)\t\t\t|\n", update->nilaiBindo);
        printf("|    8. Back To Main Menu                                       |\n");
        printf("|                                                               |\n");
        printf("/////////////////////////////////////////////////////////////////\n");
        printf("|\n");
        printf(" ===> Choose an Option from Menu [1-8]: ");
        //buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        Scanfint(&menu);
        if(menu >= 1 && menu <=8) {
            break;
        } else {
            printf("Invalid choice! Please enter a number between 1 and 6.\n\n");
            ClearScreen();
        }
    }

    switch(menu) {
        case 1: {
            while(1) {
                printf("Input fullname[2-90]: ");
                Scanfstr(upfullname);
                if(strlen(upfullname) < 2 || strlen(upfullname) > 90 || !isValidName(upfullname)) {
                    printf("Fullname must be alfabet between 2 and 90 characters!\n");
                    continue;
                } else {
                    formatname(upfullname);
                    strcpy(update->fullname, upfullname);
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 2: {
            while(1) {
                printf("Input the class [ 10 | 11 | 12 ]: ");
                Scanfint(&upkelas);
                if(upkelas != 10 && upkelas != 11 && upkelas != 12) {
                    printf("Error please Input the class with number 10 or 11 or 12!\n");
                    continue;
                } else {
                    update->kelas = upkelas;
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 3: {
            while(1) {
                printf("Input the mathematics score [0-100]: ");
                Scanfint(&upMat);
                if(upMat < 0 || upMat > 100) {
                    printf("Error please Input the mathematics score with number 0 to 100!\n");
                    continue;
                } else {
                    update->nilaiMat = upMat;
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 4: {
            while(1) {
                printf("Input the physics score [0-100]: ");
                Scanfint(&upFis);
                if(upFis < 0 || upFis > 100) {
                    printf("Error please Input the physics score with number 0 to 100!\n");
                    continue;
                } else {
                    update->nilaiFis = upFis;
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 5: {
            while(1) {
                printf("Input the chemistry score [0-100]: ");
                Scanfint(&upKim);
                if(upKim < 0 || upKim > 100) {
                    printf("Error please Input the chemistry score with number 0 to 100!\n");
                    continue;
                } else {
                    update->nilaiKim = upKim;
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 6: {
            while(1) {
                printf("Input the biology score [0-100]: ");
                Scanfint(&upBio);
                if(upBio < 0 || upBio > 100) {
                    printf("Error please Input the biology score with number 0 to 100!\n");
                    continue;
                } else {
                    update->nilaiBio = upBio;
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 7: {
            while(1) {
                printf("Input the language score [0-100]: ");
                Scanfint(&upBindo);
                if(upBindo < 0 || upBindo > 100) {
                    printf("Error please Input the language score with number 0 to 100!\n");
                    continue;
                } else {
                    update->nilaiBindo = upBindo;
                    printf("Data updated successfully !\n\n");
                    break;
                }
            }
            break;
        }
        case 8:
            return; // main menu
        default:
            printf("Invalid choice! Please enter a number between 1 and 8.\n\n");
            ClearScreen();
    }

    update->rata2 = (update->nilaiMat + update->nilaiFis + update->nilaiKim + update->nilaiBio + update->nilaiBindo) / 5.0;
    update->grade = generateGrade(update->rata2);
    const char *filename = "sma_students_data1.csv";
    writeStudentsToFile(*node, filename);
    printf("Type [Y|y] if you finished edit or update data, and want to see the updated data: ");
    char input2;
    scanf(" %c", &input2);
    //buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    if(input2 == 'y' || input2 == 'Y') {
        displayStudents(&(*node));
        return;
    }
}

__declspec(dllexport) void deleteStudent(struct AVLSiswa **node) {
    if (*node == NULL) {
        printf("There is no student in data, please input student data type [1] in homepage menu!\n");
        ClearScreen();
        return;
    }

    char name[100];
    struct DataSiswa *deleteStudent = NULL;
    char studentId[25];

    while (1) {
        while (1) {
            printf("Masukan nama siswa: ");
            char nameSubstring[100];
            scanf(" %[^\n]", nameSubstring);

            struct StudentArray searchResults = { .count = 0 };
            searchByName(*node, nameSubstring, &searchResults);

            if (searchResults.count > 0) {
                displaySearchResults(&searchResults);
                break;
            } else {
                printf("No students found with the name containing \"%s\".\n", nameSubstring);
                printf("Press 1 to go back or 2 to try again: ");
                int choice;
                scanf("%d", &choice);

                if (choice == 1) {
                    return;
                } else if (choice != 2) {
                    printf("Invalid choice. Going back.\n");
                    return;
                }
            }
        }

        printf("Masukan Id Siswa yang ingin di hapus datanya: ");
        Scanfstr(studentId);
        deleteStudent = searchID(*node, studentId);

        if (deleteStudent != NULL) {
            strcpy(name, deleteStudent->fullname);
            // buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            *node = deleteDataSiswa(*node, studentId);
            const char *filename = "sma_students_data1.csv";
            writeStudentsToFile(*node, filename);
            printf("The data of the student with the name %s is deleted!\n", name);
            ClearScreen();
            break;
        } else {
            printf("Forget the student Id? Go to the view page to search the student id and copy the id\nType [Y/y] if you want to go to the view page: ");
            char input;
            scanf(" %c", &input);
            // buffer
            int c;
            while ((c = getchar()) != '\n' && c != EOF);

            if (input == 'y' || input == 'Y') {
                displayStudents(node);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------
// Fungsi untuk memuat data siswa dari file CSV ke AVL tree

__declspec(dllexport) struct AVLSiswa* loadStudentsFromFile(struct AVLSiswa *root, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        ClearScreen();
        return root;
    }
    int i = 0;
    char line[256];
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {
        struct DataSiswa *newDataf =  (struct DataSiswa *)malloc(sizeof(struct DataSiswa));
        char *studentID = strtok(line, ",");
        char *nama = strtok(NULL, ",");
        int nilaiMat = atoi(strtok(NULL, ","));
        int nilaiFis = atoi(strtok(NULL, ","));
        int nilaiKim = atoi(strtok(NULL, ","));
        int nilaiBio = atoi(strtok(NULL, ","));
        int nilaiBindo = atoi(strtok(NULL, ","));
        int kelas = atoi(strtok(NULL, ","));
        double rata2 = strtod(strtok(NULL, ","), NULL);
        char *grade_str = strtok(NULL, "\n");
        char grade = grade_str[0];
        if (studentID && nama && nilaiMat && nilaiFis && nilaiKim && nilaiBio && nilaiBindo && rata2 && grade) {
            strcpy(newDataf->studentID,studentID);
            strcpy(newDataf->fullname,nama);
            newDataf->nilaiMat = nilaiMat;
            newDataf->nilaiFis = nilaiFis;
            newDataf->nilaiKim = nilaiKim;
            newDataf->nilaiBio = nilaiBio;
            newDataf->nilaiBindo = nilaiBindo;
            newDataf->kelas = kelas;
            newDataf->rata2 = rata2;
            newDataf->grade = grade;
            root = insertDataSiswa(root, newDataf);
            i++;
        }
    }
    fclose(file);
    return root;
}
//mengupdate data file
__declspec(dllexport) void writeStudentToFile(FILE *file, struct DataSiswa *student) {
    fprintf(file, "%s,%s,%d,%d,%d,%d,%d,%d,%.2f,%c\n",
            student->studentID,
            student->fullname,
            student->nilaiMat,
            student->nilaiFis,
            student->nilaiKim,
            student->nilaiBio,
            student->nilaiBindo,
            student->kelas,
            student->rata2,
            student->grade);
}
__declspec(dllexport) void writeStudentsInOrder(struct AVLSiswa *root, FILE *file) {
    if (root == NULL) return;

    writeStudentsInOrder(root->left, file);
    writeStudentToFile(file, root->data);
    writeStudentsInOrder(root->right, file);
}
__declspec(dllexport) void writeStudentsToFile(struct AVLSiswa *root, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file %s\n", filename);
        return;
    }

    // Tulis header CSV
    fprintf(file, "ID,Nama,Nilai Matematika,Nilai Fisika,Nilai Kimia,Nilai Biologi,Nilai Bahasa Indonesia,Kelas,Rata-rata,Grade\n");

    // Tulis data siswa secara inorder traversal
    writeStudentsInOrder(root, file);

    fclose(file);
}

// Fungsi untuk menghasilkan string eksport dengan angka unik
__declspec(dllexport) const char* generateExportName(const char* filename) {
    // Seed generator angka acak berdasarkan waktu saat ini
    srand(time(NULL));
    
    // Menghasilkan 9 angka acak antara 100000000 dan 999999999
    int unique_number = rand() % (999999999 - 100000000 + 1) + 100000000;
    
    // Menghitung panjang string yang diperlukan untuk menyimpan unique_number
    int digits = snprintf(NULL, 0, "%d", unique_number);
    
    // Menghitung panjang total string eksport (Unsorted_ + filename + angka unik)
    int total_length = 9 + strlen(filename) + digits + 1;
    
    // Mengalokasikan memori untuk menyimpan string eksport
    char *eksport = (char *)malloc(sizeof(char) * total_length);
    
    // Mengisi string eksport dengan format "Unsorted_<filename><unique_number>"
    snprintf(eksport, total_length, "%s%d.csv", filename, unique_number);
    
    return eksport;
}

//----------------------------------------------------------------------------------------------------------------
//-------------------------------------------MAIN FUNCTION--------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
 //void mainDatasiswa()
__declspec(dllexport) void mainDatasiswa(){

    int choice;
    struct AVLSiswa *node = NULL;
    const char *filename = "sma_students_data1.csv";
    node = loadStudentsFromFile(node, filename);
    do {
        system("cls");
        printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        printf("|                    Welcome To Homepage                        |\n");
        printf("|                                                               |\n");
        printf("|             HIGH SCHOOL TEACHER ASSESSMENT SYSTEM             |\n");
        printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
        printf("|                                                               |\n");
        printf("|           Tools Menu for Assessment:                          |\n");
        printf("|                                                               |\n");
        printf("|    1. Insert Student Information and Grades                   |\n");
        printf("|    2. Sort, View, Search, and Export Student Grades           |\n");
        printf("|    3. Edit Student Information and Grades                     |\n");
        printf("|    4. Delete Student Data                                     |\n");
        printf("|    5. Export Student Data (Unsorted)                          |\n");
        printf("|    6. Log out                                                 |\n");
        printf("|                                                               |\n");
        printf("/////////////////////////////////////////////////////////////////\n");
        printf("|\n");
        printf(" ===> Choose an Option from the Tools Menu [1-6]: ");

        scanf("%d", &choice);
        // Clear input buffer
        while (getchar() != '\n')
			;
		system("cls");

        switch (choice) {
            case 1:
                insertData(&node);
                break;
            case 2:
                displayStudents(&node);
                break;
            case 3:
                updateStudent(&node);
                break;
            case 4:
                deleteStudent(&node);
                break;
            case 5:
                
                animatePrint("Exporting students data...\n");
                const char *eksport = generateExportName("Unsorted_");
                writeStudentsToFile(node, eksport);
                printf("Success file exported in %s\n", eksport);
                ClearScreen();
                break;
            case 6:
                animatePrint("Log out...\n");
                ClearScreen();
                return;
            default:
                printf("Invalid choice! Please enter a number between 1 and 6.\n\n");
                ClearScreen();
        }

    } while (choice != 6);
 }


// Main function
__declspec(dllexport) int main() {
    struct AVLNode *root = NULL;
    int choice;
    mainDatasiswa();
    do {
        printf("Login & Register Page\n");
	    printf("======================\n");
        printf("\n1. Login\n2. Register\n3. Exit\nEnter your choice: ");
        scanf("%d", &choice);

        // Clear input buffer
        while (getchar() != '\n')
			;
		system("cls");

        switch (choice) {
            case 1:
                loginPage(root);
                
                break;
            case 2:
                registerPage(&root);
                break;
            case 3:
                animatePrint("Exiting...\n");
                ClearScreen();
                exit(0);
            default:
                printf("Invalid choice. Please enter again.\n");
                ClearScreen();
        }
    } while (choice != 3);

    return 0;
}
