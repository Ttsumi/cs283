#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
int get_str_len(char *);
int get_str_len_excluding_whitespace(char *);
void reverse_string(char *, int);
int word_print(char *, int);
void search_and_replace(char *, int, int);

// Copies the user string to the buffer and removes any extra white space
// Fills the empty slots in the buffer with dots
// Returns the length of the string
// If the provided buffer or string is Null or the length of the buffer is <=0, then it returns -2
// If the provider user string is greater than the buffer, it returns -1
int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions

    if (len <= 0 || buff == NULL || user_str == NULL){
        return -2;
    }

    int user_str_len = get_str_len(user_str);
    int user_str_len_exclude_extra_space = get_str_len_excluding_whitespace(user_str);

    if (user_str_len_exclude_extra_space > len){
        return -1;
    }

    int curr_index = 0;                 // to track length of the string without extra spaces
    bool prev_was_space = false;        // to help eliminate extra spaces

    //To ignore leading whitespaces
    int start = 0;
    while (user_str[start] == ' ' || user_str[start] == '\t'){
        start++;
    }

    for (int i=start; user_str[i] != '\0'; i++){
        if (user_str[i] != ' ' && user_str[i] != '\t'){
            buff[curr_index++] = user_str[i];
            prev_was_space = false;
        }
        else{
            if (!prev_was_space){
                buff[curr_index++] = ' ';
                prev_was_space = true;
            }
        }
    }

    // If there's an extra space at the end, remove it
    if (buff[curr_index-1] == ' '){
        buff[curr_index-1] = '.';
    }
    
    // Fill any remaining empty buffer slots with dots
    int c = curr_index;
    while(c < len){
        buff[c++] = '.';
    }

    return curr_index;
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

//Loops through the buffer and counts the number of words excluding whitespaces or dots appended at the end of the buffer
int count_words(char *buff, int len, int str_len){
    int word_count = 0;
    bool inWord = false;
    for( int i=0; i < str_len; i++){
        if (buff[i] != ' ' && buff[i] != '.' && !inWord){
            word_count++;
            inWord = true;
        }

        if ( buff[i] == ' '){
            inWord = false;
        }
    }
    return word_count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

//loops through the user provided string and counts the length
int get_str_len(char *user_str){
    int len = 0;
    for (int i=0; user_str[i] != '\0'; i++ ){
        len++;
    }
    return len;
}


//Counts the number of non whitespace characters in the user string
int get_str_len_excluding_whitespace(char *user_str){
    int len = 0;
    for (int i=0; user_str[i] != '\0'; i++ ){
        if(user_str[i] != ' ' && user_str[i] != '\t'){
            len++;
        }
    }
    return len;
}


//Reverses the given string in place
//Two pointers are used to swap the starting character with the end and so on
void reverse_string(char *buff, int str_len){
    //printf("Reversed String: ");
    int from = 0;
    int to = str_len - 1;
    char temp;
    while (from < to){
        temp = buff[from];
        buff[from] = buff[to];
        buff[to] = temp;
        from++;
        to--;
    }
    /*
    for(int i=0; i < str_len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');*/
}

//Loops through the buffer and prints out all the words along with the character count for each word
int word_print(char *buff, int str_len){
    printf("Word Print\n");
    printf("----------\n");
    bool prev_was_space = false;
    bool in_word = false;
    int word_count = 0;
    int char_count = 0;
    for(int i=0; i < str_len; i++){
        if (buff[i] != ' ' && !in_word){
            in_word = true;
            word_count++;
            printf("%d. ", word_count);
        }

        if(in_word && buff[i] != ' '){
            char_count++;
            putchar(*(buff+i));
        }

        if(buff[i] == ' ' && in_word){
            printf("(%d)\n", char_count);
            in_word = false;
            char_count = 0;
        }
    }
    printf("(%d)\n", char_count);
    return word_count;
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //      This is safe because the first condition we are checking in the expression is that the length of arguments. 
    //      If the length is less that 2, then the next part of the OR expression is not evaluated, and thus we do not
    //      worry about argv[1] not existing.
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //      The purpose of the following if statement is to ensure that the required argument has been passed in
    //      Otherwise, if we try to access argv[2] without making this check, we would run into a null pointer
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = malloc(BUFFER_SZ);

    if (buff == NULL){
        printf("Error allocating space!");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            break;
        
        case 'w':
            int word_count = word_print(buff, user_str_len);
            printf("\nNumber of words returned: %d\n", word_count);
            break;

        case 'x':
            printf("Not Implemented!\n");
            exit(0);
            /*if (argc < 5){
                //printf("Not enough arguments provided");
                exit(2);
            }*/
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE
//          Because we don't want to make the assumption that the length will always be a fixed size because it can change later.
//          While in this particular program, the size of the buffer will always be 50, in general, its a good practice to
//          dynamically handle these things, because if someone changes the size, it will break the program if we don't provide
//          the length of the buffer