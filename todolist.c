#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LENGTH 100
#define MAX_TASK_LENGTH 200
#define INITIAL_CAPACITY 10
#define DATE_FORMAT "%Y-%m-%d"

typedef struct {
    char name[MAX_NAME_LENGTH];
    char description[MAX_TASK_LENGTH];
    time_t deadline;
    int daysLeft;
    int isDone;
} Task;

typedef struct {
    Task** tasks;
    int count;
    int capacity;
} TaskList;

// Function declarations
TaskList* initializeTaskList();
void freeTaskList(TaskList* list);
time_t getDateFromUser();
void createTask(TaskList* list);
void displayTasks(const TaskList* list);
void toggleTask(TaskList* list);
void saveToFile(const TaskList* list, const char* filename);
TaskList* loadFromFile(const char* filename);
void calculateDaysLeft(Task* task);
void clearInputBuffer();

// Initialize task list
TaskList* initializeTaskList() {
    TaskList* list = malloc(sizeof(TaskList));
    if (!list) {
        fprintf(stderr, "Memory allocation failed for TaskList\n");
        exit(1);
    }
    
    list->tasks = malloc(INITIAL_CAPACITY * sizeof(Task*));
    if (!list->tasks) {
        fprintf(stderr, "Memory allocation failed for tasks array\n");
        free(list);
        exit(1);
    }
    
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    return list;
}

// Free memory
void freeTaskList(TaskList* list) {
    if (!list) return;
    
    for (int i = 0; i < list->count; i++) {
        free(list->tasks[i]);
    }
    free(list->tasks);
    free(list);
}

time_t getDateFromUser() {
    int year, month, day;
    char input[20];  // Increased buffer size for safety
    struct tm date = {0};
    
    while (1) {
        printf("Enter deadline (YYYY-MM-DD): ");
        if (fgets(input, sizeof(input), stdin)) {
            // Remove newline if present
            input[strcspn(input, "\n")] = 0;
            
            // Try to parse the date
            if (sscanf(input, "%d-%d-%d", &year, &month, &day) == 3) {
                // Validate date components
                if (month < 1 || month > 12) {
                    printf("Invalid month. Please enter a month between 1 and 12.\n");
                    continue;
                }
                if (day < 1 || day > 31) {
                    printf("Invalid day. Please enter a day between 1 and 31.\n");
                    continue;
                }
                
                // Set up the tm structure
                date.tm_year = year - 1900;
                date.tm_mon = month - 1;
                date.tm_mday = day;
                date.tm_hour = 0;
                date.tm_min = 0;
                date.tm_sec = 0;
                
                // Validate the date by trying to make a timestamp
                time_t timestamp = mktime(&date);
                if (timestamp != -1) {
                    return timestamp;
                }
            }
            printf("Invalid date format. Please use YYYY-MM-DD (e.g., 2024-12-09)\n");
        }
    }
}

// Calculate days left
void calculateDaysLeft(Task* task) {
    time_t now = time(NULL);
    double diff = difftime(task->deadline, now);
    task->daysLeft = (int)(diff / (24 * 60 * 60));
}

// Create new task
void createTask(TaskList* list) {
    if (list->count == list->capacity) {
        list->capacity *= 2;
        Task** temp = realloc(list->tasks, list->capacity * sizeof(Task*));
        if (!temp) {
            fprintf(stderr, "Memory reallocation failed\n");
            return;
        }
        list->tasks = temp;
    }
    
    Task* newTask = malloc(sizeof(Task));
    if (!newTask) {
        fprintf(stderr, "Memory allocation failed for new task\n");
        return;
    }
    
    clearInputBuffer();
    printf("Enter task name: ");
    fgets(newTask->name, MAX_NAME_LENGTH, stdin);
    newTask->name[strcspn(newTask->name, "\n")] = 0;
    
    printf("Enter task description: ");
    fgets(newTask->description, MAX_TASK_LENGTH, stdin);
    newTask->description[strcspn(newTask->description, "\n")] = 0;
    
    newTask->deadline = getDateFromUser();
    newTask->isDone = 0;
    calculateDaysLeft(newTask);
    
    list->tasks[list->count++] = newTask;
    printf("Task created successfully!\n");
}

// Display tasks
void displayTasks(const TaskList* list) {
    if (list->count == 0) {
        printf("No tasks available.\n");
        return;
    }
    
    printf("\n=== Tasks List ===\n");
    for (int i = 0; i < list->count; i++) {
        Task* task = list->tasks[i];
        char dateStr[11];
        struct tm* tm_info = localtime(&task->deadline);
        strftime(dateStr, sizeof(dateStr), DATE_FORMAT, tm_info);
        
        printf("\nTask %d:\n", i + 1);
        printf("Name: %s\n", task->name);
        printf("Description: %s\n", task->description);
        printf("Deadline: %s\n", dateStr);
        printf("Days Left: %d\n", task->daysLeft);
        printf("Status: %s\n", task->isDone ? "Complete" : "Pending");
        printf("---------------\n");
    }
}

