// ref: https://www.cnblogs.com/xinsheng/p/3880567.html

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/bufferevent.h>

#define LISTEN_PORT 9999
#define LISTEN_BACKLOG 32

void do_accept(evutil_socket_t listener, short event, void *arg);
void read_cb(struct bufferevent *bev, void *arg);
void error_cb(struct bufferevent *bev, short event, void *arg);
void write_cb(struct bufferevent *bev, void *arg);

int main(int argc, const char *argv[])
{
    int ret;
    evutil_socket_t listener;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    assert(listener >0);
    evutil_make_listen_socket_reuseable(listener);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(LISTEN_PORT);

    if(bind(listener, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("bind");
        return 1;
    }

    if(listen(listener, LISTEN_BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    printf("Listening...\n");

    evutil_make_socket_nonblocking(listener);

    struct event_base *base = event_base_new();
    assert(base != NULL);
    struct event *listen_event;
    listen_event = event_new(base, listener, EV_READ|EV_PERSIST, do_accept, (void*)base);
    event_add(listen_event, NULL);
    event_base_dispatch(base);

    printf("The End.");

    return 0;
}

void do_accept(evutil_socket_t listener, short event, void *arg) 
{
    struct event_base *base = (struct event_base *) arg;
    evutil_socket_t fd;
    struct sockaddr_in sin;
    socklen_t slen = sizeof(sin);
    fd = accept(listener, (struct sockaddr *)&sin, &slen);
    if(fd < 0) {
        perror("accept");
        return;
    }

    printf("ACCEPT: fd = %u\n", fd);

    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    //bufferevent_setcb(bev, read_cb, NULL, error_cb, arg);
    bufferevent_setcb(bev, read_cb, write_cb, error_cb, arg);
    bufferevent_enable(bev, EV_READ|EV_WRITE|EV_PERSIST);

}
/*
 * read callback is called whenever any data is read from the underlying transport.
 * */
void read_cb(struct bufferevent *bev, void *arg) 
{
#define MAX_LINE 4
    char line[MAX_LINE + 1];
    int n;
    evutil_socket_t fd = bufferevent_getfd(bev);

    while(n = bufferevent_read(bev, line, MAX_LINE), n > 0) {
        line[n] = '\0';
        printf("fd = %u, 行尾是否为换行符: %d,  read line: %s\n", fd, (line[n-1] == '\n'),  line);

        bufferevent_write(bev, line, n);
    }
}
/**
 * write callback is called whenever enough data from the output buffer is empied to the underlying transport.
 */
void write_cb(struct bufferevent *bev, void *arg)
{
/*
    struct evbuffer *tmp = evbuffer_new();
    evbuffer_add_printf(tmp, "Hello信息");
    bufferevent_write_buffer(bev, tmp);
    evbuffer_free(tmp);
*/
    printf("写入socket完成, write_cb() called.\n");
}

void error_cb(struct bufferevent *bev, short event, void *arg)
{
    evutil_socket_t fd = bufferevent_getfd(bev);
    printf("fd = %u, ", fd);
    if(event & BEV_EVENT_TIMEOUT) {
        printf("Timeout\n"); //if bufferevent_set_timeouts() called
    }
    /*
    else if( event & BEV_EVENT_CONNECTED ) {
        printf("Connection connected\n");
    } */
    else if( event & BEV_EVENT_EOF ) {
        printf("Connection closed\n");
    }
    else if(event & BEV_EVENT_ERROR) {
        printf("some other error\n");
    }
    bufferevent_free(bev);
}
