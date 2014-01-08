#include <event.h>
#include <evhttp.h>
#include <getopt.h>
#include <netdb.h> /* hostent */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/vbrute.h"

#define DEBUG 0

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

int printResult(struct evhttp_request *req, Items *items)
{
   /* domain, ip, response code, lengeth */
   printf("%-24.24s%-16.16s%-10u%-10.10s\n",items->domain[items->domainPtr],items->ip[items->ipPtr],req->response_code,evhttp_find_header(req->input_headers, "Content-Length"));
}

void reqhandler(struct evhttp_request *req, void *vItems)
{
   /* Typecast */
   Items *items = (Items *) vItems;

   if (req == NULL) {
      debugPrintf("timed out!\n");
   } else if (req->response_code == 0) {
      debugPrintf("connection refused!\n");
   } else {
      debugPrintf("success: %u %s\n", req->response_code, req->response_code_line);
      debugPrintf("var == %s\n", evhttp_find_header(req->input_headers, "Content-Length"));
      printResult(req,items);
   }
   event_loopexit(NULL);
}

//int doHTTPRequest(Items *items, char *domain, const char *addr)
int doHTTPRequest(Items *items)
{
   const char *state = "misc. state you can pass as argument to your handler";
   unsigned int port = 80;
   struct evhttp_connection *conn;
   struct evhttp_request *req;

   debugPrintf("initializing libevent subsystem..\n");
   event_init();

   //conn = evhttp_connection_new(addr, port);
   conn = evhttp_connection_new(items->ip[items->ipPtr], port);

   req = evhttp_request_new(reqhandler, items);
   //evhttp_add_header(req->output_headers, "Host", domain);
   evhttp_add_header(req->output_headers, "Host", items->domain[items->domainPtr]);
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
   Items items;

   char domainFile[ITEM_LENGTH_MAX], ipFile[ITEM_LENGTH_MAX];
   int i, j, domainArraySize, ipArraySize;

   strcpy(domainFile,argv[1]);
   strcpy(ipFile,argv[2]);

   /* domain is 2d array */
   domainArraySize = readFile(domainFile,items.domain);
   /* ip is 2d array */
   ipArraySize = readFile(ipFile,items.ip);

   /* for ips in file */
   for(items.ipPtr=0;items.ipPtr<ipArraySize;items.ipPtr++)
   {
      /* domain, ip, response code, lengeth */
      printf("-----------------------------------------------------------\n");
      printf("Domain                  IP              Code      Length\n");
      printf("-----------------------------------------------------------\n");
      /* for domains in file */
      for(items.domainPtr=0;items.domainPtr<domainArraySize;items.domainPtr++)
      {
         //doHTTPRequest(&items, items.domain[items.domainPtr], items.ip[items.ipPtr]);
         doHTTPRequest(&items);
      }
   }

   if (DEBUG == 1)
   {
      printf("Domains\n");
      for(i=0;i<domainArraySize;i++)
      {
         printf("%s\n", items.domain[i]);
      }

      printf("IPs\n");
      for(i=0;i<domainArraySize;i++)
      {
         printf("%s\n", items.ip[i]);
      }
   }

   return 0;
}
