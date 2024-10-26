#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int studentCount = 0;


struct Student{
char name[50]; 
int score;
int ID;
};

struct Student* students;

void saveContent(){
    FILE *fptr;
    fptr = fopen("student_data.txt", "w");
    if (fptr != NULL){
    fprintf(fptr, "Total Students: %d\n", studentCount);
    for(int i = 0; i < studentCount; i++){
        fprintf(fptr ,"Student:%d, Name:%s, Score:%d, ID:%d\n", i, students[i].name, students[i].score, students[i].ID);
    }
    fclose(fptr);       
    } else {
        printf("Error getting a handle to save file.\n");
    }


}




void searchStudents(){
//add search feature that'll loop through the student array and their elements.
}

struct Student* reallocate_student_array(struct Student* array, int elementcount) {
    if (elementcount == 0) {
        free(array);
        return NULL;
    }

    struct Student* new_array = (struct Student*)realloc(array, elementcount * sizeof(struct Student));
    if (new_array == NULL) {

        fprintf(stderr, "Memory reallocation failed\n");
        return array; 
    }
    return new_array;
}

void createStudent(char name[50], int score, int ID) {
    struct Student* temp = reallocate_student_array(students, studentCount + 1);
    if (temp != NULL) {
        students = temp;
        
        strcpy(students[studentCount].name, name);
        students[studentCount].score = score;
        students[studentCount].ID = ID;
        
        studentCount++;
    }
}

void loadContent() {
    FILE* file = fopen("student_data.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
    }


    int total;
    if (fscanf(file, "Total Students: %d\n", &total) != 1) {
        printf("Error reading total students count!\n");
        fclose(file);
    }


    struct Student* temp = reallocate_student_array(students, total);
    if (temp == NULL) {
        printf("Failed to allocate memory for loaded students!\n");
        fclose(file);
    }
    students = temp;


    studentCount = 0;


    char line[200];  
    while (fgets(line, sizeof(line), file) && studentCount < total) {
        char name[50];
        int score, id, student_num;
        

        if (sscanf(line, "Student:%d, Name:%[^,], Score:%d, ID:%d",
                   &student_num, name, &score, &id) == 4) {
            

            strcpy(students[studentCount].name, name);
            students[studentCount].score = score;
            students[studentCount].ID = id;
            studentCount++;
        }
    }

    printf("Successfully loaded %d students from file.\n", studentCount);
    fclose(file);
}

void checkInputs(char userinput[50]){
    char student_name[50];
    int student_ID;
    int student_score;
    if(strcmp(userinput, "create") == 0){
        printf("Enter the student's name: ");
        scanf("%s", &student_name);
        printf("Enter the student's ID: ");
        scanf("%d", &student_ID);
        printf("Enter the student's score: ");
        scanf("%d", &student_score);
        createStudent(student_name, student_score, student_ID);
    }
    if(strcmp(userinput, "display") == 0){
        for(int i = 0; i < studentCount; i++){
            printf("Student%d, Name: %s, Score: %d, ID: %d\n", i, students[i].name, students[i].score, students[i].ID);
        }
    }
    if(strcmp(userinput, "save") == 0){
        saveContent();
    }
    if(strcmp(userinput, "load") == 0){
        loadContent();
    }
    if(strcmp(userinput, "search") == 0){
        
    }     
    if(strcmp(userinput, "exit") == 0){
        printf("Exiting the program.\n");
        exit(0);
    }    
}



int main(){
int loop = 1;
char userinput[50];

struct Student* students = (struct Student*)malloc(1 * sizeof(struct Student));

printf("Welcome to Student Management Program!\n");
while(loop){
printf("create  | creates a new student\n");
printf("display | displays students\n");
printf("save    | saves student data to a file\n");
printf("load    | loads data from a file\n");
printf("search  | search the current database\n");
printf("exit    | exits the program\n");

scanf("%s", &userinput);
checkInputs(userinput);
}

return 0;
}