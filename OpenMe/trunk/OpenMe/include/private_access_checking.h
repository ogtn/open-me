//  ome: Open Minimalistic Engine
//
//  Copyright: (c) 2012-2013
//  Olivier Guittonneau <OpenMEngine@gmail.com>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the Do What The Fuck You Want To
//  Public License, Version 2, as published by Sam Hocevar. See
//  http://sam.zoy.org/projects/COPYING.WTFPL for more details.

#ifndef OME_PRIVACY_START
	#define OME_PRIVACY_START
	#if defined(OME_CHECK_PRIVACY) && !defined(OME_IMPL)
		#define private const
	#else
		#define private
	#endif
	#define public
#else
	#undef private
	#undef public
	#undef OME_IMPL
	#undef OME_PRIVACY_START
#endif
