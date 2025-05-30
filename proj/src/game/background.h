#ifndef _BACKGROUND_H_
#define _BACKGROUND_H_

/**
 * @brief Initialize the background
 *
 * @return int 0 if successful, 1 otherwise
 */
int background_init();

/**
 * @brief Draw the background to the current buffer
 *
 * @return int 0 if successful, 1 otherwise
 */
int background_draw();

/**
 * @brief Clean up the background resources
 */
void background_destroy();

#endif // _BACKGROUND_H_
