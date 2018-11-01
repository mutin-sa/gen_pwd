/*
 *
 * gen_pwd
 *
 * Compiling:
 *     TCC: tcc.exe gen_pwd.c argtable3.c -o gen_pwd.exe
 *     PCC: 
 *     GCC: 
 *     CLang: 
 *     ICC: 
 *     MSVC: 
 *
 */

#include <stdlib.h>	// for rand() and srand(), for random() and srandom(), for EXIT_SUCCESS and EXIT_FAILURE macro
#include <stdio.h>	// for printf(), for fprintf(), for NULL const
#include <string.h>	// for strlen()
#include <locale.h>	// for setlocale() - for correct displaying some characters, from "--characters" otion
// #include <stdbool.h>
// #include <limits.h>	// for UINT_MAX macro
#if defined(_WIN32) && !defined(_WIN64)
    #define srandom srand
    #define random rand
    #include <windows.h>	// for QueryPerformanceCounter()
#elif defined(_WIN32) && defined(_WIN64)
    #define srandom srand
    #define random rand
    #include <windows.h>	// for QueryPerformanceCounter()
#elif defined(__linux__)
     
#elif defined(__unix__) || defined(__APPLE__) && defined(__MACH__)
    #include <sys/param.h>
    #if defined(BSD)
        #define PLATFORM_NAME "bsd"
    #endif
#else
#endif
#include "argtable3.h"

/* global arg_xxx structs */
struct arg_lit *help, *version;
struct arg_int *length, *number;
struct arg_str *dictionary;
struct arg_file *o;
struct arg_end *end;

#define PWD_MIN_LEN 6
#define PWD_DEF_LEN 12
#define PWD_MAX_LEN 4096
#define PWD_MIN_NUM 1
#define PWD_DEF_NUM 1
#define PWD_MAX_NUM 4096

int len = PWD_DEF_LEN;
int num = PWD_DEF_NUM;

char *gen_rnd(char dict[], int len);
int chk_rnd(char str[]);
char *gen_pwd(int len);

int main(int argc, char *argv[]) {

    setlocale(LC_ALL, "Russian");

    /* the global arg_xxx structs are initialised within the argtable */
    void *argtable[] = {
        help    = arg_litn("h", "help", 0, 1, "display this help and exit"),
        version = arg_litn("v", "version", 0, 1, "display version info and exit"),
        length  = arg_intn(NULL, "length", "<n>", 0, 1, "set password length"),
        number  = arg_intn(NULL, "number", "<n>", 0, 1, "set number of passwords"),
     dictionary = arg_str0(NULL,"dictionary","WORD",       "across -x, commas -m, horizontal -x, long -l,"),
                  arg_rem (NULL,                       "  single-column -1, verbose -l, vertical -C"),
        o       = arg_filen("o", NULL, "myfile", 0, 1, "output file"),
        end     = arg_end(20),
    };
    
    // int exitcode = 0;
    int exitcode = EXIT_SUCCESS;
    char progname[] = "gen_pwd.exe";
    
    int nerrors;
    nerrors = arg_parse(argc,argv,argtable);

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
    {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout, end, progname);
        printf("Try '%s --help' for more information.\n", progname);
        // exitcode = 1;
        exitcode = EXIT_FAILURE;
        goto exit;
    }

    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
    {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout, argtable, "\n");
        // printf("Demonstrate command-line parsing in argtable3.\n\n");
        arg_print_glossary(stdout, argtable, "  %-25s %s\n");
        // exitcode = 0;
        exitcode = EXIT_SUCCESS;
        goto exit;
    }

    // version
    if (version->count > 0)
    {
        printf("Program: gen_pwd\n");
        printf("Version: 0.0.1\n");
        // exitcode = 0;
        exitcode = EXIT_SUCCESS;
        goto exit;
    }

    // length
    if (length->count > 0)
    {
        //printf("Length: %d", *length->ival);
        len = *length->ival;
        //printf("Length: %d", len);
    }

    // number
    if (number->count > 0)
    {
        //printf("Number: %d", *number->ival);
        num = *number->ival;
        //printf("Number: %d", num);
    }

    // dictionary
    if (dictionary->count > 0)
    {
        printf("Dictionary: %s\n", dictionary->sval[0]);
        exitcode = EXIT_SUCCESS;
        goto exit;
    }

    // to file
    if (o->count > 0)
    {
        FILE *fp;
        fp = fopen(*o->filename, "a+");
        while (num > 0) {
            num--;
            fprintf(fp, "%s\n", gen_pwd(len));
        }
        fclose(fp);

        //printf("Length: %d", *length->ival);
        //fn = *o->filename;
        //                                                                              printf("File: %s\n", *o->filename);
        //while (0==0){printf("%s\n", gen_pwd(len));}
        //printf("%s\n", gen_pwd(len));
        // exitcode = 0;
        exitcode = EXIT_SUCCESS;
        goto exit;
    }

    while (num > 0) {
        num--;
        printf("%s\n", gen_pwd(len));
    }

exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
    return exitcode;
}

