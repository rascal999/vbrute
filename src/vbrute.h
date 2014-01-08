#define ITEMS_MAX 512
#define ITEM_LENGTH_MAX 512

typedef struct Items Items;
   
struct Items {
   char domain[ITEMS_MAX][ITEM_LENGTH_MAX];
   char ip[ITEMS_MAX][ITEM_LENGTH_MAX];
   int domainPtr;
   int ipPtr;
   //struct evhttp_connection *libevent_conn;
};
