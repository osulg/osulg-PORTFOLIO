///////////////////////////////////////////////////////////////
// File name : proxy_cache.c                                 //
// Date : 2025/05/29                                         //
// OS : Ubuntu 20.04 LTS 64bits                              //
// Author : Oh Seol Gyeong                                   //
// Student ID : 2023202031                                   //
// ----------------------------------------------------------//
// Title : System Programming Assignment #3-1 (proxy server) //
// Description :                                             //
//   - Forwards HTTP GET requests from the client to the     //
//     web server and returns the response                   //
//   - Implements synchronization using a semaphore          //
//   - only one process can access the log at a time         //
//     (critical section)                                    //
///////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define RWX (S_IRWXU | S_IRWXG | S_IRWXO) // define with full permission (0777)
#define PORTNO 39999                      // define the port number
#define backlog 5                         // set maximun number of client connections in queue to 5
#define BUFSIZE 1024                      // Buffer size

time_t start_time; // start time to store (global variable)

// char last_input_url[300] = "";
char last_input_host[256] = ""; //
bool host_saved = false;

char first_input_url[300] = "";
bool first_input_saved = false;

// define function
char *sha1_hash(char *input_url, char *hashed_url);
char *getHomeDir(char *home);
void getReceivingTime(char *buf);
void p(int semid);
void v(int semid);

///////////////////////////////////////////////////////////////
// Function : sha1_hash                                      //
// ==========================================================//
// Input    : char *input_url                                //
//          : char *hashed_url                               //
// Output   : SHA-1 hash result                              //
// Purpose  : Hash the input URL using SHA-1           //
///////////////////////////////////////////////////////////////

char *sha1_hash(char *input_url, char *hashed_url)
{
    unsigned char hashed_160bits[20]; // SHA-1 result
    char hashed_hex[41];              // hex character value
    int i;

    // SHA-1 hash
    SHA1((unsigned char *)input_url, strlen(input_url), hashed_160bits);

    // convert to hex
    for (i = 0; i < sizeof(hashed_160bits); i++)
    {
        sprintf(hashed_hex + i * 2, "%02x", hashed_160bits[i]);
    }

    strcpy(hashed_url, hashed_hex); // copy to output buffer

    return hashed_url; // return hashed URL
}

///////////////////////////////////////////////////////////////
// Function : getHomeDIr                                     //
// ==========================================================//
// Input    : char *home                                     //
// Output   : Home Directory                                 //
// Purpose  : Get the user's home directory path             //
///////////////////////////////////////////////////////////////

char *getHomeDir(char *home)
{
    struct passwd *usr_info = getpwuid(getuid()); // get user information
    strcpy(home, usr_info->pw_dir);               // copy home path

    return home; // return home directory
}

///////////////////////////////////////////////////////////////
// Function : getReceivingTime                               //
// ========================================================= //
// Input    : char *buf                                      //
// Output   : formatted local time                           //
// Purpose  : Get Current Local Time as formatted string     //
///////////////////////////////////////////////////////////////

void getReceivingTime(char *buf)
{
    time_t t;      // time variable
    struct tm *lt; // structure for local time

    time(&t);           // get current time
    lt = localtime(&t); // convert time to local time

    // formatted time
    sprintf(buf, "%d/%02d/%02d, %02d:%02d:%02d", lt->tm_year + 1900, lt->tm_mon + 1, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);
}

///////////////////////////////////////////////////////////////
// Function : sigchld_handler                                //
// ========================================================= //
// Purpose : handle zombie process                           //
///////////////////////////////////////////////////////////////

static void sigchld_handler()
{
    pid_t pid;  // process id
    int status; // status of child process

    // prevent zombie process
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        ;
}

///////////////////////////////////////////////////////////////
// Function : isValidURL                                     //
// ==========================================================//
// Input    : char* input_url                                //
// Output   : true/false                                     //
// Purpose   : URL format validation                         //
///////////////////////////////////////////////////////////////

