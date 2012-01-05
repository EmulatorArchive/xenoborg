#include "win32.h"
#include "gl_drv.h"


/* OpenGL contexts */
HDC hdc;
HGLRC hrc;

/* Misc */
void* fb = NULL;
uint32_t fb_tex = 0;
uint32_t fb_tex_fmt = GL_BGRA_EXT;
uint32_t fb_tex_target = GL_TEXTURE_2D;


int ogl_init_fb_tex( GLsizei width, GLsizei height, GLuint target, GLenum format )
{
	uint32_t* buf = NULL;

	/* Delete texture if it already exists in memory */
	if( fb_tex )
	{
		glDeleteTextures( 1, &fb_tex );
		fb_tex = 0;
	}

	glEnable( format );

	/* Create the frame buffer texture */
	glGenTextures( 1, &fb_tex );
	glBindTexture( target, fb_tex );

	/* Initialize texture data */
	/* TODO: Verify valid formats, i.e. no 24-bit, 8-bit, etc. */
	fb_tex_fmt = format;
	fb_tex_target = target;

	glTexImage2D( target, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, fb );

	/* Enable bilinear filtering */
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	return 1;
}

void ogl_delete_fb_tex()
{
	/* Delete the frame buffer texture */
	if( fb_tex )
	{
		glDeleteTextures( 1, &fb_tex );
		fb_tex = 0;
	}
}

void ogl_enable2d()
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();

	glOrtho( 0, 640, 480, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
 //  glTranslatef(0.375, 0.375, 0.);

	glPushAttrib( GL_DEPTH_BUFFER_BIT );
	glDisable( GL_DEPTH_TEST );
}

void ogl_disable2d()
{
   glPopAttrib();
   glMatrixMode( GL_PROJECTION );
   glPopMatrix();   
   glMatrixMode( GL_MODELVIEW );
   glPopMatrix();       
}

/* Initialize OpenGL */
int		enable_ogldrv( struct window_t* wnd )
{
	PIXELFORMATDESCRIPTOR pfd;
	int pixel_format = 0;

	if( !wnd )
		return 0;

	/* Initialize OpenGL */
	memset( &pfd, 0, sizeof(pfd) );
	pfd.nVersion = 1;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 16;
	pfd.cStencilBits = 8;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.nSize = sizeof(pfd);

	hdc = GetDC( wnd->hwnd );
	if( !hdc )
		return 0;

	pixel_format = ChoosePixelFormat( hdc, &pfd );
	if( !pixel_format )
		return 0;

	if( !SetPixelFormat( hdc, pixel_format, &pfd ) )
		return 0;

	hrc = wglCreateContext(hdc);
	if( !hrc )
		return 0;

	if( !wglMakeCurrent( hdc, hrc ) )
		return 0;

	/* Set up viewport and projection matrix */
	glViewport( 0, 0, 640, 480 );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluPerspective( 45.0f, 640/480, 0.1, 1000.0f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	/* Set up default render states */
	glEnable( GL_DEPTH_TEST );
	glDepthFunc( GL_LEQUAL );
	glClearDepth( 1.0f );

	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );
	glDisable( GL_BLEND );
	glDisable( GL_DITHER );
	glDisable( GL_ALPHA_TEST );

	/* Allocate framebuffer pointer */
//	fb = malloc( 640*480*4 );
	fb = malloc( 4*1024*1024 );
	if(!fb)
		return 0;

//	memset( fb, 0, 640*480*4 );
	memset( fb, 0, 4*1024*1024 );

	/* Create framebuffer texture */
//	ogl_init_fb_tex( 640, 480, GL_TEXTURE_RECTANGLE_NV, GL_BGRA_EXT );

	return 1;
}

/* Uninitialize OpenGL */
void	disable_ogldrv()
{
//	ogl_delete_fb_tex();

	if(fb)
	{
		free(fb);
		fb = NULL;
	}

	if( hrc )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( hrc );
		hrc = NULL;
	}

	if( hdc )
	{
		ReleaseDC( GetActiveWindow(), hdc );
		hdc = NULL;
	}
}

/* Clear the colour buffer */
void	ogl_clear_colour_buffer()
{
	glClear( GL_COLOR_BUFFER_BIT );
}

void	ogl_clear_depth_buffer()
{
	glClear( GL_DEPTH_BUFFER_BIT );
}

void	ogl_clear_stencil_buffer()
{
	glClear( GL_STENCIL_BUFFER_BIT );
}

void	ogl_swap_buffers()
{
	SwapBuffers( hdc );
}

void	ogl_draw_to_fb32( int x, int y, uint32_t colour )
{
	unsigned char r = (colour>>16)&0xff;
	unsigned char g = (colour>> 8)&0xff;
	unsigned char b = (colour>> 0)&0xff;

	if(!fb) return;

	((unsigned int*)fb)[x+(y*640)] = (0xff<<24)|(b<<16)|(g<<8)|(r);
	//((uint32_t*)fb)[x+(y*640)] = colour;
}

uint32_t ogl_read_from_fb32( int x, int y )
{
	uint32_t colour = ((uint32_t*)fb)[x+(y*640)];
	unsigned char b = (colour>>16)&0xff;
	unsigned char g = (colour>> 8)&0xff;
	unsigned char r = (colour>> 0)&0xff;

	if(!fb) return 0;

	return (0xff<<24)|(b<<16)|(g<<8)|(r);
}

void	ogl_update_fb()
{
	if(!fb) return;
	
#if 1
	glDrawPixels( 640, 480, GL_BGRA_EXT, GL_UNSIGNED_BYTE, fb );
#else
	ogl_enable2d();
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable( fb_tex_fmt );
//	glEnable( GL_BLEND );
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glBindTexture( fb_tex_target, fb_tex );
	//	glTexSubImage2D( fb_tex_target, 0, 0, 0, 640, 480, fb_tex_fmt, GL_UNSIGNED_BYTE, fb );
	glTexImage2D( fb_tex_target, 0, fb_tex_fmt, 640, 480, 0, fb_tex_fmt, GL_UNSIGNED_BYTE, fb );
	glBegin( GL_QUADS );
		glColor3f( 1.0f, 1.0f, 1.0f );
		glTexCoord2f( 0.0f, 480.0f ); glVertex2f( 0, 0 );
		glTexCoord2f( 640.0f, 480.0f ); glVertex2f( 640, 0 );
		glTexCoord2f( 640.0f, 0.0f ); glVertex2f( 640, 480 );
		glTexCoord2f( 0.0f, 0.0f ); glVertex2f( 0, 480 );
	glEnd();
	glPopAttrib();
	ogl_disable2d();
#endif
}

void*	ogl_get_fb()
{
	return fb;
}