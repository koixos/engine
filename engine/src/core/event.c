#include "event.h"
#include "kmemory.h"
#include "containers/darray.h"

typedef struct registered_event {
    void *listener;
    PFN_on_event callback;
} registered_event;

typedef struct event_code_entry {
    registered_event *events;
} event_code_entry;

#define MAX_MSG_CODES 16384

typedef struct event_sys_state {
    //lookup table for event codes
    event_code_entry registered[MAX_MSG_CODES];
} event_sys_state;

//event sys internal state
static b8 is_inited = FALSE;
static event_sys_state state;

b8 event_init() {
    if (is_inited)
        return FALSE;
    
    kzero_mem(&state, sizeof(state));
    is_inited = TRUE;
    return TRUE;
}

void event_shutdown() {
    //free the events arr and objs pointed to should be destroyed on their own
    for (u16 i = 0; i < MAX_MSG_CODES; i++) {
        if (state.registered[i].events != 0) {
            darray_destroy(state.registered[i].events);
            state.registered[i].events = 0;
        }
    }
}

b8 event_register(u16 code, void *listener, PFN_on_event on_event) {
    if (!is_inited)
        return FALSE;

    if (state.registered[code].events == 0)
        state.registered[code].events = darray_create(registered_event);

    u64 registered_count = darray_length(state.registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        if (state.registered[code].events[i].listener == listener) {
            //TODO: warn - litener already registered to an event
            return FALSE;
        }
    }

    registered_event event;
    event.listener = listener;
    event.callback = on_event;
    darray_push(state.registered[code].events, event);

    return TRUE;
}

b8 event_unregister(u16 code, void *listener, PFN_on_event on_event) {
    if (!is_inited)
        return FALSE;

    if (state.registered[code].events == 0) {
        //TODO: warn - no event registered for this code
        return FALSE;
    }

    u64 registered_count = darray_length(state.registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        registered_event e = state.registered[code].events[i];
        if (e.listener == listener && e.callback == on_event) {
            registered_event popped;
            _darray_pop_at(state.registered[code].events, i, &popped);
            return TRUE;
        }
    }

    //not found
    return FALSE;
}

b8 event_fire(u16 code, void *sender, event_context data) {
    if (!is_inited)
        return FALSE;

    if (state.registered[code].events == 0) 
        return FALSE;
    
    u64 registered_count = darray_length(state.registered[code].events);
    for (u64 i = 0; i < registered_count; i++) {
        registered_event e = state.registered[code].events[i];
        if (e.callback(code, sender, e.listener, data)) {
            //msg handled, dont sent to other listeners
            return TRUE;
        }
    }

    //not found
    return FALSE;
}