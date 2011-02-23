/*
    NanoÉdit: Un modeleur algébrique interactif.
    Copyright (C) 1996-2004 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: Thierry.EXCOFFIER@liris.univ-lyon1.fr
*/
/*
 * Créé le Thu Jul 20 11:10:08 2000 par Thierry EXCOFFIER
 */

#ifndef _HOME_EXCO_NE_1_11_UTILITAIRES_ENLEVE_GL_H
#define _HOME_EXCO_NE_1_11_UTILITAIRES_ENLEVE_GL_H

#include "glu.h"
#include "glx.h"

#define GL_H
#define GLU_H
#define GLX_H


#define glClearIndex
#define glClearColor
#define glClear
#define glIndexMask
#define glColorMask
#define glAlphaFunc
#define glBlendFunc
#define glLogicOp
#define glCullFace
#define glFrontFace
#define glPointSize
#define glLineWidth
#define glLineStipple
#define glPolygonMode
#define glPolygonOffset
#define glPolygonStipple
#define glGetPolygonStipple
#define glEdgeFlag
#define glEdgeFlagv
#define glScissor
#define glClipPlane
#define glGetClipPlane
#define glDrawBuffer
#define glReadBuffer
#define glEnable
#define glDisable
#define glIsEnabled
#define glEnableClientState
#define glDisableClientState
#define glGetBooleanv
#define glGetDoublev
#define glGetFloatv
#define glGetIntegerv
#define glPushAttrib
#define glPopAttrib()
#define glPushClientAttrib
#define glPopClientAttrib
#define glRenderMode
#define glGetError
#define glGetString
#define glFinish
#define glFlush
#define glHint
#define glClearDepth
#define glDepthFunc
#define glDepthMask
#define glDepthRange
#define glClearAccum
#define glAccum
#define glMatrixMode
#define glOrtho
#define glFrustum
#define glViewport
#define glPushMatrix()
#define glPopMatrix()
#define glLoadIdentity()
#define glLoadMatrixd
#define glLoadMatrixf
#define glMultMatrixd
#define glMultMatrixf
#define glRotated
#define glRotatef
#define glScaled
#define glScalef
#define glTranslated
#define glTranslatef
#define glIsList
#define glDeleteLists
#define glGenLists
#define glNewList
#define glEndList()
#define glCallList
#define glCallLists
#define glListBase
#define glBegin
#define glEnd()
#define glVertex2d
#define glVertex2f
#define glVertex2i
#define glVertex2s
#define glVertex3d
#define glVertex3f
#define glVertex3i
#define glVertex3s
#define glVertex4d
#define glVertex4f
#define glVertex4i
#define glVertex4s
#define glVertex2dv
#define glVertex2fv
#define glVertex2iv
#define glVertex2sv
#define glVertex3dv
#define glVertex3fv
#define glVertex3iv
#define glVertex3sv
#define glVertex4dv
#define glVertex4fv
#define glVertex4iv
#define glVertex4sv
#define glNormal3b
#define glNormal3d
#define glNormal3f
#define glNormal3i
#define glNormal3s
#define glNormal3bv
#define glNormal3dv
#define glNormal3fv
#define glNormal3iv
#define glNormal3sv
#define glIndexd
#define glIndexf
#define glIndexi
#define glIndexs
#define glIndexub
#define glIndexdv
#define glIndexfv
#define glIndexiv
#define glIndexsv
#define glIndexubv
#define glColor3b
#define glColor3d
#define glColor3f
#define glColor3i
#define glColor3s
#define glColor3ub
#define glColor3ui
#define glColor3us
#define glColor4b
#define glColor4d
#define glColor4f
#define glColor4i
#define glColor4s
#define glColor4ub
#define glColor4ui
#define glColor4us
#define glColor3bv
#define glColor3dv
#define glColor3fv
#define glColor3iv
#define glColor3sv
#define glColor3ubv
#define glColor3uiv
#define glColor3usv
#define glColor4bv
#define glColor4dv
#define glColor4fv
#define glColor4iv
#define glColor4sv
#define glColor4ubv
#define glColor4uiv
#define glColor4usv
#define glTexCoord1d
#define glTexCoord1f
#define glTexCoord1i
#define glTexCoord1s
#define glTexCoord2d
#define glTexCoord2f
#define glTexCoord2i
#define glTexCoord2s
#define glTexCoord3d
#define glTexCoord3f
#define glTexCoord3i
#define glTexCoord3s
#define glTexCoord4d
#define glTexCoord4f
#define glTexCoord4i
#define glTexCoord4s
#define glTexCoord1dv
#define glTexCoord1fv
#define glTexCoord1iv
#define glTexCoord1sv
#define glTexCoord2dv
#define glTexCoord2fv
#define glTexCoord2iv
#define glTexCoord2sv
#define glTexCoord3dv
#define glTexCoord3fv
#define glTexCoord3iv
#define glTexCoord3sv
#define glTexCoord4dv
#define glTexCoord4fv
#define glTexCoord4iv
#define glTexCoord4sv
#define glRasterPos2d
#define glRasterPos2f
#define glRasterPos2i
#define glRasterPos2s
#define glRasterPos3d
#define glRasterPos3f
#define glRasterPos3i
#define glRasterPos3s
#define glRasterPos4d
#define glRasterPos4f
#define glRasterPos4i
#define glRasterPos4s
#define glRasterPos2dv
#define glRasterPos2fv
#define glRasterPos2iv
#define glRasterPos2sv
#define glRasterPos3dv
#define glRasterPos3fv
#define glRasterPos3iv
#define glRasterPos3sv
#define glRasterPos4dv
#define glRasterPos4fv
#define glRasterPos4iv
#define glRasterPos4sv
#define glRectd
#define glRectf
#define glRecti
#define glRects
#define glRectdv
#define glRectfv
#define glRectiv
#define glRectsv
#define glVertexPointer
#define glNormalPointer
#define glColorPointer
#define glIndexPointer
#define glTexCoordPointer
#define glEdgeFlagPointer
#define glGetPointerv
#define glArrayElement
#define glDrawArrays
#define glDrawElements
#define glInterleavedArrays
#define glShadeModel
#define glLightf
#define glLighti
#define glLightfv
#define glLightiv
#define glGetLightfv
#define glGetLightiv
#define glLightModelf
#define glLightModeli
#define glLightModelfv
#define glLightModeliv
#define glMaterialf
#define glMateriali
#define glMaterialfv
#define glMaterialiv
#define glGetMaterialfv
#define glGetMaterialiv
#define glColorMaterial
#define glPixelZoom
#define glPixelStoref
#define glPixelStorei
#define glPixelTransferf
#define glPixelTransferi
#define glPixelMapfv
#define glPixelMapuiv
#define glPixelMapusv
#define glGetPixelMapfv
#define glGetPixelMapuiv
#define glGetPixelMapusv
#define glBitmap
#define glReadPixels
#define glDrawPixels
#define glCopyPixels
#define glStencilFunc
#define glStencilMask
#define glStencilOp
#define glClearStencil
#define glTexGend
#define glTexGenf
#define glTexGeni
#define glTexGendv
#define glTexGenfv
#define glTexGeniv
#define glGetTexGendv
#define glGetTexGenfv
#define glGetTexGeniv
#define glTexEnvf
#define glTexEnvi
#define glTexEnvfv
#define glTexEnviv
#define glGetTexEnvfv
#define glGetTexEnviv
#define glTexParameterf
#define glTexParameteri
#define glTexParameterfv
#define glTexParameteriv
#define glGetTexParameterfv
#define glGetTexParameteriv
#define glGetTexLevelParameterfv
#define glGetTexLevelParameteriv
#define glTexImage1D
#define glTexImage2D
#define glGetTexImage
#define glGenTextures
#define glDeleteTextures
#define glBindTexture
#define glPrioritizeTextures
#define glAreTexturesResident
#define glIsTexture
#define glTexSubImage1D
#define glTexSubImage2D
#define glCopyTexImage1D
#define glCopyTexImage2D
#define glCopyTexSubImage1D
#define glCopyTexSubImage2D
#define glMap1d
#define glMap1f
#define glMap2d
#define glMap2f
#define glGetMapdv
#define glGetMapfv
#define glGetMapiv
#define glEvalCoord1d
#define glEvalCoord1f
#define glEvalCoord1dv
#define glEvalCoord1fv
#define glEvalCoord2d
#define glEvalCoord2f
#define glEvalCoord2dv
#define glEvalCoord2fv
#define glMapGrid1d
#define glMapGrid1f
#define glMapGrid2d
#define glMapGrid2f
#define glEvalPoint1
#define glEvalPoint2
#define glEvalMesh1
#define glEvalMesh2
#define glFogf
#define glFogi
#define glFogfv
#define glFogiv
#define glFeedbackBuffer
#define glPassThrough
#define glSelectBuffer
#define glInitNames()
#define glLoadName
#define glPushName
#define glPopName()
#define glBlendEquationEXT
#define glBlendColorEXT
#define glPolygonOffsetEXT
#define glVertexPointerEXT
#define glNormalPointerEXT
#define glColorPointerEXT
#define glIndexPointerEXT
#define glTexCoordPointerEXT
#define glEdgeFlagPointerEXT
#define glGetPointervEXT
#define glArrayElementEXT
#define glDrawArraysEXT
#define glGenTexturesEXT
#define glDeleteTexturesEXT
#define glBindTextureEXT
#define glPrioritizeTexturesEXT
#define glAreTexturesResidentEXT
#define glIsTextureEXT
#define glTexImage3DEXT
#define glTexSubImage3DEXT
#define glCopyTexSubImage3DEXT
#define glColorTableEXT
#define glColorSubTableEXT
#define glGetColorTableEXT
#define glGetColorTableParameterfvEXT
#define glGetColorTableParameterivEXT
#define glActiveTextureARB
#define glClientActiveTextureARB
#define glMultiTexCoord1dARB
#define glMultiTexCoord1dvARB
#define glMultiTexCoord1fARB
#define glMultiTexCoord1fvARB
#define glMultiTexCoord1iARB
#define glMultiTexCoord1ivARB
#define glMultiTexCoord1sARB
#define glMultiTexCoord1svARB
#define glMultiTexCoord2dARB
#define glMultiTexCoord2dvARB
#define glMultiTexCoord2fARB
#define glMultiTexCoord2fvARB
#define glMultiTexCoord2iARB
#define glMultiTexCoord2ivARB
#define glMultiTexCoord2sARB
#define glMultiTexCoord2svARB
#define glMultiTexCoord3dARB
#define glMultiTexCoord3dvARB
#define glMultiTexCoord3fARB
#define glMultiTexCoord3fvARB
#define glMultiTexCoord3iARB
#define glMultiTexCoord3ivARB
#define glMultiTexCoord3sARB
#define glMultiTexCoord3svARB
#define glMultiTexCoord4dARB
#define glMultiTexCoord4dvARB
#define glMultiTexCoord4fARB
#define glMultiTexCoord4fvARB
#define glMultiTexCoord4iARB
#define glMultiTexCoord4ivARB
#define glMultiTexCoord4sARB
#define glMultiTexCoord4svARB
#define glPointParameterfEXT
#define glPointParameterfvEXT
#define glBlendFuncSeparateINGR
#define glWindowPos2iMESA
#define glWindowPos2sMESA
#define glWindowPos2fMESA
#define glWindowPos2dMESA
#define glWindowPos2ivMESA
#define glWindowPos2svMESA
#define glWindowPos2fvMESA
#define glWindowPos2dvMESA
#define glWindowPos3iMESA
#define glWindowPos3sMESA
#define glWindowPos3fMESA
#define glWindowPos3dMESA
#define glWindowPos3ivMESA
#define glWindowPos3svMESA
#define glWindowPos3fvMESA
#define glWindowPos3dvMESA
#define glWindowPos4iMESA
#define glWindowPos4sMESA
#define glWindowPos4fMESA
#define glWindowPos4dMESA
#define glWindowPos4ivMESA
#define glWindowPos4svMESA
#define glWindowPos4fvMESA
#define glWindowPos4dvMESA
#define glResizeBuffersMESA
#define glDrawRangeElements
#define glTexImage3D
#define glTexSubImage3D
#define glCopyTexSubImage3D
#define glColorTable
#define glColorSubTable
#define glColorTableParameteriv
#define glColorTableParameterfv
#define glCopyColorSubTable
#define glCopyColorTable
#define glGetColorTable
#define glGetColorTableParameterfv
#define glGetColorTableParameteriv
#define glBlendEquation
#define glBlendColor
#define glHistogram
#define glResetHistogram
#define glGetHistogram
#define glGetHistogramParameterfv
#define glGetHistogramParameteriv
#define glMinmax
#define glResetMinmax
#define glGetMinmax
#define glGetMinmaxParameterfv
#define glGetMinmaxParameteriv
#define glConvolutionFilter1D
#define glConvolutionFilter2D
#define glConvolutionParameterf
#define glConvolutionParameterfv
#define glConvolutionParameteri
#define glConvolutionParameteriv
#define glCopyConvolutionFilter1D
#define glCopyConvolutionFilter2D
#define glGetConvolutionFilter
#define glGetConvolutionParameterfv
#define glGetConvolutionParameteriv
#define glSeparableFilter2D
#define glGetSeparableFilter
#define glLockArraysEXT
#define glUnlockArraysEXT
#define gluLookAt
#define gluOrtho2D
#define gluPerspective
#define gluPickMatrix
#define gluProject
#define gluUnProject
#define gluErrorString
#define gluScaleImage
#define gluBuild1DMipmaps
#define gluBuild2DMipmaps
#define gluNewQuadric
#define gluDeleteQuadric
#define gluQuadricDrawStyle
#define gluQuadricOrientation
#define gluQuadricNormals
#define gluQuadricTexture
#define gluQuadricCallback
#define gluCylinder
#define gluSphere
#define gluDisk
#define gluPartialDisk
#define gluNewNurbsRenderer
#define gluDeleteNurbsRenderer
#define gluLoadSamplingMatrices
#define gluNurbsProperty
#define gluGetNurbsProperty
#define gluBeginCurve
#define gluEndCurve
#define gluNurbsCurve
#define gluBeginSurface
#define gluEndSurface
#define gluNurbsSurface
#define gluBeginTrim
#define gluEndTrim
#define gluPwlCurve
#define gluNurbsCallback
#define gluNewTess
#define gluDeleteTess
#define gluTessBeginPolygon
#define gluTessBeginContour
#define gluTessVertex
#define gluTessEndContour
#define gluTessEndPolygon
#define gluTessProperty
#define gluTessNormal
#define gluTessCallback
#define gluGetTessProperty
#define gluBeginPolygon
#define gluNextContour
#define gluEndPolygon
#define gluGetString


#endif
