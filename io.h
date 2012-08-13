#ifndef IO_H
#define IO_H

extern int X_MAX;
extern int Y_MAX;

void init_io(void);
void stop_io(void);
void update_io(void);
void resize_io(int w, int h);

void io_printf(int x, int y, const char *format, ...);
void io_puts(int x, int y, const char *s);
void io_putc(int x, int y, char c);

void io_set_color(int r, int g, int b);
void io_reset_color(void);

void io_clear_screen(void);

#endif /* IO_H */

