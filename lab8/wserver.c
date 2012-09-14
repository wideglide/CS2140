/* Bundt, Josh          CS2140          Lab 8 - wserver  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>

#include "http_support.h"

//
// Globals and constants
//

static unsigned int tcpQue = LISTEN_QUEUE;

fd_set s_fds, r_fds;

int fd_max, kill_switch, arg_c;

enum {terminate, run};

char **arg_v, *configfile;


/* necessary struct for getopt_long function */
const struct option long_opts[] = {
    {"help", no_argument, 0, 'h'},
    {"port", required_argument, 0, 'p'},
    {"config", required_argument, 0, 'c'},
    {0, 0, 0, 0}
};

/* these strings will display usage cases for each cmd arg option */
const char *opts_desc[] = {
    ", to display this usage message.",
    "=PORT#, optional port number, the default is 8888",
    "=FILE, optional configuration file which specifies root dir, \n  \
    \t\tthe default is /home/jabundt/www"
};

//
// External variables
//
extern char *server_root;

//
// Function prototypes and functions
//
void handle_connection(int fd);

void usage(const char* arg);

int getSocket(const char *port);

void accept_connection(int cfd);

void wait_connect( );

int read_config( );

static void handler(int signum);


//
// Main
//
int main(int argc, char *argv[])
{
    int lfd, i, opt, len;
    FD_ZERO(&s_fds);
    fd_max = 0;
    kill_switch = run;
    configfile = NULL;
    arg_c = argc;
    arg_v = argv;
    
    // copy the server_root into dynamic memory reference by a global
    i = strlen(DEFAULT_SERVER_ROOT);
    server_root = malloc(i+2);
    memset(server_root, '\0', i+2);
    strncpy(server_root, DEFAULT_SERVER_ROOT, i);

    // set up handlers for signals we care about
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGHUP, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    
    
    while(-1 != (opt = getopt_long(argc, argv, "hp:c:", long_opts, &i)) ) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'p':
                // open ports 
                if ((atoi(optarg) > 65535) || (atoi (optarg) < 1)) {
                    fprintf(stderr, "bad port number: (1-65535)\n");
                    exit(0);
                }

                lfd = getSocket(optarg);
                // getSocket handles errors internally
                printf(" - lfd = %d  fd_max = %d\n", lfd, fd_max);
                FD_SET(lfd, &s_fds);
                break;
            case 'c':
                // accept configuration file - check for existance
                exit_msg(access(optarg, F_OK) , "Config file not found");
                configfile = optarg;
                read_config();
                break;
            default:
                fprintf(stderr, "Invalid option.\n");
                usage(argv[0]);
                break;
        }
    } // while for long_opts

    if (fd_max == 0) {
        lfd = getSocket(DEFAULT_PORT);
        FD_SET(lfd, &s_fds);
    }
    // for consistency ensure server root ends with '/'
    len = strlen(server_root);
    if (server_root[len-1] != '/')
        strncat(server_root, "/", len);
    printf("Starting web server at %s \n", server_root);
    wait_connect( );

    free(server_root);
    return 0;
}

//
//  The function to open bind a socket given a port
//
int getSocket(const char *port) 
{
    struct addrinfo hint, *aip, *rp;
    int lfd, rval, optval;
    
    // initialize variables
    memset((void*) &hint, 0, sizeof(hint));
    
    // provide hints to point getaddrinfo in the right direction
    hint.ai_family = AF_UNSPEC;     // either IPv4 or IPv6
    hint.ai_socktype = SOCK_STREAM;     // use TCP
    hint.ai_flags = AI_PASSIVE;     // suitable for bind
    printf("port = %s\n", port);
    getaddrinfo(NULL, port, &hint, &aip);
    for (rp = aip; rp != NULL; rp = rp->ai_next) {
        lfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (lfd == -1) 
            continue;
        optval = 1;
        rval = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, 
                          &optval, sizeof(optval));
        exit_msg(rval < 0, "setsockopt() error");
        if (bind(lfd, rp->ai_addr, rp->ai_addrlen) != -1) 
            break;          // bind was successful
        close(lfd);
    }
    freeaddrinfo(aip);
    exit_msg(rp == NULL, "could not bind");
    
    rval = listen(lfd, tcpQue);
    exit_msg(rval < 0, "listen() error");
    
    if (lfd >= fd_max) fd_max = lfd + 1;
    return lfd;
}

//
// The function to wait for a client connection
//
void wait_connect( )
{
    //struct addrinfo * rp;
    struct timeval tv;
    int lfd, rval;
    
    do {
        FD_ZERO(&r_fds);
        /* reset fds from s_fds */
        for (lfd = 3; lfd < fd_max; lfd++) {
            if (FD_ISSET(lfd, &s_fds)) {
                FD_SET(lfd, &r_fds);
            }
        }
        tv.tv_sec = 15;
        tv.tv_usec = 0;
        
        rval = select(fd_max, &r_fds, NULL, NULL, &tv);
        if (rval < 0 && errno == EINTR)  {
            perror("select()");
            continue;
        } else if (rval < 0) {
            exit_msg((1), "select error");
        }
        
        waitpid(-1, NULL, WNOHANG);
        if (rval == 0) continue;

        /* need to figure out what to accept */
        for (lfd = 3; lfd < fd_max; lfd++) {
            if (FD_ISSET(lfd, &r_fds)) {
                accept_connection(lfd);
            }
        }
    } while ( kill_switch );
    
    for (lfd = 3; lfd < fd_max; lfd++) {
        if (FD_ISSET(lfd, &s_fds))  close(lfd);
    }
    printf("%5d: Server exiting.\n", getpid());
    
}

