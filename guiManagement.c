#include "raylib.h"
#include "cjson/cJSON.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_STUDENTS 100
#define MAX_NAME_LENGTH 50
#define MAX_COURSE_LENGTH 50
#define FILENAME "students.json"

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char course[MAX_COURSE_LENGTH];
    float gpa;
} Student;

Student students[MAX_STUDENTS];
int studentCount = 0;

// Function prototypes
void DrawButton(Rectangle bounds, const char *text, Color color);
bool IsButtonPressed(Rectangle bounds);
void DrawInputField(Rectangle bounds, char *text, int *letterCount, int maxLetters, bool *focused);
void AddStudent(const char *name, const char *course, float gpa);
void DeleteStudent(int index);
void SaveStudents(const char *filename);
void LoadStudents(const char *filename);

int main() {
    InitWindow(800, 600, "Student Management System");
    SetTargetFPS(60);

    char nameInput[MAX_NAME_LENGTH] = "";
    char courseInput[MAX_COURSE_LENGTH] = "";
    char gpaInput[10] = "";
    int nameLetterCount = 0;
    int courseLetterCount = 0;
    int gpaLetterCount = 0;

    bool nameFocused = false;
    bool courseFocused = false;
    bool gpaFocused = false;

    Rectangle addButton = {20, 220, 150, 30};
    Rectangle deleteButton = {180, 220, 150, 30};
    Rectangle saveButton = {340, 220, 150, 30};
    Rectangle loadButton = {500, 220, 150, 30};

    LoadStudents(FILENAME); // Load students from file at startup

    while (!WindowShouldClose()) {
        // Handle input field focus
        Vector2 mousePos = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            nameFocused = CheckCollisionPointRec(mousePos, (Rectangle){20, 50, 300, 30});
            courseFocused = CheckCollisionPointRec(mousePos, (Rectangle){20, 120, 300, 30});
            gpaFocused = CheckCollisionPointRec(mousePos, (Rectangle){20, 190, 100, 30});
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw input fields
        DrawText("Name:", 20, 20, 20, BLACK);
        DrawInputField((Rectangle){20, 50, 300, 30}, nameInput, &nameLetterCount, MAX_NAME_LENGTH, &nameFocused);

        DrawText("Course:", 20, 90, 20, BLACK);
        DrawInputField((Rectangle){20, 120, 300, 30}, courseInput, &courseLetterCount, MAX_COURSE_LENGTH, &courseFocused);

        DrawText("GPA:", 20, 160, 20, BLACK);
        DrawInputField((Rectangle){20, 190, 100, 30}, gpaInput, &gpaLetterCount, 5, &gpaFocused);

        // Draw buttons
        DrawButton(addButton, "Add Student", GREEN);
        DrawButton(deleteButton, "Delete Student", RED);
        DrawButton(saveButton, "Save Students", BLUE);
        DrawButton(loadButton, "Load Students", DARKGRAY);

        // Handle button clicks
        if (IsButtonPressed(addButton)) {
            // Check if any input field is empty
            if (nameLetterCount == 0 || courseLetterCount == 0 || gpaLetterCount == 0) {
                DrawText("Please fill all fields!", 20, 250, 20, RED);
            } else {
                float gpa = atof(gpaInput);
                AddStudent(nameInput, courseInput, gpa);
                memset(nameInput, 0, sizeof(nameInput));
                memset(courseInput, 0, sizeof(courseInput));
                memset(gpaInput, 0, sizeof(gpaInput));
                nameLetterCount = courseLetterCount = gpaLetterCount = 0;
                nameFocused = courseFocused = gpaFocused = false; // Reset focus after adding
            }
        }

        if (IsButtonPressed(deleteButton) && studentCount > 0) {
            DeleteStudent(studentCount - 1);
        }

        if (IsButtonPressed(saveButton)) {
            SaveStudents(FILENAME);
        }

        if (IsButtonPressed(loadButton)) {
            LoadStudents(FILENAME);
        }

        // Display student list
        DrawText("Student List:", 20, 270, 20, BLACK);
        for (int i = 0; i < studentCount; i++) {
            char studentInfo[100];
            sprintf(studentInfo, "%d. %s - %s (GPA: %.2f)", students[i].id, students[i].name, students[i].course, students[i].gpa);
            DrawText(studentInfo, 20, 300 + i * 30, 20, BLACK);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void DrawButton(Rectangle bounds, const char *text, Color color) {
    DrawRectangleRec(bounds, color);
    DrawText(text, bounds.x + 10, bounds.y + 5, 20, BLACK);
}

bool IsButtonPressed(Rectangle bounds) {
    return CheckCollisionPointRec(GetMousePosition(), bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void DrawInputField(Rectangle bounds, char *text, int *letterCount, int maxLetters, bool *focused) {
    Color color = *focused ? GRAY : (Color){200, 200, 200, 255}; // Change color if focused
    DrawRectangleRec(bounds, color);
    DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, DARKGRAY);

    // Check if the input field is focused
    if (*focused) {
        if (*letterCount < maxLetters) {
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125)) {
                    text[*letterCount] = (char)key;
                    (*letterCount)++;
                }
                key = GetCharPressed();
            }
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            (*letterCount)--; 
            if (*letterCount < 0) *letterCount = 0; 
            text[*letterCount] = '\0'; 
        }
    }

    DrawText(text, bounds.x + 5, bounds.y + 8, 20, MAROON);
}

void AddStudent(const char *name, const char *course, float gpa) {
    if (studentCount < MAX_STUDENTS) {
        Student newStudent;
        newStudent.id = studentCount + 1;
        strcpy(newStudent.name, name);
        strcpy(newStudent.course, course);
        newStudent.gpa = gpa;
        students[studentCount++] = newStudent;
    }
}

void DeleteStudent(int index) {
    if (index >= 0 && index < studentCount) {
        for (int i = index; i < studentCount - 1; i++) {
            students[i] = students[i + 1];
            students[i].id = i + 1;
        }
        studentCount--;
    }
}

void SaveStudents(const char *filename) {
    cJSON *studentsJson = cJSON_CreateArray();

    for (int i = 0; i < studentCount; i++) {
        cJSON *studentJson = cJSON_CreateObject();
        cJSON_AddNumberToObject(studentJson, "id", students[i].id);
        cJSON_AddStringToObject(studentJson, "name", students[i].name);
        cJSON_AddStringToObject(studentJson, "course", students[i].course);
        cJSON_AddNumberToObject(studentJson, "gpa", students[i].gpa);
        cJSON_AddItemToArray(studentsJson, studentJson);
    }

    FILE *file = fopen(filename, "w");
    if (file) {
        char *jsonString = cJSON_Print(studentsJson);
        fprintf(file, "%s", jsonString);
        free(jsonString);
        fclose(file);
    }
    cJSON_Delete(studentsJson);
}

void LoadStudents(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fseek(file, 0, SEEK_END);
        long length = ftell(file);
        fseek(file, 0, SEEK_SET);

        char *jsonString = (char *)malloc(length + 1);
        fread(jsonString, 1, length, file);
        jsonString[length] = '\0';
        fclose(file);

        cJSON *studentsJson = cJSON_Parse(jsonString);
        if (studentsJson) {
            studentCount = 0; // Reset student count before loading
            int arraySize = cJSON_GetArraySize(studentsJson);
            for (int i = 0; i < arraySize && studentCount < MAX_STUDENTS; i++) {
                cJSON *studentJson = cJSON_GetArrayItem(studentsJson, i);
                if (studentJson) {
                    Student newStudent;
                    newStudent.id = cJSON_GetObjectItem(studentJson, "id")->valueint;
                    strcpy(newStudent.name, cJSON_GetObjectItem(studentJson, "name")->valuestring);
                    strcpy(newStudent.course, cJSON_GetObjectItem(studentJson, "course")->valuestring);
                    newStudent.gpa = (float)cJSON_GetObjectItem(studentJson, "gpa")->valuedouble;
                    students[studentCount++] = newStudent;
                }
            }
            cJSON_Delete(studentsJson);
        }
        free(jsonString);
    }
}
