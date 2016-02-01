
/*
** Header for support functions, used to build a HTTP 1.0 compliant webserver
*/

#ifndef _HTTP_SUPPORT_H_
#define _HTTP_SUPPORT_H_
#include <stdio.h>

// seconds to timeout waiting for activity
#define CLIENT_TIMEOUT    (5)

// listen() backlog
#define LISTEN_QUEUE      (10)

// buffer large enough to read in a request
#define MAX_REQUEST_LEN   (1024)

// default port for server
#define DEFAULT_PORT        "8888"

// default server root
#define DEFAULT_SERVER_ROOT "/home/jabundt/www/"

//
// Status codes from RFC 1945 (6.1.1)
//
#define OK              200
#define CREATED         201
#define ACCEPTED        202
#define NO_CONTENT      204
#define MOVED_PERM      301
#define MOVED_TEMP      302
#define NOT_MODIFIED    304
#define BAD_REQUEST     400
#define UNAUTHORIZED    401
#define FORBIDDEN       403
#define NOT_FOUND       404
#define INTERNAL_ERROR  500
#define NOT_IMPLEMENTED 501
#define BAD_GATEWAY     502
#define UNAVAILABLE     503

// supported methods
enum req_method { GET, HEAD, UNSUPPORTED };

// request types
enum req_type   { SIMPLE, FULL };

//
// State related to a client request 
//
typedef struct {
    int fd;                  // fd of request's connection
    enum req_type type;      // request type
    enum req_method method;  // requested method
    int  status;             // response status
    char *resource;          // requested resource name
    char *path;              // requested resource root path location
    int resource_fd;         // fs associated with requested resource
    off_t length;            // content length (size)
    char *mime;              // mime type
} http_req;

//
// Function prototypes
//
void init_req(http_req *req);
void free_req(http_req *req);
int http_get_request(FILE *stream, http_req *req);
int http_response(FILE *stream, http_req *req);
int http_process_request(http_req *req);
int get_mime_type(char *filename, http_req *req);
void exit_msg(int cond, const char* msg);
int dir_listing(char* path);


// a helpful debug macro
//   you can use it by calling debug_here();
#define debug_here(a)  \
do { \
    printf("%d: at %s:%d\n", getpid(), __FILE__, __LINE__); \
    fflush(stdout); \
} while(0)

//
// define statements for thead cancellation macros
//
#define CTYPE (void (*) (void*))
#define CARG (void *)


#endif // _HTTP_SUPPORT_H_