bool isValidURL(char *input_url)
{
    // chech if the input is null or an empty string
    if (input_url == NULL || strlen(input_url) == 0)
    {
        printf("Invalid URL : Input is Empty!\n");
        return false;
    }

    // check if the input is too short (ex. abc)
    if (strlen(input_url) < 4)
    {
        printf("Invalid URL : Input is too short!\n");
        return false;
    }

    // check for spaces in the URL
    if (strchr(input_url, ' ') != NULL)
    {
        printf("Invalid URL : URL can't contain space!\n");
        return false;
    }

    // check if the URL ends with a dot(.)
    if (input_url[strlen(input_url) - 1] == '.')
    {
        printf("Invalid URL : Can't end with a dot(.)!\n");
        return false;
    }

    // check if the URL starts with a dot(.)
    if (input_url[0] == '.')
    {
        printf("Invalid URL : Can't start wth a dot(.)!\n");
        return false;
    }

    char tok_url[300];                            // buffer to copy
    strncpy(tok_url, input_url, sizeof(tok_url)); // copy input_url into a buf(tok_url)
    tok_url[sizeof(tok_url) - 1] = '\0';          // null termination

    int token_count = 0; // counter for tokens

    char *token = strtok(tok_url, "."); // get the first token using dot

    // parsing the rest of tokens
    while (token != NULL)
    {
        token_count++;             // token + 1
        token = strtok(NULL, "."); // next token
    }

    return true; // true if, url format is valid
}

///////////////////////////////////////////////////////////////
// Function : sigint_handler                                 //
// ========================================================= //
// Input : int sig                                           //
// Purpose : handles the SIGINT signal                       //
//           count HIT/MISS count and calculate run time     //
//           log terminated status                           //
///////////////////////////////////////////////////////////////

void sigint_handler(int sig)
{
    time_t end_time;                                                     // end time
    int run_time;                                                        // running time
    FILE *log_fp = fopen("/home/kw2023202031/logfile/logfile.txt", "r"); // open log file in read mode

    // fail to open
    if (!log_fp)
    {
        printf("open fail\n"); // print error
        exit(1);
    }

    char line[1024];    // buffer to store each line
    int hit_count = 0;  // hit count
    int miss_count = 0; // miss count

    // read each line
    while (fgets(line, sizeof(line), log_fp))
    {
        // contain both [HIT] and http://
        if (strstr(line, "[HIT]") && strstr(line, "http://"))
        {
            hit_count++; // hit_count++
        }
        // contain both [MISS] and http://
        else if (strstr(line, "[MISS]") && strstr(line, "http://"))
        {
            miss_count++; // miss_count++
        }
    }

    fclose(log_fp); // close the log file

    end_time = time(NULL);                          // get end time
    run_time = (int)difftime(end_time, start_time); // calculate rum time (end_time - start_time)

    // open log file in append mode
    log_fp = fopen("/home/kw2023202031/logfile/logfile.txt", "a");

    if (log_fp)
    {
        // log terminated status
        int total_sub = hit_count + miss_count;

        // print total sub process
        fprintf(log_fp, "**SERVER** [Terminated] run time: %d sec. #sub process: %d\n", run_time, total_sub);
    }

    exit(0); // exit
}

///////////////////////////////////////////////////////////////
// Function : sigalrm_handler                                //
// ========================================================= //
// Purpose : handles the SIGALRM signal                      //
//           print NO RESPONSE message                       //
///////////////////////////////////////////////////////////////

void sigalrm_handler()
{
    printf("========           ========\n"); // print

    exit(1);
}

///////////////////////////////////////////////////////////////
// Function : is_Request                                     //
// ==========================================================//
// Input    : const char* url                                //
// Output   : true/false                                     //
// Purpose  : filter out automatic or irrelevant             //
//            request                                        //
///////////////////////////////////////////////////////////////

