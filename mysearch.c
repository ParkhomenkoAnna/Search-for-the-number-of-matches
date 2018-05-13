#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>

//---------------------------- Дефайн ----------------------------------------
#define MAX_LEN 255
#define MAX_SIZE 100
//--------------------------- Структуры ---------------------------------------
struct {
    pthread_mutex_t	mutex;
    char *inputFileName;
    char *keyword;
} shared = { PTHREAD_MUTEX_INITIALIZER };

struct DATA {
     char str[MAX_LEN];
     int num;
};
typedef struct DATA Data;

//--------------------------- Прототипы ---------------------------------------
int meetings_count(const char *, const char *);
void *handler(void *);
int searchKeyword(int, char *, char *, char *);

//--------------------------- Главная функция ---------------------------------
int main(int argc, char* argv[])
{
    // Объявления и инициализация переменных
    int cntThrd = 0;
    char *keyword, *inputFileName, *outputFileName;
    // Считываение аргументов из командной строки
    for (int i=1; i<argc; i++)
    {
        if (strcmp(argv[i], "-n") == 0)
            cntThrd = atoi(argv[++i]);
        else if (strcmp(argv[i], "-s") == 0)
            keyword = argv[++i];
        else if (strcmp(argv[i], "-inputfile") == 0)
            inputFileName = argv[++i];
        else if (strcmp(argv[i], "-outputfile") == 0)
            outputFileName = argv[++i];
    }
    if ( (cntThrd == 0 ) || (keyword == NULL) || (inputFileName == NULL) || (outputFileName == NULL) )
    {
        printf("Usage: \n\tn - number of threads\n\ts - pattern for search\n\tinputFileName - lists files for search"
               "\n\toutputFileName - file for writing\n");
        return 1;
    }
     // Запуск функции поиска
    searchKeyword(cntThrd, inputFileName, outputFileName, keyword);
    return 0;
}

//--------------------------- Функции ----------------------------------------

int meetings_count(const char * str, const char * sub) {
    return ( str = strstr(str, sub) ) ? 1 + meetings_count(str + 1, sub) : 0;
}

void *handler(void *arg)
{
    // Объявления и инициализация переменных
    Data *data = (Data*)arg;
    clock_t start;
    // Установка блокировки
    pthread_mutex_lock(&shared.mutex);
    start = clock();
    int match;
    FILE *in;
    if ((in = fopen(shared.inputFileName, "a")) == NULL)
    {
            printf("Не удается открыть файл для дозаписи\n");
            exit(1);
    }
    if ((match = meetings_count(data->str, shared.keyword)) > 0)
       fprintf(in, "In lines #%d was founed %d matchs in times %lu ms\n", data->num, match, (clock()-start));
    fclose(in);
    //  Снятие  блокировки
    pthread_mutex_unlock(&shared.mutex);
}

int searchKeyword(int cntThrd, char *inputFileName, char *outputFileName, char * keyword)
{
    FILE *out;
    shared.inputFileName = inputFileName;
    shared.keyword = keyword;
    pthread_t thread[cntThrd];
    Data data[MAX_SIZE];
    int i, result;
    i = 0;
    if ((out = fopen(outputFileName, "r")) == NULL)
    {
            printf("Не удается открыть файл для чтения.\n");
            exit(1);
    }
    while (!feof(out))
    {
       if (fgets(data[i].str, MAX_LEN, out))
       {
           data[i].num = i;
           if ( (result = pthread_create(&thread[i%cntThrd], NULL , handler, &data[i])) != 0)
           {
                   perror("Не удается создать поток");
                   return EXIT_FAILURE;
           }
           i++;
           sleep(rand()%1);
       }
    }
    fclose(out);
    for (int i = 0; i < cntThrd; i++)
    if ( (result = pthread_join(thread[i], NULL)) != 0)
    {
            perror("Не удается заввершить поток");
            return EXIT_FAILURE;
    }
    return 0;
}
