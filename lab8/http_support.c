/* Bundt, Josh          CS2140          Lab 8 - wserver  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <stdint.h>
#include "http_support.h"

#define HTTP_TIME_FORMAT "%a, %d %b %Y %H:%M:%S GMT"
#define DIRS_TIME_FORMAT "%d-%b-%Y %H:%M:%S"
#define TIME_STR_MAX 21

char *server_root;


//
// initializes a request to sensible defaults
//
void init_req(http_req *req)
{
    req->resource = NULL;
    req->mime = NULL;
    req->resource_fd = -1;
    req->method = UNSUPPORTED;
    req->status = OK;
    req->type = SIMPLE;
    req->length = 0;
}

//
// Free the resources associated with a request
//
void free_req(http_req *req)
{
    if (req->resource_fd != -1) {
        close(req->resource_fd);
        req->resource_fd = -1;
    }
    if (req->resource) {
        free(req->resource);
        req->resource = NULL;
    }
}

//
// Reads in all the headers sent by the client
// and uses them to populate req
//
int http_get_request(FILE *stream, http_req *req)
{
    int len, rval, lineno, fd;
    char *ptr, *val, buf[MAX_REQUEST_LEN];
    fd_set fds;
    struct timeval tv;

    // initialize
    fd = fileno(stream);
    tv.tv_sec = CLIENT_TIMEOUT;
    tv.tv_usec = 0;
    lineno = 0;

    // Just get the first line of the request. The
    //  first line contains the essential URI request,
    //  and contains enough information to distinguish between
    //  full and simple requests.
    //
    // Simple-Request = "GET" SP Request-URI CRLF
    // Full-Request =   Method SP Request-URI SP HTTP-Version CRLF
    //                  *(General-Header | Request-Header | Entity-Header)
    //                  CRLF
    //                  [Entity-Body]
    FD_ZERO(&fds);
    FD_SET(fd, &fds);
    rval = select(fd + 1, &fds, NULL, NULL, &tv);
    exit_msg(rval < 0, "select() error");
    if (rval == 0) return -1;

    fgets(buf, sizeof(buf), stream);
    ptr = buf;
    lineno++;

    // "GET" SP Request-URI CRLF
    // Method SP Request-URI SP HTTP-Version CRLF
    if (lineno == 1) {
        if (strncmp(ptr, "GET ", 4) == 0) {
            req->method = GET;
            ptr += 4;
        } else if (strncmp(ptr, "HEAD ", 5) == 0) {
            req->method = HEAD;
            ptr += 5;
        } else {
            req->method = UNSUPPORTED;
            req->status = NOT_IMPLEMENTED;
            return -1;
        }
        // skip SP
        while (*ptr && isspace(*ptr))
            ptr++;
        // Request-URI
        val = strchr(ptr, ' ');
        if (!val)
            len = strlen(ptr);
        else
            len = val - ptr;
        if (len < 1) {
            req->status = BAD_REQUEST;
            return -1;
        }
        req->resource = malloc(len+1);
        if (req->resource == NULL) {
            req->status = INTERNAL_ERROR;
            return -1;
        }
        memset(req->resource, '\0', len+1);
        strncpy(req->resource, ptr, len);

        // HTTP-Version present?
        if (strstr(ptr, "HTTP/"))
            req->type = FULL;
        else
            req->type = SIMPLE;
    } else {
        // we are going to ignore the value of subsequent headers
        // but this is where we would process them
    }
    return 0;
}


//
// Sends a response to the client
//
// Based on RFC1945 (6)
// Response =           Simple-Response | Full-Response
// Simple-Response =    [Entity-Body]
// Full-Response =      Status-Line
//                      *(General-Header | Response-Header | Entity-Header)
//                      CRLF
//                      [Entity-Body]
//
int http_response(FILE *stream, http_req *req)
{
    int i, fd = fileno(stream);
    char c, timestr[128];
    time_t now;

    // (3.3) HTTP-date format
    now = time(NULL);
    strftime(timestr, sizeof(timestr), HTTP_TIME_FORMAT, gmtime(&now));

    // Send HTTP headers
    //  Only full request need an HTTP header in the response
    //
    // Status-Line
    // *(General-Header | Response-Header | Entity-Header)
    // CRLF
    if (req->type == FULL) {
        fprintf(stream, "HTTP/1.0 %d\r\n", req->status); //req->status
        fprintf(stream, "Server: CS2140 v1\r\n");
        fprintf(stream, "Date: %s GMT\r\n", timestr);
        if (req->status == OK)
            fprintf(stream, "Content-Length: %ju\r\n", (uintmax_t) req->length);
        fprintf(stream, "Content-Type: %s\r\n", 
            (req->mime && (req->status == OK)) ? req->mime : "text/html");
        fprintf(stream, "\r\n");
        fflush(stream);
    }

    // Send Entity-Body
    //
    if (req->method == HEAD) {
        return 0;
    } else if (req->status == OK) {
    // read the resource_fd; write it out to the client
        lseek(req->resource_fd, 0, SEEK_SET);
        while ((i = read(req->resource_fd, &c, 1))) {
            write(fd, &c, 1);
        }
    } else {
    // Return an error message
        fprintf(stream, "<HTML>\n<HEAD>\n<TITLE>");
        fprintf(stream, "Server Error %d", req->status);
        fprintf(stream, "</TITLE>\n</HEAD>\n<BODY>\n<H1>");
        fprintf(stream, "Server Error %d", req->status);
        fprintf(stream, "</H1>\n<P>");
        fprintf(stream, "The request could not be completed.");
        fprintf(stream, "</P>\n</BODY>\n</HTML>\n");
        debug_here();
    }
    fflush(stream);
    return 0;
}


//
// Parses the request from the client and populates req
//
int http_process_request(http_req *req)
{
    char *apath, path[PATH_MAX];
    struct stat statbuf;
    int rlen, alen;

    if (req->method != GET)
        return 0;

    // path with server_root prepended
    strncpy(path, server_root, sizeof(path));
    if (req->resource[0] != '/')
        strncat(path, "/", sizeof(path));
    strncat(path, req->resource, sizeof(path) - strlen(path));
    // use realpath to determine the absolute path
    apath = realpath(path, NULL);
    if (apath == NULL) {
        perror("apath() error");
        // handle not found and not authorized conditions
        if (errno == ENOENT) 
            req->status = NOT_FOUND;
        else if (errno == EACCES)
            req->status = UNAUTHORIZED;
        return -1;
    }
    rlen = strlen(server_root);
    alen = strlen(apath);
    // rewrite path and req-resource using new absolute path
    strncpy(path, apath, sizeof(path));
    // copy server_root to ensure the request is bounded
    strncpy(path, server_root, rlen);
    if (alen <= rlen)
        strncpy(req->resource, "/", strlen(req->resource));
    else
        strncpy(req->resource, apath+rlen-1, strlen(req->resource));

    free(apath);
 
    // stat it -- could we open it read-only?
    if (stat(path, &statbuf) < 0) {
            perror("stat() error");
            req->status = INTERNAL_ERROR;
        return -1;
    } else if (S_ISDIR(statbuf.st_mode)) {
    // its a directory
        // output web content summarizing directory contents
        
        /* check permissions - personally I think access() works much better
         * in checking directory access permissions.  scandir() does not fail 
         * if the execute permission is not set on a directory. */