bool is_Request(const char *url)
{
    // list of URL patterns to ignore
    char *not_log[] = {
        "favicon.ico",
        "detectportal",
        "success.txt",
        "canonical.html",
        ".css",
        ".js",
        ".jpg",
        ".png",
        ".gif",
        NULL};

    // check if the url contains ignored patterns
    for (int i = 0; not_log[i] != NULL; i++)
    {
        if (strstr(url, not_log[i]) != NULL)
        {
            return false; // if contain, return false
        }
    }

    // if url is NULL or empty
    if (!url || strlen(url) == 0)
    {
        return false; // return false
    }

    // check if url starts with "http://"
    if (strncmp(url, "http://", 7) == 0)
    {
        // find the start of the path part
        const char *path_start = strchr(url + 7, '/');

        // if path exists, accept url
        if (path_start == NULL || strlen(path_start) > 0)
        {
            return true;
        }
    }

    return false; // other case false
}

///////////////////////////////////////////////////////////////
// Function : parse_url                                      //
// ==========================================================//
// Input    : const char* url                                //
//          : char* host                                     //
//          : char* path                                     //
// Purpose  : parse url into host and path                   //
///////////////////////////////////////////////////////////////

void parse_url(const char *url, char *host, char *path)
{
    // check if the url starts with http://
    const char *url_start = strstr(url, "http://");

    if (url_start)
    {
        url_start += 7; // ship http://
    }
    else
    {
        url_start = url; // if not found, use entire string
    }

    // find first / after host
    const char *slash_pos = strchr(url_start, '/');

    // if slash exists
    if (slash_pos)
    {
        // extract host and path
        size_t host_len = slash_pos - url_start;
        strncpy(host, url_start, host_len);
        host[host_len] = '\0';
        strncpy(path, slash_pos, 1023);
        path[1023] = '\0';
    }
    else
    {
        // no slash, entire string is host
        strncpy(host, url_start, 255);
        host[255] = '\0';
        strcpy(path, "/");
    }
}

///////////////////////////////////////////////////////////////
// Function : sub_server                                     //
// ========================================================= //
// Input : client_fd, client_addr, int semid                 //
// Purpose : handle a client request                         //
//           checks the cache and return HIT/MISS            //
///////////////////////////////////////////////////////////////

