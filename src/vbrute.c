#include <event.h>
#include <evhttp.h>
#include <getopt.h>
#include <netdb.h> /* hostent */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ITEMS_MAX 512
#define ITEM_LENGTH_MAX 512
#define DEBUG 1

char domain[ITEMS_MAX][ITEM_LENGTH_MAX];
char ip[ITEMS_MAX][ITEM_LENGTH_MAX];

int debugPrintf(char msg[1024], ...)
{
   if (DEBUG == 1)
   {
      va_list args;
      va_start(args, msg);

      vprintf(msg, args);

      va_end(args);
   }

   return 0;
}

void reqhandler(struct evhttp_request *req, void *state)
{
    printf("in _reqhandler. state == %s\n", (char *) state);
    if (req == NULL) {
        printf("timed out!\n");
    } else if (req->response_code == 0) {
        printf("connection refused!\n");
    } else if (req->response_code != 200) {
        printf("error: %u %s\n", req->response_code, req->response_code_line);
    } else {
        printf("success: %u %s\n", req->response_code, req->response_code_line);
    }
    event_loopexit(NULL);
}

int doHTTPRequest(char *domain, const char *addr)
{
   const char *state = "misc. state you can pass as argument to your handler";
   unsigned int port = 80;
   struct evhttp_connection *conn;
   struct evhttp_request *req;

   printf("initializing libevent subsystem..\n");
   event_init();

   conn = evhttp_connection_new(addr, port);

   req = evhttp_request_new(reqhandler, (void *) state);
   evhttp_add_header(req->output_headers, "Host", domain);
   evhttp_add_header(req->output_headers, "Content-Length", "0");
   evhttp_make_request(conn, req, EVHTTP_REQ_GET, "/");
   event_dispatch();

   return 0;
}

/*
   1. Read in IP list and domain file
   2. Visit each domain using IP list
   3. Record response length and code for diff

   Need HTTPS support
*/

char readFile(char *fileName, char itemArray[ITEMS_MAX][ITEM_LENGTH_MAX])
{
   FILE *file;
   int c;

   file = fopen(fileName, "r");

   while(fgets(itemArray[c++],sizeof(itemArray[ITEMS_MAX]),file))
   {
      itemArray[c-1][strnlen(itemArray[c-1],ITEM_LENGTH_MAX)-1] = '\0';
      debugPrintf("readFile itemArray[c] == %s\n", itemArray[c-1]);
   }

   /* Array size */
   return c-1;
}

int main(int argc, char **argv)
{
   char domainFile[ITEM_LENGTH_MAX], ipFile[ITEM_LENGTH_MAX];
   int i, j, domainArraySize, ipArraySize;

   strcpy(domainFile,argv[1]);
   strcpy(ipFile,argv[2]);

   /* domain is 2d array */
   domainArraySize = readFile(domainFile,domain);
   /* ip is 2d array */
   ipArraySize = readFile(ipFile,ip);

   /* for domains in file */
   for(i=0;i<domainArraySize;i++)
   {
      /* for ips in file */
      for(j=0;j<domainArraySize;j++)
      {
         doHTTPRequest(domain[i], ip[j]);
      }
   }

   printf("Domains\n");
   for(i=0;i<domainArraySize;i++)
   {
      printf("%s\n", domain[i]);
   }

   printf("IPs\n");
   for(i=0;i<domainArraySize;i++)
   {
      printf("%s\n", ip[i]);
   }

   return 0;
}
