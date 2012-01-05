#pragma once

#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glext.h>
#include <gl\wglext.h>


/* OpenGL based routines */
int		enable_ogldrv( struct window_t* wnd );
void	disable_ogldrv();
void	ogl_clear_colour_buffer();
void	ogl_clear_depth_buffer();
void	ogl_clear_stencil_buffer();
void	ogl_swap_buffers();
void	ogl_draw_to_fb32( int x, int y, uint32_t colour );
uint32_t ogl_read_from_fb32( int x, int y );
void	ogl_update_fb();
void*	ogl_get_fb();