/***************************************************************************
 *
 *  Copyright (C) 1995-1998 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       vmanpset.h
 *  Content:    Voice Manager property sets.
 ***************************************************************************/

#include "Direct.Sound.Basic.hxx"

#ifndef __VMANPSET_H__
#define __VMANPSET_H__

#ifdef	__cplusplus
extern "C" {
#endif // __cplusplus

    typedef enum
    {
        DSPROPERTY_VMANAGER_MODE = 0,
        DSPROPERTY_VMANAGER_PRIORITY,
        DSPROPERTY_VMANAGER_STATE
    } DSPROPERTY_VMANAGER;

    typedef enum
    {
        DSPROPERTY_VMANAGER_MODE_DEFAULT = 0,
        DSPROPERTY_VMANAGER_MODE_AUTO,
        DSPROPERTY_VMANAGER_MODE_REPORT,
        DSPROPERTY_VMANAGER_MODE_USER
    } VmMode;

    typedef enum
    {
        DSPROPERTY_VMANAGER_STATE_PLAYING3DHW = 0,
        DSPROPERTY_VMANAGER_STATE_SILENT,
        DSPROPERTY_VMANAGER_STATE_BUMPED,
        DSPROPERTY_VMANAGER_STATE_PLAYFAILED
    } VmState;

    // Voice Manager property set identifier {62A69BAE-DF9D-11d1-99A6-00C04FC99D46}
    DEFINE_GUID(DSPROPSETID_VoiceManager, 0x62a69bae, 0xdf9d, 0x11d1, 0x99, 0xa6, 0x0, 0xc0, 0x4f, 0xc9, 0x9d, 0x46);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __VMANPSET_H__