void sub_server(int client_fd, struct sockaddr_in client_addr, int semid)
{
    signal(SIGALRM, sigalrm_handler); // SIGALRM handler

    // save client IP address
    struct in_addr inet_client_address;
    inet_client_address.s_addr = client_addr.sin_addr.s_addr;

    char input_url[300] = {0}; // input url
    char hashed_url[41];       // hashed url
    char home[128];            // user home directory
    char dir_name[512];        // final directory path
    char hashed_dir_name[4];   // first 3 character of hash
    char cache_dir[256];       // cache directory path
    char file_name[1024];      // file path
    char log_dir[256];         // logfile directory
    char log_file_path[512];   // logfile.txt path
    char time_buf[128];        // buffer for time
    time_t end_time;           // end time
    int run_time;              // run time
    int hit_count = 0;         // hit_count
    int miss_count = 0;        // miss_count
    char buf[BUFSIZE] = {
        0,
    }; // buffer to store client requsert
    char tmp[BUFSIZE] = {
        0,
    }; // temporary buffer for tokenizing
    char method[20] = {
        0,
    };                                    // store HTTP method
    char *tok = NULL;                     // token
    char response_header[BUFSIZE] = {0};  // HTTP response header
    char response_message[BUFSIZE] = {0}; // HTTP response message
    char studentID[20] = "kw2023202031";  // student ID
    FILE *log_fp;                         // logfile
    FILE *fp = NULL;

    start_time = time(NULL); // get current time

    bool log = false;
    int len = sizeof(client_addr);

    getHomeDir(home);                                                          // get home directory
    snprintf(cache_dir, sizeof(cache_dir), "%s/cache", home);                  // ~/cache
    snprintf(log_dir, sizeof(log_dir), "%s/logfile", home);                    // ~/logfile
    snprintf(log_file_path, sizeof(log_file_path), "%s/logfile.txt", log_dir); // ~/logfile/logfile.txt

    // printf("[%s : %d] Clinet was connected\n", inet_ntoa(inet_client_address), client_addr.sin_port); // print client connection message

    read(client_fd, buf, BUFSIZE); // read HTTP request from client(wev browser)

    /*puts("================================================");
    printf("Requset from [%s : %d]\n", inet_ntoa(inet_client_address), client_addr.sin_port);
    puts(buf);
    puts("================================================");*/

    // Extract method and URL
    strcpy(tmp, buf);       // copy buffer for parsing
    tok = strtok(tmp, " "); // get HTTP method
    if (tok)
    {
        strcpy(method, tok); // store 'method' in method var
        method[sizeof(method) - 1] = '\0';
    }

    tok = strtok(NULL, " "); // URL
    if (tok != NULL)
    {
        strcpy(input_url, tok);
        // printf("[INPUT URL] : %s\n", input_url);

        if (!first_input_saved && !host_saved && strcmp(method, "GET") == 0 && is_Request(input_url))
        {

            char host_buf[256], path_buf[1024];
            parse_url(input_url, host_buf, path_buf);
            strcpy(last_input_host, host_buf);
            host_saved = true;
        }
    }

    // ignore non-GET requests
    if (strcmp(method, "GET") != 0)
    {
        // printf("[DEBUG] Ignored request - Method: %s\n", method);
        close(client_fd);
        exit(0);
    }

    sha1_hash(input_url, hashed_url);        // SHA-1 hash of URL
    strncpy(hashed_dir_name, hashed_url, 3); // first 3 characters
    hashed_dir_name[3] = '\0';

    snprintf(dir_name, sizeof(dir_name), "%s/%s", cache_dir, hashed_dir_name); // ~/cache/3ef
    snprintf(file_name, sizeof(file_name), "%s/%s", dir_name, hashed_url + 3); // ~/cache/3ef/(file name with hashed_URL - 3)

    // Filter automatic requsets
    bool is_auto_request = false;
    if (strstr(input_url, "favicon.ico") || strstr(input_url, "images") ||
        strstr(input_url, "detectportal") || strstr(input_url, "success.txt") ||
        strstr(input_url, "canonical.html"))
    {
        is_auto_request = true;
    }

    bool hit = false; // hit flag to false
    DIR *dp;          // directory stream
    struct dirent *d; // directory pointer (entry)

    // printf("Checking cache directory: %s\n", dir_name);

    // if cache directory exists, open directory
    if ((dp = opendir(dir_name)) != NULL)
    {
        while ((d = readdir(dp)))
        { // read entry

            // compare file name
            if (d->d_ino != 0 && strcmp(d->d_name, hashed_url + 3) == 0)
            {
                struct stat st;
                char check_path[BUFSIZE];

                snprintf(check_path, sizeof(check_path), "%s/%s", dir_name, d->d_name);

                // printf("[DEBUG] checking file: %s\n", check_path);
                if (stat(check_path, &st) == 0)
                {
                    // printf("[DEBUG] file size = %ld\n", st.st_size);
                }
                else
                {
                    perror("[ERROR] stat failed");
                }

                if (stat(check_path, &st) == 0 && st.st_size > 0)
                {
                    hit = true;
                }
                break;
            }
        }
        closedir(dp); // close directory
    }

    log_fp = fopen(log_file_path, "a"); // open logfile in append mode

    if (!log_fp)
    {                      // if fail to open
        perror("error\n"); // print error
        return;
    }

    if (hit)
    { // hit
        // printf("[DEBUG] HIT cache found.\n");

        p(semid);

        sleep(5);

        fp = fopen(file_name, "r");
        char cache_buf[BUFSIZE];
        int n;

        while ((n = fread(cache_buf, 1, BUFSIZE, fp)) > 0)
        {
            write(client_fd, cache_buf, n);
        }
        fclose(fp);

        // formats the HTTP message to send client (HTML body)
        snprintf(response_message, BUFSIZE,
                 "<h1>HIT</h1><br>"
                 "%s:%d<br>"
                 "%s<br>"
                 "%s",
                 inet_ntoa(inet_client_address), client_addr.sin_port, input_url, studentID);

        // formats the HTTP header to send client (HTTP header)
        snprintf(response_header, BUFSIZE,
                 "HTTP/1.0 200 OK\r\n"
                 "Cache-Control: no-store, no-cache, must-revalidate\r\n"
                 "Pragma: no-cache\r\n"
                 "Expires: 0\r\n"
                 "Content-length:%lu\r\n"
                 "Content-type:text/html\r\n\r\n",
                 strlen(response_message));

        write(client_fd, response_header, strlen(response_header));   // send response header
        write(client_fd, response_message, strlen(response_message)); // send response message

        // extract host and message to client
        char host_buf[256], path_buf[1024];
        parse_url(input_url, host_buf, path_buf);

        /*printf("[hit : HOST] : %s\n", host_buf);
        printf("[hit : PATH] : %s\n", path_buf);*/

        // log
        // 1. this is the first user-input URL (not automatic)
        // 2. domain matches the last input host
        if (!first_input_saved && !is_auto_request && strlen(last_input_host) > 0)
        {
            char input_host[256], input_path[1024];
            parse_url(input_url, input_host, input_path);

            if (strcmp(input_host, last_input_host) == 0)
            {
                getReceivingTime(time_buf); // get current time

                // p(semid);

                // sleep(5);

                // printf("[DEBUG] PID %d is about to write HIT log for URL: %s\n", getpid(), input_url);
                // printf("[DEBUG] is_auto_request: %d | is_Request: %d\n", is_auto_request, is_Request(input_url));

                // log HIT
                fprintf(log_fp, "[HIT] ServerPID : %d | %s/%s-[%s]\n", getpid(), hashed_dir_name, hashed_url + 3, time_buf);
                fprintf(log_fp, "[HIT]%s\n", input_url);

                // printf("[DEBUG] Logged HIT for %s\n", input_url);

                // v(semid);

                // reset the first_input_saved flag
                first_input_saved = false;
                memset(first_input_url, 0, sizeof(first_input_url));
            }
        }

        fclose(log_fp);   // close logfile
        v(semid);         // v operation on semaphore
        close(client_fd); // close client socket

        exit(0); // exit
    }
    else
    { // miss
        // printf("[DEBUG] MISS - Fetching from web server\n");
        char host[256];     // buffer for host
        char path[BUFSIZE]; // buffer for path

        // parse input_url to get host and path
        parse_url(input_url, host, path);
        /*printf("[miss : HOST] : %s\n", host);
        printf("[miss : PATH] : %s\n", path);*/

        struct hostent *hent = gethostbyname(host); // host to IP address

        // fail
        if (hent == NULL)
        {
            fprintf(stderr, "ERROR, no such host: %s\n", host);
            close(client_fd); // close client socket
            fclose(fp);       // close file
            return;
        }

        int web_fd = socket(AF_INET, SOCK_STREAM, 0); // create socket to connect to web server
        if (web_fd < 0)
        {
            perror("socket"); // socket creation error
            close(client_fd);
            fclose(fp);
            return;
        }

        struct sockaddr_in web_addr; // web server address structure

        bzero((char *)&web_addr, sizeof(web_addr));               // zero out
        web_addr.sin_family = AF_INET;                            // IPv4
        memcpy(&web_addr.sin_addr, hent->h_addr, hent->h_length); // copy IP address
        web_addr.sin_port = htons(80);                            // set port

        // connect to web server
        if (connect(web_fd, (struct sockaddr *)&web_addr, sizeof(web_addr)) < 0)
        {
            perror("connecting to web server\n"); // fail to create
            close(client_fd);
            close(web_fd);
            return;
        }

        char request[2048];         // buffer to store HTTP GET request
        bool first_response = true; // flag for first

        parse_url(input_url, host, path); // parse url

        // HTTP GET requset
        snprintf(request, sizeof(request),
                 "GET %s HTTP/1.1\r\n"
                 "Host: %s\r\n"
                 "Connection: close\r\n"
                 "\r\n",
                 path, host);

        write(web_fd, request, strlen(request)); // send HTTP GET request

        alarm(20); // set 20 sec timeout for HTTP response

        char web_buf[BUFSIZE]; // buffer to store web server response
        int n;

        umask(0);             // set umask to 0
        mkdir(dir_name, RWX); // create directory

        // printf("[DEBUG] PID %d is about to write MISS log for URL: %s\n", getpid(), input_url);
        // printf("[DEBUG] is_auto_request: %d | is_Request: %d\n", is_auto_request, is_Request(input_url));
        // printf("[DEBUG] log_fp open: %s\n", log_file_path);

        if (is_Request(input_url) && strlen(input_url))
        {
            p(semid); // p operation on semaphore

            FILE *fp = fopen(file_name, "w"); // create file

            // if successfully open
            if (!fp)
            {
                perror("fail to open cache file\n"); // print error
                fclose(fp);
                close(client_fd); // close client socket
                return;
            }

            log_fp = fopen(log_file_path, "a");
            if (!log_fp)
            {
                perror("log_fp open failed");
                fclose(log_fp);
                v(semid);
                return;
            }

            // read web werver response
            while ((n = read(web_fd, web_buf, BUFSIZE)) > 0)
            {
                if (first_response)
                {
                    alarm(0); // cancel previously set alarm
                    first_response = false;
                }
                fwrite(web_buf, 1, n, fp);    // write to cache file
                write(client_fd, web_buf, n); // send to client
            }
            fclose(fp); // close file

            getReceivingTime(time_buf); // get current time

            sleep(5);

            fprintf(log_fp, "[MISS] ServerPID : %d | %s-[%s]\n", getpid(), input_url, time_buf); // log MISS
            // printf("[DEBUG] Logged MISS for %s\n", input_url);

            v(semid); // v operation on semaphore
            fclose(log_fp);
        }
        else
        {
            // printf("[DEBUG] Skipping log for automatic request: %s\n", input_url);
            fclose(log_fp); // 여긴 안전하게 닫아줘야 함
        }

        close(web_fd); // close web server socket

        // formats the HTTP message to send client
        snprintf(response_message, BUFSIZE,
                 "<h1>MISS</h1><br>"
                 "%s:%d<br>"
                 "%s<br>"
                 "%s",
                 inet_ntoa(inet_client_address), client_addr.sin_port, input_url, studentID);

        // formats the HTTP header to send client
        snprintf(response_header, BUFSIZE,
                 "HTTP/1.0 200 OK\r\n"
                 "Cache-Control: no-store, no-cache, must-revalidate\r\n"
                 "Pragma: no-cache\r\n"
                 "Expires: 0\r\n"
                 "Content-length:%lu\r\n"
                 "Content-type:text/html\r\n\r\n",
                 strlen(response_message));

        write(client_fd, response_header, strlen(response_header));   // send response header
        write(client_fd, response_message, strlen(response_message)); // send response message
    }

    // printf("[%s : %d] Client was disconnected\n", inet_ntoa(inet_client_address), client_addr.sin_port); // print disconnect message

    close(client_fd); // close client socket
    exit(0);          // terminate child process
}

