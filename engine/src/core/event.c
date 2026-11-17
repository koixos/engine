#include "event.h"
#include "kmemory.h"
#include "logger.h"

#include "containers/darray.h"

#define MAX_MSG_CODES 16384

typedef struct registered_event {
    void *listener;
    PFN_on_event callback;
} registered_event;

typedef struct event_code_entry {
    registered_event *events;
} event_code_entry;

typedef struct event_sys_state {
    //lookup table for event codes
    event_code_entry registered[MAX_MSG_CODES];
} event_sys_state;

static event_sys_state *state_ptr;

b8 init_event_sys(u64 *mem_requirement, void *st) {
    //TODO: alw returns true
    *mem_requirement = sizeof(event_sys_state);

    if (st == 0)
        return false;
    
    kzero_mem(st, sizeof(st));
    state_ptr = st;

    KINFO("Event subsystem initialized.");
    return true;
}

void event_sys_shutdown(void *st) {
    if (!state_ptr)
        return;

    //free the events arr and objs pointed to should be destroyed on their own
    for (u16 i = 0; i < MAX_MSG_CODES; i++) {
        if (state_ptr->registered[i].events != 0) {
            darray_destroy(state_ptr->registered[i].events);
            state_ptr->registered[i].events = 0;
        }
    }

    state_ptr = 0;
}

b8 event_register(u16 code, void *listener, PFN_on_event on_event) {
    if (!state_ptr)
        return false;

    if (state_ptr->registered[code].events == 0)
        state_ptr->registered[code].events = darray_create(registered_event);

    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        if (state_ptr->registered[code].events[i].listener == listener) {
            //TODO: warn - litener already registered to an event
            return false;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = on_event;

    darray_push(state_ptr->registered[code].events, event);

    return true;
}

b8 event_unregister(u16 code, void *listener, PFN_on_event on_event) {
    if (!state_ptr)
        return false;

    if (state_ptr->registered[code].events == 0) {
        //TODO: warn - no event registered for this code
        return false;
    }

    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        registered_event e = state_ptr->registered[code].events[i];
        if (e.listener == listener && e.callback == on_event) {
            registered_event popped;
            darray_pop_at(state_ptr->registered[code].events, i, &popped);
            return true;
        }
    }

    //not found
    return false;
}

b8 event_fire(u16 code, void *sender, event_context data) {
    if (!state_ptr || state_ptr->registered[code].events == 0)
        return false;
    
    u64 registered_count = darray_length(state_ptr->registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        registered_event e = state_ptr->registered[code].events[i];
        if (e.callback(code, sender, e.listener, data)) {
            //msg handled, dont sent to other listeners
            return true;
        }
    }

    //not found
    return false;
}