char *gen_rnd(char dict[], int len) {

    LARGE_INTEGER pc_val;
    QueryPerformanceCounter(&pc_val);

    srandom((unsigned int)pc_val);

    char *str = (char *) malloc(sizeof(char) * (len + 1));

    for(int i = 0; i < len; i++)
    {
        //str[i] = dict[random() % (sizeof dict - 1)];
        str[i] = dict[random() % (strlen(dict))];
    }
    str[len] = '\0';

    char* rnd = str;

    free(str);
    str = NULL;

    return rnd;

}

int chk_rnd(char str[]) {

    int strl = strlen(str);

    int lc_chars=0;
    int uc_chars=0;
    int num_chars=0;
    int sym_chars=0;
    int sym_ext_chars=0;

    // Проверяем первый символ - он может быть только буквой
    if (str[0] >= 97 && str[0] <= 122) {
        ++lc_chars;
    } else if (str[0] >= 65 && str[0] <= 90) {
        ++uc_chars;
    } else {
        return EXIT_FAILURE;
    }
    // Проверяем середину
        for (int i = 1; i <= strl-2; i++) {
             if (str[i] >= 97 && str[i] <= 122) {
                 ++lc_chars;
             } else if (str[i] >= 65 && str[i] <= 90) {
                 ++uc_chars;
             } else if (str[i] >= 48 && str[i] <= 57) {
                 ++num_chars;


             } else if (str[i] == 42 || str[i] == 45 || str[i] == 46 || str[i] == 58) {
                 ++sym_chars;
             } else if (str[i] == 35 || str[i] == 36 || str[i] == 38 || str[i] == 64) {
                 ++sym_ext_chars;


             } else {
                 return EXIT_FAILURE;
             }
        }
    // Проверяем последний символ - он может быть только буквой либо цифрой
    if (str[strl-1] >= 97 && str[strl-1] <= 122) {
        ++lc_chars;
    } else if (str[strl-1] >= 65 && str[strl-1] <= 90) {
        ++uc_chars;
    } else if (str[strl-1] >= 48 && str[strl-1] <= 57) {
        ++num_chars;
    } else {
        return EXIT_FAILURE;
    }

    // Тут мы проверяем количество символов каждой категории в пароле
    if (lc_chars >= 2) {
    } else {
        return EXIT_FAILURE;
    }
    if (uc_chars >= 2) {
    } else {
        return EXIT_FAILURE;
    }
    if (num_chars >= 2) {
    } else {
        return EXIT_FAILURE;
    }
    if (sym_chars >= 2) {
    } else {
        return EXIT_FAILURE;
    }
    if (sym_ext_chars >= 2) {
    } else {
        return EXIT_FAILURE;
    }

    //printf ("lc_chars %d\n", lc_chars);
    //printf ("uc_chars %d\n", uc_chars);
    //printf ("num_chars %d\n", num_chars);
    //printf ("sym_chars %d\n", sym_chars);
    //printf ("sym_ext_chars %d\n", sym_ext_chars);

    return EXIT_SUCCESS;

}

char *gen_pwd(int len) {

    char dict[] = "abcdefhkmnprstuxyzABCDEFHKMNPRSTUXYZ2345689:*-.@#$&";
    // char dict[] = "abcdefhkmnprstuxyz";
    // char dict[] = "ABCDEFHKMNPRSTUXYZ";
    // char dict[] = "2345689";
    // char dict[] = ":*-.";
    // char dict[] = "@#$&";
    // int len = 12;

    char* str = gen_rnd(dict, len);

    while (chk_rnd(str) != EXIT_SUCCESS) {
        str = gen_rnd(dict, len);
    }

    char* pwd = str;

    return pwd;

}



/*
// tcc.exe gen_pwd.c argtable3.c -luser32 -o gen_pwd.exe
// -luser32 for GetCursorPos()

int prng(void) {

    //unsigned int seed;



    printf("RND_SRC:\n");


    LARGE_INTEGER pc_val;
    QueryPerformanceCounter(&pc_val);
    printf("pc_val: %lld\n", pc_val);
    printf("pc_val.LowPart: %lu\n", pc_val.LowPart);
    printf("pc_val.HighPart: %ld\n", pc_val.HighPart);
    printf("pc_val.QuadPart: %lld\n", pc_val.QuadPart);

    DWORD pid = GetCurrentProcessId();
    printf("PID: %lu\n", pid);

    DWORD tid = GetCurrentThreadId();
    printf("TID: %lu\n", tid);

    POINT cur_pos;
    GetCursorPos(&cur_pos);
    printf("CPOSx: %li\n", cur_pos.x);
    printf("CPOSy: %li\n", cur_pos.y);


    long long int entropy;
    entropy = (long long int)pid+(long long int)tid+((long long int)cur_pos.x+1)+((long long int)cur_pos.y+1);
    printf("ALL0: %lli\n", entropy);

    long long int entropy2;
    entropy2 = entropy % UINT_MAX;
    printf("ALL1: %lli\n", entropy2);

    unsigned int entropy3;
    entropy3 = (unsigned int)entropy2;
    printf("ALL2: %u\n", entropy3);


    printf("UINT_MAX: %u\n", UINT_MAX);

    //unsigned int test;
    //test = entropy % 2147483647;
    //printf("unsigned int entropy: %u\n", test);

    //unsigned int test;
    //test = (unsigned int)seed.HighPart;
    //printf("unsigned int seed: %d\n", test);



    //return seed;

}
*/