///////////////////////////////////////////////////////////////
// Function : p                                              //
// ==========================================================//
// Input    : semid                                          //
// Purpose  : perform P operation on semaphore               //
//          : Block the process until it can enter           //
//          : critical section / decrements the semaphore    //
///////////////////////////////////////////////////////////////

void p(int semid)
{
    struct sembuf pbuf;

    // set up semaphore P operation
    pbuf.sem_num = 0;        // semaphore number
    pbuf.sem_op = -1;        // P operation
    pbuf.sem_flg = SEM_UNDO; // kernel recovers semaphore (terminated abnormally)

    // print waiting message before entering critical section (with pid)
    printf("*PID# %d is waiting for the semaphore.\n", getpid());

    // perform P operation
    if (semop(semid, &pbuf, 1) == -1)
    {
        perror("p : semop failed");
        exit(1);
    }

    // print entered critical section
    printf("*PID# %d is in the critical zone.\n", getpid());
}

///////////////////////////////////////////////////////////////
// Function : v                                              //
// ==========================================================//
// Input    : semid                                          //
// Purpose  : perform V operation on semaphore               //
//          : signals that the process has exited            //
//          : critical section / increments the semaphore    //
///////////////////////////////////////////////////////////////

void v(int semid)
{
    struct sembuf vbuf;

    // set up semaphore V operation
    vbuf.sem_num = 0;        // semaphore number
    vbuf.sem_op = 1;         // V operation
    vbuf.sem_flg = SEM_UNDO; // kernel recovers semaphore (terminated abnormally)

    // perform V operation
    if (semop(semid, &vbuf, 1) == -1)
    {
        perror("v : semop failed");
        exit(1);
    }

    // print that the process exited the critical section
    printf("*PID# %d exited the critical zone.\n", getpid());
}

