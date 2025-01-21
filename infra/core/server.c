#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

// Platform-specific includes
#if defined(__linux__)
    #include <sys/epoll.h>
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__)
    #include <sys/event.h>
    #include <sys/time.h>
#else
    #error "Unsupported platform"
#endif

#define PORT 8080
#define BUFFER_SIZE 4096
#define MAX_EVENTS 10000
#define MAX_THREADS 32
#define BACKLOG 50000

// Event handling abstraction
typedef struct {
    int fd;
    #if defined(__linux__)
        int epoll_fd;
    #else
        int kqueue_fd;
    #endif
} event_loop_t;

typedef struct {
    int fd;
    char buffer[BUFFER_SIZE];
    size_t buffer_used;
    time_t last_active;
    bool keep_alive;
} connection_t;

volatile bool server_running = true;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

// Create event loop
event_loop_t* create_event_loop(void) {
    event_loop_t* loop = malloc(sizeof(event_loop_t));
    if (!loop) return NULL;

    #if defined(__linux__)
        loop->epoll_fd = epoll_create1(0);
        if (loop->epoll_fd < 0) {
            free(loop);
            return NULL;
        }
    #else
        loop->kqueue_fd = kqueue();
        if (loop->kqueue_fd < 0) {
            free(loop);
            return NULL;
        }
    #endif

    return loop;
}

// Add file descriptor to event loop
int add_to_event_loop(event_loop_t* loop, int fd, void* data) {
    #if defined(__linux__)
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;
        ev.data.ptr = data;
        return epoll_ctl(loop->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    #else
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, data);
        return kevent(loop->kqueue_fd, &ev, 1, NULL, 0, NULL);
    #endif
}

// Remove file descriptor from event loop
int remove_from_event_loop(event_loop_t* loop, int fd) {
    #if defined(__linux__)
        return epoll_ctl(loop->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    #else
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        return kevent(loop->kqueue_fd, &ev, 1, NULL, 0, NULL);
    #endif
}

// Wait for events
int wait_for_events(event_loop_t* loop, void* events, int max_events, int timeout_ms) {
    #if defined(__linux__)
        return epoll_wait(loop->epoll_fd, (struct epoll_event*)events, max_events, timeout_ms);
    #else
        struct timespec ts = {
            .tv_sec = timeout_ms / 1000,
            .tv_nsec = (timeout_ms % 1000) * 1000000
        };
        return kevent(loop->kqueue_fd, NULL, 0, (struct kevent*)events, max_events, &ts);
    #endif
}

// Get event data
void* get_event_data(void* event) {
    #if defined(__linux__)
        return ((struct epoll_event*)event)->data.ptr;
    #else
        return ((struct kevent*)event)->udata;
    #endif
}

// Rest of the server implementation remains similar, just using the abstraction layer
// [Previous socket optimization, worker thread, and main function code goes here]
// [For brevity, I'm showing the key differences in the worker thread function]

void* worker_thread(void* arg) {
    event_loop_t* loop = create_event_loop();
    if (!loop) {
        return NULL;
    }

    #if defined(__linux__)
        struct epoll_event events[MAX_EVENTS];
    #else
        struct kevent events[MAX_EVENTS];
    #endif

    while (server_running) {
        int nfds = wait_for_events(loop, events, MAX_EVENTS, 1000);

        for (int i = 0; i < nfds; i++) {
            connection_t* conn = get_event_data(&events[i]);

            #if defined(__linux__)
                if (events[i].events & EPOLLIN) {
            #else
                if (events[i].filter == EVFILT_READ) {
            #endif
                // Handle read event
                ssize_t bytes_read = recv(conn->fd,
                                        conn->buffer + conn->buffer_used,
                                        BUFFER_SIZE - conn->buffer_used,
                                        0);

                if (bytes_read > 0) {
                    // Handle the received data
                    // [Previous handling code remains the same]
                } else if (bytes_read == 0 || (bytes_read < 0 && errno != EAGAIN)) {
                    remove_from_event_loop(loop, conn->fd);
                    close(conn->fd);
                    free(conn);
                }
            }
        }
    }

    // Cleanup
    #if defined(__linux__)
        close(loop->epoll_fd);
    #else
        close(loop->kqueue_fd);
    #endif
    free(loop);
    return NULL;
}
