#include "spinnerclock.h"

#define TEXT_HEIGHT 28
#define TEXT_TAIL_HEIGHT 10
#define TOP_Y 37
#define CENTER_X 0
#define LEFT_X -144
#define RIGHT_X 144

// Keep pointer to parent layer.
static Layer* parentLayer;

// Primary text layers.
static TextLayer* textLayer[3];

// Secondary text layers for animation
static TextLayer* textLayer_alt[3] = { NULL };

// The current value of all 3 spinners
static int spinnerValue[3];

// The color scheme to use based on pebble model.
static GColor fgColor;

// Clunky way of making a non-square 2d array.
static const char* text0[] = {"It's", "Five", "Ten", "Quarter", "Twenty", "Twenty-five", "Half"};
static const char* text1[] = {"about", "past", "to"};
static const char* text2[] = {"twelve", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", "eleven"};
static const char** spinnerText[3] = {text0, text1, text2};

/** Creates a new TextLayer at specified x and y with same width as parent layer and height of text.
 *  Must be called after spinners_create().
 *  \param  text    Pointer to a string to set the layers text to.
 *  \param  x       The x value of the left edge of the layer.
 *  \param  y       The y value of the top edge of the layer.
 *  \return         A pointer to the newly created TextLayer object.
 */
static TextLayer* create_spinner_text(int spinner, int x, int y) {
    // Create the TextLayer using specified coordinates and geometry from parent layer.
    TextLayer* layer = text_layer_create((GRect) {
                .origin = { x, y },
                .size = { layer_get_bounds(parentLayer).size.w, TEXT_HEIGHT + TEXT_TAIL_HEIGHT}
            });
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "Created text layer: %p", layer);
    // Set the text to centered.
    text_layer_set_text_alignment(layer, GTextAlignmentCenter);
    // Make it see through so text tails won't get covered.
    text_layer_set_background_color(layer, GColorClear);
    // Set the text color to the fg color.
    text_layer_set_text_color(layer, fgColor);
    // Set the biggest font capable of displaying "Twenty-five".
    text_layer_set_font(layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    // Set the actual text.
    if (spinner != 0 || spinnerValue[spinner] < 7) {
        // The simple case that the text can simply be indexed.
        text_layer_set_text(layer, spinnerText[spinner][spinnerValue[spinner]]);
    } else {
        // The complex case where the first spinner is counting down again.
        text_layer_set_text(layer, spinnerText[spinner][12-spinnerValue[spinner]]);
    }
    //Add text layers to window layer
    layer_add_child(parentLayer, text_layer_get_layer(layer));

    return layer;
}

/** Deletes the alt text layer and associated animation after rotating off screen.
 */
static void animation_stopped(Animation* anim, bool finished, void* context) {
    // Free the memory used by the animation.
    property_animation_destroy((PropertyAnimation*) anim);
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed property animation: %p", anim);
    // Destroy the alt text if this animation rotated it offscreen.
    if((int) context != 255){
        text_layer_destroy(textLayer_alt[(int) context]);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed text layer: %p", textLayer_alt[(int) context]);
        textLayer_alt[(int) context] = NULL;
    }
}

/** Moves the old layer off the edge of the screen and the new layer into
 *  the old layers position.
 */
static void animate_layers(Layer* oldLayer, Layer* newLayer, int targetx, int spinner) {
    // Frame of the outgoing layer (target location for new layer).
    GRect oldFrame = layer_get_frame(oldLayer);
    // target frame for old layer (off screen, opposite to new layer).
    GRect offFrame = {
            .origin = { targetx, oldFrame.origin.y },
            .size = oldFrame.size
        };
    // Create animation to rotate the old layer off.
    PropertyAnimation *old = property_animation_create_layer_frame(oldLayer,
            NULL, &offFrame);
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "Created property animation: %p", old);
    // Create animation to rotate the new layer onto the screen.
    PropertyAnimation *new = property_animation_create_layer_frame(newLayer,
            NULL, &oldFrame);
    // APP_LOG(APP_LOG_LEVEL_DEBUG, "Created property animation: %p", new);
    // Set characteristics of both animations.
    animation_set_duration((Animation*) old, ANIMATION_DURATION);
    animation_set_duration((Animation*) new, ANIMATION_DURATION);
    animation_set_curve((Animation*) old, AnimationCurveEaseInOut);
    animation_set_curve((Animation*) new, AnimationCurveEaseInOut);
    // Create and assign animation handlers.
    AnimationHandlers h = {
        .stopped = (AnimationStoppedHandler) animation_stopped
    };
    animation_set_handlers((Animation*) old, h, (void*) spinner);
    animation_set_handlers((Animation*) new, h, (void*) 255);
    // Start the animations.
    animation_schedule((Animation*) old);
    animation_schedule((Animation*) new);
}