// Toggle task completion status
void toggleTask(TaskList* list) {
    if (list->count == 0) {
        printf("No tasks available to toggle.\n");
        return;
    }
    
    int index;
    printf("Enter task number (1-%d): ", list->count);
    if (scanf("%d", &index) == 1 && index > 0 && index <= list->count) {
        list->tasks[index-1]->isDone = !list->tasks[index-1]->isDone;
        printf("Task %d marked as %s\n", index, 
               list->tasks[index-1]->isDone ? "complete" : "pending");
    } else {
        printf("Invalid task number.\n");
        clearInputBuffer();
    }
}

// Save tasks to file
void saveToFile(const TaskList* list, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error opening file for writing.\n");
        return;
    }
    
    fprintf(file, "Total Tasks: %d\n", list->count);
    for (int i = 0; i < list->count; i++) {
        Task* task = list->tasks[i];
        fprintf(file, "Task %d\n", i + 1);
        fprintf(file, "Task name: %s\n", task->name);
        fprintf(file, "Task info: %s\n", task->description);
        fprintf(file, "Task deadline: %lld\n", (long long)task->deadline);  // Changed to long long
        fprintf(file, "Task isDone: %d\n", task->isDone);
    }
    
    fclose(file);
    printf("Tasks saved successfully!\n");
}

// Load tasks from file
TaskList* loadFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error opening file for reading.\n");
        return NULL;
    }
    
    TaskList* list = initializeTaskList();
    char line[256];
    int totalTasks = 0;
    
    // Read total tasks
    if (fgets(line, sizeof(line), file)) {
        sscanf(line, "Total Tasks: %d", &totalTasks);
    }
    
    // Read each task
    for (int i = 0; i < totalTasks; i++) {
        Task* task = malloc(sizeof(Task));
        if (!task) {
            fprintf(stderr, "Memory allocation failed while loading task\n");
            freeTaskList(list);
            fclose(file);
            return NULL;
        }
        
        // Initialize task with default values
        memset(task, 0, sizeof(Task));
        
        fgets(line, sizeof(line), file); // Task number
        
        // Read task name
        if (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            sscanf(line, "Task name: %[^\n]", task->name);
        }
        
        // Read task info
        if (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = 0;
            sscanf(line, "Task info: %[^\n]", task->description);
        }
        
        // Read deadline
        long long timestamp;  // Use long long for timestamp
        if (fgets(line, sizeof(line), file)) {
            if (sscanf(line, "Task deadline: %lld", &timestamp) == 1) {
                task->deadline = (time_t)timestamp;
            }
        }
        
        // Read isDone
        if (fgets(line, sizeof(line), file)) {
            sscanf(line, "Task isDone: %d", &task->isDone);
        }
        
        calculateDaysLeft(task);  // Update days left
        list->tasks[list->count++] = task;
    }
    
    fclose(file);
    printf("Tasks loaded successfully!\n");
    return list;
}
// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main() {
    TaskList* taskList = initializeTaskList();
    char command[20];
    
    printf("Welcome to TODO List Manager\n");
    
    while (1) {
        printf("\nAvailable commands:\n");
        printf("create  - Create a new task\n");
        printf("toggle  - Toggle task completion\n");
        printf("display - Show all tasks\n");
        printf("save    - Save tasks to file\n");
        printf("load    - Load tasks from file\n");
        printf("exit    - Exit program\n");
        printf("\nEnter command: ");
        
        scanf("%19s", command);
        clearInputBuffer();
        
        if (strcmp(command, "create") == 0) {
            createTask(taskList);
        }
        else if (strcmp(command, "toggle") == 0) {
            toggleTask(taskList);
        }
        else if (strcmp(command, "display") == 0) {
            displayTasks(taskList);
        }
        else if (strcmp(command, "save") == 0) {
            saveToFile(taskList, "listdata.txt");
        }
        else if (strcmp(command, "load") == 0) {
            TaskList* newList = loadFromFile("listdata.txt");
            if (newList) {
                freeTaskList(taskList);
                taskList = newList;
            }
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
        else {
            printf("Invalid command. Please try again.\n");
        }
    }
    
    freeTaskList(taskList);
    printf("Goodbye!\n");
    return 0;
}