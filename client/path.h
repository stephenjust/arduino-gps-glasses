#ifndef PATH_H
#define PATH_H

extern int16_t path_errno;

uint8_t read_path(uint16_t *length_p, coord_t *path_p[]);
void draw_path(uint16_t length, coord_t path[]);
uint8_t is_coord_visible(coord_t point);

#endif
