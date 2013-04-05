/*
  
  If you have the libc 1.6.4 libraries then free space at the end of the
  heap is NOT released.

  With the libc 1.7.1 libraries with the fixed malloc, then free space 
  at the end of the heap is released so that the space is available 
  to the stack or the heap.
  
  AVAIL_MEM is a macro to tell you how much available free memory is left
  between the top of stack and end of heap.  

  STACK_TOP is a macro to tell you the current address of the top of
  the stack.  There is data in this location ??

  STACK_BOTTOM is a macro to tell you the address of the bottom of the
  stack.  It should normally be constant.

  STACK_SIZE is a macro to tell you the current size of the stack

  HEAP_START is a macro to provide the address of the first location in
  the heap.

  HEAP_END is macro to provide the address of the current last location in
  in the heap.  This will increase as memory is allocated, and decrease if
  the end of the heap consists of free memory and can be shrunk.

  HEAP_SIZE is a macro to tell you the current size of the heap, but it
  includes chunks that are free.

  There should be a routine to detect the presence of the old or new malloc.

 */

#ifndef mem_syms_h
#define mem_syms_h

#include "avr/io.h" 

/* these symbols get us to the details of the stack and heap */

// the address of the first byte of the stack, the last byte of onchip SRAM
#define STACK_BOTTOM ( (char*)RAMEND )

// the first free location at the top of the stack
#define STACK_TOP ((char *)AVR_STACK_POINTER_REG)
  
// the amount of stack used.  i.e. from bottom to top-1
#define STACK_SIZE (STACK_BOTTOM - STACK_TOP)

#define AVAIL_MEM (STACK_TOP - HEAP_END)

// the address of the first location of the heap
#define HEAP_START __malloc_heap_start

// the address of the next free location after the end of the heap
// __brkval is 0 until the first malloc
#define HEAP_END (__brkval ? __brkval : __malloc_heap_start) 

// the amount of space taken up by the heap, including free chunks
// inside the heap
#define HEAP_SIZE (HEAP_END - HEAP_START)

extern char*__brkval;

#endif
