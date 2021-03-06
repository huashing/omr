/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 ******************************************************************************/


#ifndef OMR_DEFINES_H
#define OMR_DEFINES_H

/* 
   This is the first file that the OMR code should be including.
   The idea is the define a standard set of C macros based on the
   system configuration. This is the only file (hopefully) needs to
   use system specific macros (e.g. __linux__).
   
   In addition, if there are system-configuration specific feature
   test macros that you want to define, this is probably the file you
   want to define them in. You should also provide documentation of
   what your macro does. As an example see SUPPORTS_THREAD_LOCAL
   
   Note that this file is included is asm code as well. Apart from C
   pre-processor macros, it shouldn't contain anything else.
*/


/* Operating Systems */
#ifndef OMR_LINUX
#  define OMR_LINUX   201
#endif
#ifndef OMR_WINDOWS
#  define OMR_WINDOWS 202
#endif
#ifndef OMR_AIX
#  define OMR_AIX     203
#endif
#ifndef OMR_ZOS
#  define OMR_ZOS     204
#endif

/* Architectures */
#ifndef ARCH_X86
#  define ARCH_X86     101
#endif
#ifndef ARCH_POWER
#  define ARCH_POWER   102
#endif
#ifndef ARCH_ZARCH
#  define ARCH_ZARCH   103
#endif
#ifndef ARCH_ARM
#  define ARCH_ARM     104
#endif

/* Compilers */
#ifndef COMPILER_GCC
#  define COMPILER_GCC 301
#endif
#ifndef COMPILER_XLC
#  define COMPILER_XLC 302
#endif
#ifndef COMPILER_MSVC
#  define COMPILER_MSVC 303
#endif
#ifndef COMPILER_CLANG
#  define COMPILER_CLANG 304
#endif

/*
  Standarize the OS macros Use HOST_OS instead of various different
  ways of checking for a particular host os.  As a reference, see
  http://sourceforge.net/p/predef/wiki/OperatingSystems/
*/
#if defined(__linux__)
#  define HOST_OS OMR_LINUX
#elif defined(_AIX)
#  define HOST_OS OMR_AIX
#elif defined(_WIN32)
#  define HOST_OS OMR_WINDOWS
#elif defined(__MVS__)
#  define HOST_OS OMR_ZOS
#else
#  error "defines.h: unknown OS"
#endif

/* Architecture macros */
/* As a reference see http://sourceforge.net/p/predef/wiki/Architectures */
/* FIXME: rename TR_HOST_32BIT and TR_HOST_64BIT */
#if   defined(__amd64__) || defined(_M_AMD64)
#  define HOST_ARCH ARCH_X86
#  define TR_HOST_64BIT 1
#elif defined(__i386__) || defined(_M_IX86)
#  define HOST_ARCH ARCH_X86
#  define TR_HOST_32BIT 1
#elif defined(__PPC__) || defined(_ARCH_PPC)
#  define HOST_ARCH ARCH_POWER
#  if defined(__PPC64__) || defined(_ARCH_PPC64)
#    define TR_HOST_64BIT 1
#  else
#    define TR_HOST_32BIT 1
#  endif
#elif defined(__s390x__) || (defined(__370__) && defined(__64BIT__))
#  define HOST_ARCH ARCH_ZARCH
#  define TR_HOST_64BIT 1
#elif defined(__s390__)  || (defined(__370__) && !defined(__64BIT__))
#  define HOST_ARCH ARCH_ZARCH
#  define TR_HOST_32BIT 1
#elif defined(__arm__)
#  define HOST_ARCH ARCH_ARM
#  define TR_HOST_32BIT 1
#else
#  error "defines.h: unknown architecture"
#endif

/* Unless the target arch was specified, we are building the same target as the host. */
#ifndef TARGET_ARCH
#  define TARGET_ARCH HOST_ARCH
#endif

/* Compiler macros */
#if defined(__GNUC__)
#  define HOST_COMPILER COMPILER_GCC
#elif defined(__IBMC__) || defined(__IBMCPP__)
#  define HOST_COMPILER COMPILER_XLC
#elif defined(_MSC_VER)
#  define HOST_COMPILER COMPILER_MSVC
#elif defined(__clang__)
#  define HOST_COMPILER COMPILER_CLANG
#else
#  error "defines.h: unknown compiler"
#endif

/* FIXME: we need to deprecate TR_HOST_* macros and replace them with 
   code of the form: #if (HOST_ARCH == ARCH_X86) */
#if (HOST_ARCH == ARCH_X86)
#  define TR_HOST_X86    1
#endif

#if (HOST_ARCH == ARCH_ZARCH)
#  define TR_HOST_S390   1
#endif

#if (HOST_ARCH == ARCH_POWER)
#  define TR_HOST_POWER  1
#endif

#if (HOST_ARCH == ARCH_ARM)
#  define TR_HOST_ARM    1
#endif

/* FIXME: we need to document what these ifdefs do
   If these are permanently enabled, the ifdefs should be removed
   and the guarded code permanently enabled */
#define BITVECTOR_BIT_NUMBERING_MSB   1
#define DUT_DIRECT_TRACE_REGISTRATION 1

#endif /* OMR_DEFINES_H */
