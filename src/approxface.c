#include <pebble.h>
#include "spinnerclock.h"

static Window *window;
static AppTimer* timer = NULL;

static void clock_tick (void* data) {
    spinners_tick();
    timer = NULL;
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    if (tick_time->tm_min % 5 == 2)
    {
        if (tick_time->tm_sec > 0 && tick_time->tm_sec < 30)
            timer = app_timer_register(30000 - (tick_time->tm_sec * 1000), (AppTimerCallback) clock_tick, NULL);
        else
            timer = app_timer_register(30000 - ANIMATION_DURATION / 2, (AppTimerCallback) clock_tick, NULL);
    }
}

static void window_load(Window *window) {
    Layer *windowLayer = window_get_root_layer(window);

    time_t temp = time(NULL);
    struct tm t = *(localtime(&temp));

    spinners_create(windowLayer, &t);

    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void window_unload(Window *window) {
    spinners_destroy();
    tick_timer_service_unsubscribe();
    if (timer)
    {
        app_timer_cancel(timer);
        timer = NULL;
    }
}

static void init(void) {
    window = window_create();
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });


    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}