/*        if (access(path, X_OK) < 0) {
            req->status = FORBIDDEN;
            return 0;
        }*/
        
        req->resource_fd = dir_listing(path);
        // dir_listing returns -1 on scandir errors
        if (req->resource_fd == -1) {
            req->status = FORBIDDEN;
            return 0;
        }
        req->mime = "text/html";
        fstat(req->resource_fd, &statbuf);
        req->length = statbuf.st_size;
    } else {
    // its a file,
        //  open it and we will send it to the client
        req->resource_fd = open(path, O_RDONLY);
        if (req->resource_fd < 0) {
            perror("open() error");
            if (errno == EACCES) 
                req->status = UNAUTHORIZED;
            else
                req->status = INTERNAL_ERROR;
        }
        get_mime_type(req->resource, req);
        req->length = statbuf.st_size;
    }
    return 0;
}

//
// Sets the request's mime type to point to a string
// constant that is associated with the mime type;
// the server will generate a Content-Type HTTP header
// for the retrieved media.
//
int get_mime_type(char *filename, http_req *req)
{
    char *ext = strrchr(filename, '.');
    if (!ext)
        req->mime = NULL;
    else if (!strncmp(ext, ".html", 5) || !strncmp(ext, ".htm", 4))
        req->mime = "text/html";
    else if (!strncmp(ext, ".jpg", 4) || !strncmp(ext, ".jpeg", 5))
        req->mime = "image/jpeg";
    else if (!strncmp(ext, ".gif", 4))
        req->mime = "image/gif";
    else if (!strncmp(ext, ".png", 4))
        req->mime = "image/png";
    else if (!strncmp(ext, ".css", 4))
        req->mime = "text/css";
    else if (!strncmp(ext, ".au", 4))
        req->mime = "audio/basic";
    else if (!strncmp(ext, ".wav", 4))
        req->mime = "audio/wav";
    else if (!strncmp(ext, ".avi", 4))
        req->mime = "video/x-msvideo";
    else if (!strncmp(ext, ".mpeg", 5) || !strncmp(ext, ".mpg", 4))
        req->mime = "video/mpeg";
    else if (!strncmp(ext, ".mp3", 4))
        req->mime = "audio/mpeg";
    return 0;
}

