/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DummyTitleSequencePlayer.h"

DummyTitleSequencePlayer::DummyTitleSequencePlayer()
{
}	

int32_t DummyTitleSequencePlayer::GetCurrentPosition() const
{
	return 0;
}

bool DummyTitleSequencePlayer::Begin(size_t titleSequenceId)
{
	return false;
}

void DummyTitleSequencePlayer::Reset() 
{
}

bool DummyTitleSequencePlayer::Update() 
{
	return false;
}

void DummyTitleSequencePlayer::Seek(int32_t position)
{
}

void DummyTitleSequencePlayer::Eject()
{
}
