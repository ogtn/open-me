//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.


#ifndef H_OPENME
#define H_OPENME


// constants
#define OME_FAILURE         -1
#define OME_SUCCESS         0

#define OME_TRUE            1
#define OME_FALSE           0


// engine core functions
int ome_Init(int width, int height);
void ome_Quit(void);
void ome_SwapBuffer(void);
int ome_isWindowOpened(void);


#endif // H_OPENME
