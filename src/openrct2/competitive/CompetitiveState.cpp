/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitiveState.h"

#include <algorithm>

namespace OpenRCT2::Competitive
{
    Participant* FindParticipant(MatchState& state, ParticipantId id)
    {
        const auto found = std::find_if(
            state.participants.begin(), state.participants.end(), [id](const auto& value) { return value.id == id; });
        return found == state.participants.end() ? nullptr : &*found;
    }

    const Participant* FindParticipant(const MatchState& state, ParticipantId id)
    {
        const auto found = std::find_if(
            state.participants.begin(), state.participants.end(), [id](const auto& value) { return value.id == id; });
        return found == state.participants.end() ? nullptr : &*found;
    }

    Score* FindScore(MatchState& state, ParticipantId id)
    {
        const auto found = std::find_if(state.scores.begin(), state.scores.end(), [id](const auto& value) {
            return value.participantId == id;
        });
        return found == state.scores.end() ? nullptr : &*found;
    }

    const Score* FindScore(const MatchState& state, ParticipantId id)
    {
        const auto found = std::find_if(state.scores.begin(), state.scores.end(), [id](const auto& value) {
            return value.participantId == id;
        });
        return found == state.scores.end() ? nullptr : &*found;
    }

    ParticipantReport* FindReport(MatchState& state, ParticipantId id)
    {
        const auto found = std::find_if(state.reports.begin(), state.reports.end(), [id](const auto& value) {
            return value.participantId == id;
        });
        return found == state.reports.end() ? nullptr : &*found;
    }

    const ParticipantReport* FindReport(const MatchState& state, ParticipantId id)
    {
        const auto found = std::find_if(state.reports.begin(), state.reports.end(), [id](const auto& value) {
            return value.participantId == id;
        });
        return found == state.reports.end() ? nullptr : &*found;
    }
} // namespace OpenRCT2::Competitive

