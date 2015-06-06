/**\file spinnerclock.h
 * \author Dominic Shelton
 * \date 5-6-15
 * Contains functions for setting up and altering a clock comprised of 3 spinners
 */

#include <pebble.h>

#define ANIMATION_DURATION 2000

void spinners_tick(void* data);

void spinners_create(Layer* layer);

void spinners_destroy();