//
// A simple function to print error and exit, if cond is true
//
void exit_msg(int cond, const char* msg)
{
    if (cond) {
        perror(msg);
        exit(-1);
    }
    return;
}

//
// A fuction to create the temporary file for a directory listing
//
int dir_listing(char *path)
{
    char timeStr[TIME_STR_MAX], pathDir[PATH_MAX], *url;
    struct stat statbuf;
    char template[] = "/tmp/wserver.XXXXXX";
    struct dirent **dirs;
    int i, n, fd, s;
    FILE* fp;
    struct tm * ptm;
    
    // scandir needs a trailing / to operate
    i =  strlen(path);
    if (path[i-1] != '/')
        strncat(path, "/", 1);
    // create a pointer to the url path
    i = strlen(server_root);
    url = path+i-1; 
    
    n = scandir(path, &dirs,0, alphasort);
    // need to check for scandir errors
    if (n <= 0) {
        perror ("scandir()");
        return -1;
    }
    // create a temporary file to write html directory listing
    fd = mkstemp(template);
    unlink(template);
    fp = fdopen(dup(fd), "r+");
    if (fp == NULL) {
        perror("fdopen() error on temp file");
        if (fd != -1) close(fd);
        return -1;
    }

    // write the directory listing to the temporary file
    fprintf(fp, "<HTML><HEAD><TITLE>Index of %s</TITLE></HEAD>\r\n", url);
    fprintf(fp, "<BODY><H2>Index of %s</H2>\r\n<PRE>\n",url);
    fprintf(fp,"Name \t\t\t\t\t\t  Last Modified \t\tSize\r\n");
    fprintf(fp, "\r\n<HR>\r\n");
    
    for (i = 0; i < n; i++) {
        memset(pathDir, '\0', sizeof(pathDir));
        strncpy(pathDir, path, sizeof(pathDir));
        strncat(pathDir, dirs[i]->d_name, sizeof(pathDir)-strlen(pathDir));
        
        if (stat(pathDir, &statbuf) < 0) {
            free(dirs[i]);
            continue;
        }
        ptm = localtime(&statbuf.st_mtime);
        s = strftime(timeStr, TIME_STR_MAX, DIRS_TIME_FORMAT, ptm);
        if (s == 0) {
            fprintf(stderr, "strftime returned 0");
            memset(timeStr, '\0', TIME_STR_MAX);
        }
        
        // the first two entries are ./ and ../ - no url needed
        if (i < 2 ) {
            fprintf(fp, "<a href=\"%s%s", dirs[i]->d_name,
                    S_ISDIR(statbuf.st_mode) ? "/" : "");
        } else {
            fprintf(fp,"<a href=\"%s%s%s", url, 
                    dirs[i]->d_name, 
                    S_ISDIR(statbuf.st_mode) ? "/" : "");
        }
            fprintf(fp,"\">%s%s</a>", dirs[i]->d_name, 
                    S_ISDIR(statbuf.st_mode) ? "/" : "");
        fprintf(fp,"%*s", 50 - strlen(dirs[i]->d_name), "");;
        if (S_ISREG(statbuf.st_mode)) {
            fprintf(fp, " %-30s%ju\r\n", 
                    timeStr,(uintmax_t) statbuf.st_size);
        } else {
            fprintf(fp, "%-30s\r\n", timeStr);
        }
        free(dirs[i]);
    } // while

    free(dirs);
    
    fprintf(fp, "</PRE>\r\n<HR>\r\n</BODY></HTML>\r\n");
    fflush(fp);
    
    fclose(fp);

    return fd;
}