//
// The fuction to accept a client connection
//
void accept_connection(int lfd) 
{
    int cfd, pid;
    //accept 
    cfd = accept(lfd, NULL, NULL);
    while (cfd < 0) {
        perror("accept() error");
        if (errno != EINTR) 
            return;
        cfd = accept(lfd, NULL, NULL);
    }
            
    printf("%5d: Accepted connection.\n", getpid());
    
    if ((pid = fork()) == 0) {
        /* in child */
        close(lfd);
        // block signals in child process
        sigset_t mask_set, old_set;
        sigfillset(&mask_set);
        sigprocmask(SIG_BLOCK, &mask_set, &old_set);
        
        printf("%5d:   Handling connection.\n", getpid());
        handle_connection(cfd);
        printf("%5d: Done.\n", getpid());
        
        sigprocmask(SIG_UNBLOCK, &old_set, &mask_set);
        close(cfd);
        free(server_root);
        exit(0);
    }
    /* in parent */
    close(cfd);
}


// 
// The function to handle a client connection
//
void handle_connection(int fd)
{
    http_req req;
    FILE *rx, *tx;

    exit_msg((fd < 0) || (fd > FD_SETSIZE), "bad fd");
    
    // for streams with sockets, need one for read and one for write
    rx = fdopen(fd, "r");
    tx = fdopen(dup(fd), "w");

    init_req(&req);
    http_get_request(rx, &req);
    http_process_request(&req);
    http_response(tx, &req);

    shutdown(fileno(rx), SHUT_RDWR);
    fclose(rx);
    fclose(tx);
    free_req(&req);
    return;
}

//
// A fuction to parse the config file and set the root directory
//   configfile may have comments marked by #, but the last line
//   must contain a valid directory of the server root dir
//
int read_config( ) 
{
    char buf[FILENAME_MAX], *temp;
    int i;
    FILE* fp_r;

    fp_r = fopen(configfile, "r");
    if (fp_r == NULL) {
        perror("fopen - config file not available");
        return -1;
    }
    // read lines in to buf
    while (fgets(buf, FILENAME_MAX, fp_r)) {
        for (i = 0; buf[i] != '\0'; i++) {
            // check for # or terminating character
            if ( (buf[i] == '#') || isspace(buf[i]) )
                bzero(&buf[i], FILENAME_MAX-i); // fill remaining buffer w/ 0's
        }
    }
    // check for exisitance of the directory, fail safe to default
    if ( (access(buf, F_OK)) || (buf[0] == '\0') ) {
        fprintf(stderr, "valid root directory not found in config file.");
        fclose(fp_r);
        return -1;
    }
    i = strlen(buf)+2;  // +2 space for the NULL and possibly a final '/'
    temp = realloc(server_root, i);
    if (temp) {
        server_root = temp;
    } else {
        perror("malloc error");
        fclose(fp_r);
        return -1;
    }
    memset(server_root, '\0', i);
    strncpy(server_root, buf, i);

    fclose(fp_r);
    return 0;
}

//
// A function to handle signals
//
static void handler(int signum)
{
    int i;
    sigset_t mask_set, old_set;

    // block signals while handling
    sigfillset(&mask_set);
    sigprocmask(SIG_BLOCK, &mask_set, &old_set);
    switch (signum) {
        case SIGUSR1:
            fprintf(stderr,"\n\n%s was executed with the following "
            "arguments:\n  ", arg_v[0]);
            for (i = 0; i < arg_c; i++) {
                fprintf(stderr," %s", arg_v[i]);
            }
            fprintf(stderr, "\nThe current server root directory is:\n   %s", 
                    server_root);
            fprintf(stderr, "\n   process id : %d", getpid());
            fprintf(stderr, "\t group id   : %d", getpgrp());
            fprintf(stderr, "\t parent id  : %d\n\n", getppid());
            fflush(stderr);
            break;
        case SIGHUP:
            if (configfile)
                read_config();
            else 
                fprintf(stderr, "No configuration file was specified.\n");
            break;
        case SIGTERM:
            kill_switch = terminate;
            break;
        default:
        
            break;
    }
    sigprocmask(SIG_UNBLOCK, &old_set, &mask_set);
    return;
}


//
// The usage function
//
void usage(const char* arg)
{
    // print a usage message  
    int i;
    printf("Usage: %s [OPTIONS], where\n", arg);
    printf("   OPTIONS:\n");
    for (i = 0; i < sizeof(long_opts) / sizeof(struct option) -1; i++) {
        printf("\t-%c, --%s%s\n", 
            long_opts[i].val, long_opts[i].name, opts_desc[i]);
    }
    exit(0);
}
