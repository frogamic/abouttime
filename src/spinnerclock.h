/**\file spinnerclock.h
 * \author Dominic Shelton
 * \date 5-6-15
 * Contains functions for setting up and altering a clock comprised of 3 spinners
 */

#include <pebble.h>

#define ANIMATION_DURATION 2000

/** Increments the spinners, should be called every 2.5 minutes.
 *  \param  data    dummy pointer to NULL to maintain compatibility with pebble API.
 *  \brief Must be called after a call to spinners_create().
 */
void spinners_tick(void* data);

/** Creates the text layers, initialises all spinner values, and sets readout to current time.
 *  \param  layer   A pointer to the parent layer on which to draw the spinners.
 *  \param  t       A pointer to a struct holding the current time.
 */
void spinners_create(Layer* layer, struct tm* t);

/** Destroys all resources associated with the spinners.
 */
void spinners_destroy();

