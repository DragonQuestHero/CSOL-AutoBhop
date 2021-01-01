#ifndef _SNAP_
#define _SNAP_

void __stdcall m_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);
typedef void(__stdcall* glReadPixels_t)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
extern glReadPixels_t glReadPixels_s;
extern bool DrawVisuals;
void Snapshot();

#endif