///////////////////////////////////////////////////////////////
// Function : main                                           //
// ========================================================= //
// Purpose : handle socket creation, bind(), listen()        //
//           , accept(), and fork()                          //
///////////////////////////////////////////////////////////////

int main()
{
    start_time = time(NULL);        // get start time
    signal(SIGINT, sigint_handler); // handler for SIGINT signal (log terminated status)

    int semid; // semid

    // create key for semaphore, PORTNO is used as the key
    key_t sem_key = (key_t)PORTNO;

    union semum
    {
        int val;                   // SETVAL
        struct semid_ds **buf;     // buffer for IPC_STAT / IPC_SET
        unsigned short int *array; // array for GETALL / SETALL
    } arg;

    // create semaphore
    // if not existing, create with permission 0666
    semid = semget(sem_key, 1, IPC_CREAT | 0666);

    if (semid == -1)
    {
        perror("semget failed"); // print error
        exit(1);
    }

    arg.val = 1; // initalize the semaphore value to 1 (1 available resource)

    if (semctl(semid, 0, SETVAL, arg) == -1)
    {
        perror("semctl failed"); // fail to initalize
        exit(1);
    }

    int socket_fd, client_fd;                    // server socket & client socket descriptor
    struct sockaddr_in server_addr, client_addr; // Address structures for server and client
    int len = sizeof(client_addr);               // size of client address
    pid_t pid;                                   // PID for child process

    char home[128];          // user home directory
    char cache_dir[256];     // cache directory path
    char log_dir[256];       // logfile directory
    char log_file_path[512]; // logfile.txt path

    int opt = 1; // value for setsockopt()

    getHomeDir(home);                                                          // get home directory
    snprintf(cache_dir, sizeof(cache_dir), "%s/cache", home);                  // ~/cache
    snprintf(log_dir, sizeof(log_dir), "%s/logfile", home);                    // ~/logfile
    snprintf(log_file_path, sizeof(log_file_path), "%s/logfile.txt", log_dir); // ~/logfile/logfile.txt

    umask(0);              // set umsak to 0
    mkdir(cache_dir, RWX); // create cache directory
    mkdir(log_dir, RWX);   // create logfile directory

    // create TCP stream socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Server : Can't open stream socket");
        return 0;
    }

    // To prevent TIME_WAIT problems caused by bind()
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt() failed\n");
        close(socket_fd);
        return 0;
    }

    bzero((char *)&server_addr, sizeof(server_addr)); // zero out server address struct
    server_addr.sin_family = AF_INET;                 // IPv4
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // accept from any address
    server_addr.sin_port = htons(PORTNO);             // set port num

    // bind socket to address
    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Server : Can't bind local address"); // if fail to bind
        close(socket_fd);                            // close socket
        return 0;
    }

    // listening for client connections
    if (listen(socket_fd, backlog) == -1)
    {
        perror("listen error");
        exit(1);
    }

    // handler to zombie process
    signal(SIGCHLD, (void *)sigchld_handler);

    while (1)
    {
        // Initialize client address
        memset((char *)&client_addr, '\0', sizeof(client_addr));

        // accept a client connection
        if ((client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len)) == -1)
        {
            perror("accept error");
            exit(1);
        }

        pid = fork(); // create child process using fork()

        // if fork() fails
        if (pid == -1)
        {
            // close socket
            close(client_fd);
            close(socket_fd);
            continue;
        }
        // child process
        else if (pid == 0)
        {
            sub_server(client_fd, client_addr, semid); // handle client request
            close(client_fd);                          // close socket
            exit(0);                                   // end child process
        }
        // parent process
        else
        {
            close(client_fd); // close client socket and wait
        }
    }

    close(socket_fd); // close server socket
    return 0;
}