static void spinner_increase(int spinner, int newx, int targetx) {
    int rollover[] = {12, 3, 12};
    // Increase the value of the spinner rolling over if above 12.
    spinnerValue[spinner] = (1 + spinnerValue[spinner]) % rollover[spinner];
    // Put the old layer pointer into the alt variable to be rotated off.
    textLayer_alt[spinner] = textLayer[spinner];
    // Create a new layer with the correct x position to be rotated on.
    textLayer[spinner] = create_spinner_text(spinner, newx, TOP_Y + (spinner * TEXT_HEIGHT));
    animate_layers((Layer*) textLayer_alt[spinner], (Layer*) textLayer[spinner], targetx, spinner);
}

/** Increases the top spinner of the three. Visually decreases the spinner and
 *  triggers the middle spinner where necessary.
 */
void spinners_tick(void* data) {
    int x, targetx;
    // At these values the middle spinner must also be increased.
    if (spinnerValue[0] == 0 || spinnerValue[0] == 11) {
        spinner_increase(1, RIGHT_X, LEFT_X);
    }
    else if (spinnerValue[0] == 6) {
        // At this value both other spinners must be increased.
        spinner_increase(1, RIGHT_X, LEFT_X);
        spinner_increase(2, RIGHT_X, LEFT_X);
    }
    // Find the direction of rotation.
    if (spinnerValue[0] < 6) {
        x = RIGHT_X;
        targetx = LEFT_X;
    } else {
        x = LEFT_X;
        targetx = RIGHT_X;
    }
    spinner_increase(0, x, targetx);
}

void spinners_create(Layer* layer, struct tm* t) {
    // Save pointer to parent layer.
    parentLayer = layer;
    GColor bgColor;
    // Get the watch info for setting the color scheme.
    WatchInfoColor color = watch_info_get_color();

    switch(color) {
        case WATCH_INFO_COLOR_TIME_WHITE:
        case WATCH_INFO_COLOR_WHITE:
        case WATCH_INFO_COLOR_PINK:
        case WATCH_INFO_COLOR_BLUE:
        case WATCH_INFO_COLOR_GREEN:
        case WATCH_INFO_COLOR_ORANGE:
            bgColor = GColorWhite;
            fgColor = GColorBlack;
            break;
#ifdef PBL_COLOR
        case WATCH_INFO_COLOR_TIME_RED:
            bgColor = GColorBlack;
            fgColor = GColorDarkCandyAppleRed;
            break;
        case WATCH_INFO_COLOR_TIME_STEEL_GOLD:
            bgColor = GColorBlack;
            fgColor = GColorFromRGB(219,195,127);
            break;
        case WATCH_INFO_COLOR_TIME_STEEL_BLACK:
        case WATCH_INFO_COLOR_TIME_BLACK:
            bgColor = GColorBlack;
            fgColor = GColorLightGray;
            break;
#endif //PBL_COLOR
        default:
            bgColor = GColorBlack;
            fgColor = GColorWhite;
    }
    // Set the background color of the parent layer
    window_set_background_color(layer_get_window(layer), bgColor);

    // Set spinner values for current time.
    if (t->tm_sec >= 30)
        t->tm_min += 1;
    if (t->tm_min > 33)
        t->tm_hour++;
    t->tm_min = (t->tm_min + 2) % 60;
    spinnerValue[0] = t->tm_min / 5;
    if (spinnerValue[0] == 0) {
        spinnerValue[1] = 0;
    }
    else if (spinnerValue[0] < 7) {
        spinnerValue[1] = 1;
    }
    else {
        spinnerValue[1] = 2;
    }
    spinnerValue[2] = t->tm_hour % 12;

    for (int i = 0; i < 3; i++) {
        // Create text layers
        textLayer[i] = create_spinner_text(i, CENTER_X, TOP_Y + i * TEXT_HEIGHT);
    }
}

void spinners_destroy(Layer* layer) {
    for (int i = 0; i < 3; i++) {
        text_layer_destroy(textLayer[i]);
        // APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed text layer: %p", textLayer[i]);
        if (textLayer_alt[i]) {
            text_layer_destroy(textLayer_alt[i]);
            // APP_LOG(APP_LOG_LEVEL_DEBUG, "Destroyed text layer: %p", textLayer_alt[i]);
        }
    }
}

