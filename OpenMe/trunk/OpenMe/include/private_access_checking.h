//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012 Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

#ifndef OME_PRIVACY_START
	#define OME_PRIVACY_START
	#if defined(OME_CHECK_PRIVACY) && !defined(OME_IMPL)
		#define ome_cst const
	#else
		#define ome_cst
	#endif
	#define public
#else
	#undef ome_cst
	#undef public
	#undef OME_IMPL
#endif
