#ifndef THREAD_H
#define THREAD_H

#define REAL_TIME_CLASS         0x0
#define GENERAL_PURPOSE_CLASS   0x1

typedef struct ThreadMMap ThreadMMap;
//book keeping of the memory of a thread
struct __attribute__((packed)) ThreadMMap {
    ThreadMMap* previous;
    uint64_t base;
    uint64_t size;
    ThreadMMap* next;
};

typedef struct __attribute__((packed)) Thread {
    uint32_t id;
    uint8_t class;
    uint8_t priority;
    uint16_t padding;
    ThreadMMap* mmap;
} Thread;

extern volatile Thread* thread_queue;

void init_syscalls();

#endif