/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackPieceSelection.h"

#include "../core/BitSet.hpp"
#include "../core/EnumUtils.hpp"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "ted/PitchAndRoll.h"
#include "ted/TrackElementDescriptor.h"
#include "ted/TrackGroup.h"

#include <array>
#include <cstdint>
#include <sfl/static_vector.hpp>

using namespace OpenRCT2::TrackMetadata;

namespace OpenRCT2
{
    namespace
    {
        struct TrackTransitionDescriptor
        {
            bool startsDiagonally;
            TrackPitch slopeStart;
            TrackRoll rollStart;
            TrackCurve trackCurve;
            TrackPitch slopeEnd;
            TrackRoll rollEnd;
            TrackElemType trackElement;
        };

        // clang-format off
        static constexpr TrackTransitionDescriptor kNextSelectedPiece[186] = {
            {   true,   TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::diagDown60                              },
            {   true,   TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagDown60ToDown25                      },
            {   true,   TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagDown60ToFlat                        },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::diagDown25ToDown60                      },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagDown25                              },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagDown25ToLeftBank                    },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagDown25ToFlat                        },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagDown25ToRightBank                   },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftEighthBankToOrthogonal              },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagLeftBankToDown25                    },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagLeftBank                            },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagLeftBankToFlat                      },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagLeftBankToUp25                      },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::none,    TrackElemType::leftEighthToOrthogonal                  },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::diagFlatToDown60                        },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagFlatToDown25                        },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagFlatToLeftBank                      },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagFlat                                },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagFlatToRightBank                     },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagFlatToUp25                          },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::diagFlatToUp60                          },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::none,    TrackElemType::rightEighthToOrthogonal                 },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagRightBankToDown25                   },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagRightBankToFlat                     },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagRightBank                           },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagRightBankToUp25                     },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightEighthBankToOrthogonal             },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagUp25ToLeftBank                      },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagUp25ToFlat                          },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagUp25ToRightBank                     },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagUp25                                },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::diagUp25ToUp60                          },
            {   true,   TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagUp60ToFlat                          },
            {   true,   TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagUp60ToUp25                          },
            {   true,   TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::diagUp60                                },
            {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::down90,   TrackRoll::none,    TrackElemType::leftQuarterTurn1TileDown90              },
            {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down90,   TrackRoll::none,    TrackElemType::down90                                  },
            {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::down90ToDown60                          },
            {   false,  TrackPitch::down90,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::down90,   TrackRoll::none,    TrackElemType::rightQuarterTurn1TileDown90             },
            {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::down60,   TrackRoll::none,    TrackElemType::leftQuarterTurn1TileDown60              },
            {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down90,   TrackRoll::none,    TrackElemType::down60ToDown90                          },
            {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::down60                                  },
            {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::down60ToDown25                          },
            {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::down60ToFlat                            },
            {   false,  TrackPitch::down60,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::down60,   TrackRoll::none,    TrackElemType::rightQuarterTurn1TileDown60             },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftBankedQuarterTurn3TileDown25        },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::left,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftBankedQuarterTurn5TileDown25        },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::down25LeftBanked                        },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftBankedDown25ToDown25                },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBankedDown25ToLeftBankedFlat        },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftBankedDown25ToFlat                  },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftQuarterTurn3TilesDown25             },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank   },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::left,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftQuarterTurn5TilesDown25             },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::down25ToDown60                          },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::down25ToLeftBankedDown25                },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::down25                                  },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::down25ToRightBankedDown25               },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::down25ToLeftBank                        },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::down25ToFlat                            },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::down25ToRightBank                       },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::right,            TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightQuarterTurn5TilesDown25            },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightQuarterTurn3TilesDown25ToRightBank },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightQuarterTurn3TilesDown25            },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightBankedDown25ToDown25               },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::down25RightBanked                       },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::rightBankedDown25ToFlat                 },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBankedDown25ToRightBankedFlat      },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::right,            TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightBankedQuarterTurn5TileDown25       },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightBankedQuarterTurn3TileDown25       },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBankedQuarterTurn3Tiles             },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftBankToLeftQuarterTurn3TilesUp25     },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::left,             TrackPitch::none,     TrackRoll::left,    TrackElemType::bankedLeftQuarterTurn5Tiles             },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::left,    TrackElemType::leftEighthBankToDiag                    },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftBankedFlatToLeftBankedDown25        },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftBankToDown25                        },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBank                                },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftBankToFlat                          },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftBankedFlatToLeftBankedUp25          },
            {   false,  TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftBankToUp25                          },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::none,     TrackRoll::none,    TrackElemType::leftQuarterTurn3Tiles                   },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::left,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftQuarterTurn5Tiles                   },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::none,     TrackRoll::none,    TrackElemType::leftEighthToDiag                        },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down60,   TrackRoll::none,    TrackElemType::flatToDown60                            },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::flatToLeftBankedDown25                  },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::flatToDown25                            },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::flatToRightBankedDown25                 },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::flatToLeftBank                          },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::flat                                    },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::leftVerySmall,    TrackPitch::none,     TrackRoll::none,    TrackElemType::leftQuarterTurn1Tile                    },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::rightVerySmall,   TrackPitch::none,     TrackRoll::none,    TrackElemType::rightQuarterTurn1Tile                   },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::flatToRightBank                         },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::flatToLeftBankedUp25                    },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::flatToUp25                              },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::flatToRightBankedUp25                   },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::flatToUp60                              },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::none,    TrackElemType::rightEighthToDiag                       },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::right,            TrackPitch::none,     TrackRoll::none,    TrackElemType::rightQuarterTurn5Tiles                  },
            {   false,  TrackPitch::none,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::none,     TrackRoll::none,    TrackElemType::rightQuarterTurn3Tiles                  },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightBankToDown25                       },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightBankedFlatToRightBankedDown25      },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::rightBankToFlat                         },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBank                               },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightBankToUp25                         },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightBankedFlatToRightBankedUp25        },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightEighthBankToDiag                   },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::right,            TrackPitch::none,     TrackRoll::right,   TrackElemType::bankedRightQuarterTurn5Tiles            },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBankedQuarterTurn3Tiles            },
            {   false,  TrackPitch::none,       TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightBankToRightQuarterTurn3TilesUp25   },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::leftSmall,        TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftBankedQuarterTurn3TileUp25          },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::left,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftBankedQuarterTurn5TileUp25          },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::leftBankedUp25ToLeftBankedFlat          },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::leftBankedUp25ToFlat                    },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::up25LeftBanked                          },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftBankedUp25ToUp25                    },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftQuarterTurn3TilesUp25               },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::left,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftQuarterTurn5TilesUp25               },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::up25ToLeftBank                          },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::up25ToFlat                              },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::up25ToRightBank                         },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::up25ToLeftBankedUp25                    },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::up25                                    },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::up25ToRightBankedUp25                   },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::up25ToUp60                              },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::right,            TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightQuarterTurn5TilesUp25              },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightQuarterTurn3TilesUp25              },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::rightBankedUp25ToFlat                   },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::rightBankedUp25ToRightBankedFlat        },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightBankedUp25ToUp25                   },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::up25RightBanked                         },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::right,            TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightBankedQuarterTurn5TileUp25         },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::rightSmall,       TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightBankedQuarterTurn3TileUp25         },
            {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::up60,     TrackRoll::none,    TrackElemType::leftQuarterTurn1TileUp60                },
            {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::up60ToFlat                              },
            {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::up60ToUp25                              },
            {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::up60                                    },
            {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up90,     TrackRoll::none,    TrackElemType::up60ToUp90                              },
            {   false,  TrackPitch::up60,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::up60,     TrackRoll::none,    TrackElemType::rightQuarterTurn1TileUp60               },
            {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::leftSmall,        TrackPitch::up90,     TrackRoll::none,    TrackElemType::leftQuarterTurn1TileUp90                },
            {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up60,     TrackRoll::none,    TrackElemType::up90ToUp60                              },
            {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up90,     TrackRoll::none,    TrackElemType::up90                                    },
            {   false,  TrackPitch::up90,       TrackRoll::none,    TrackCurve::rightSmall,       TrackPitch::up90,     TrackRoll::none,    TrackElemType::rightQuarterTurn1TileUp90               },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftEighthToDiagUp25                    },
            {   false,  TrackPitch::up25,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightEighthToDiagUp25                   },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftEighthToDiagDown25                  },
            {   false,  TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightEighthToDiagDown25                 },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::none,    TrackElemType::leftEighthToOrthogonalUp25              },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::none,    TrackElemType::rightEighthToOrthogonalUp25             },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::none,    TrackElemType::leftEighthToOrthogonalDown25            },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::none,    TrackElemType::rightEighthToOrthogonalDown25           },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagUp25ToLeftBankedUp25                },
            {   true,   TrackPitch::up25,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagUp25ToRightBankedUp25               },
            {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagLeftBankedUp25ToUp25                },
            {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::none,    TrackElemType::diagRightBankedUp25ToUp25               },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagDown25ToLeftBankedDown25            },
            {   true,   TrackPitch::down25,     TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagDown25ToRightBankedDown25           },
            {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagLeftBankedDown25ToDown25            },
            {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::none,    TrackElemType::diagRightBankedDown25ToDown25           },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagLeftBankedFlatToLeftBankedUp25      },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagRightBankedFlatToRightBankedUp25    },
            {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagLeftBankedUp25ToLeftBankedFlat      },
            {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagRightBankedUp25ToRightBankedFlat    },
            {   true,   TrackPitch::none,       TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagLeftBankedFlatToLeftBankedDown25    },
            {   true,   TrackPitch::none,       TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagRightBankedFlatToRightBankedDown25  },
            {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::left,    TrackElemType::diagLeftBankedDown25ToLeftBankedFlat    },
            {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::right,   TrackElemType::diagRightBankedDown25ToRightBankedFlat  },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagFlatToLeftBankedUp25                },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagFlatToRightBankedUp25               },
            {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagLeftBankedUp25ToFlat                },
            {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagRightBankedUp25ToFlat               },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagFlatToLeftBankedDown25              },
            {   true,   TrackPitch::none,       TrackRoll::none,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagFlatToRightBankedDown25             },
            {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagLeftBankedDown25ToFlat              },
            {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::none,     TrackRoll::none,    TrackElemType::diagRightBankedDown25ToFlat             },
            {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::none,             TrackPitch::up25,     TrackRoll::left,    TrackElemType::diagUp25LeftBanked                      },
            {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::none,             TrackPitch::up25,     TrackRoll::right,   TrackElemType::diagUp25RightBanked                     },
            {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::none,             TrackPitch::down25,   TrackRoll::left,    TrackElemType::diagDown25LeftBanked                    },
            {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::none,             TrackPitch::down25,   TrackRoll::right,   TrackElemType::diagDown25RightBanked                   },
            {   false,  TrackPitch::up25,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftEighthBankToDiagUp25                },
            {   false,  TrackPitch::up25,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightEighthBankToDiagUp25               },
            {   false,  TrackPitch::down25,     TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftEighthBankToDiagDown25              },
            {   false,  TrackPitch::down25,     TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightEighthBankToDiagDown25             },
            {   true,   TrackPitch::up25,       TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::up25,     TrackRoll::left,    TrackElemType::leftEighthBankToOrthogonalUp25          },
            {   true,   TrackPitch::up25,       TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::up25,     TrackRoll::right,   TrackElemType::rightEighthBankToOrthogonalUp25         },
            {   true,   TrackPitch::down25,     TrackRoll::left,    TrackCurve::leftLarge,        TrackPitch::down25,   TrackRoll::left,    TrackElemType::leftEighthBankToOrthogonalDown25        },
            {   true,   TrackPitch::down25,     TrackRoll::right,   TrackCurve::rightLarge,       TrackPitch::down25,   TrackRoll::right,   TrackElemType::rightEighthBankToOrthogonalDown25       },
        };
        // clang-format on

        /**
         * Order of special track elements dropdown. Elements with the same name string must be sequential or
         * they show up twice. Includes kSpecialTrackPieceSeparator entries the UI uses for grouping.
         */
        constexpr std::array kSpecialBuildPiecesData = {
            TrackElemType::endStation,

            // Brakes
            TrackElemType::brakes,
            TrackElemType::diagBrakes,
            TrackElemType::down25Brakes,
            TrackElemType::diagDown25Brakes,
            TrackElemType::blockBrakes,
            TrackElemType::diagBlockBrakes,

            // Boosters
            TrackElemType::booster,
            TrackElemType::diagBooster,

            // Photo sections
            TrackElemType::onRidePhoto,

            // Rotation control
            TrackElemType::rotationControlToggle,

            // (Curved) lift (hills) pieces
            TrackElemType::leftCurvedLiftHill,
            TrackElemType::rightCurvedLiftHill,
            TrackElemType::cableLiftHill,
            TrackElemType::poweredLift,
            kSpecialTrackPieceSeparator,

            // Heart Line pieces
            TrackElemType::heartLineTransferUp,
            TrackElemType::heartLineTransferDown,
            TrackElemType::leftHeartLineRoll,
            TrackElemType::rightHeartLineRoll,
            kSpecialTrackPieceSeparator,

            // Brake for drop
            TrackElemType::brakeForDrop,
            kSpecialTrackPieceSeparator,

            // Tower
            TrackElemType::towerBase,
            TrackElemType::towerSection,
            kSpecialTrackPieceSeparator,

            // Mini Golf pieces
            TrackElemType::minigolfHoleA,
            TrackElemType::minigolfHoleB,
            TrackElemType::minigolfHoleC,
            TrackElemType::minigolfHoleD,
            TrackElemType::minigolfHoleE,
            kSpecialTrackPieceSeparator,

            // S-Bends
            TrackElemType::sBendLeft,
            TrackElemType::sBendRight,
            kSpecialTrackPieceSeparator,

            // Helixes
            TrackElemType::leftHalfBankedHelixUpSmall,
            TrackElemType::rightHalfBankedHelixUpSmall,
            TrackElemType::leftHalfBankedHelixDownSmall,
            TrackElemType::rightHalfBankedHelixDownSmall,
            TrackElemType::leftHalfBankedHelixUpLarge,
            TrackElemType::rightHalfBankedHelixUpLarge,
            TrackElemType::leftHalfBankedHelixDownLarge,
            TrackElemType::rightHalfBankedHelixDownLarge,
            TrackElemType::leftQuarterBankedHelixLargeUp,
            TrackElemType::rightQuarterBankedHelixLargeUp,
            TrackElemType::leftQuarterBankedHelixLargeDown,
            TrackElemType::rightQuarterBankedHelixLargeDown,
            TrackElemType::leftQuarterHelixLargeUp,
            TrackElemType::rightQuarterHelixLargeUp,
            TrackElemType::leftQuarterHelixLargeDown,
            TrackElemType::rightQuarterHelixLargeDown,
            kSpecialTrackPieceSeparator,

            // (Wooden) water splash
            TrackElemType::waterSplash,
            kSpecialTrackPieceSeparator,

            // River Rapids
            TrackElemType::waterfall,
            TrackElemType::rapids, // Also used for Monster Trucks
            TrackElemType::whirlpool,
            kSpecialTrackPieceSeparator,

            // Spinning tunnel
            TrackElemType::spinningTunnel,
            kSpecialTrackPieceSeparator,

            // Reverser pieces
            TrackElemType::leftReverser,
            TrackElemType::rightReverser,
            TrackElemType::logFlumeReverser,
            kSpecialTrackPieceSeparator,

            // Reverse freefall pieces
            TrackElemType::reverseFreefallSlope,
            TrackElemType::reverseFreefallVertical,

            // Air thrust pieces
            TrackElemType::airThrustTopCap,
            TrackElemType::airThrustVerticalDown,
            TrackElemType::airThrustVerticalDownToLevel,
            kSpecialTrackPieceSeparator,

            // Corkscrews
            TrackElemType::leftCorkscrewUp,
            TrackElemType::leftCorkscrewDown,
            TrackElemType::rightCorkscrewUp,
            TrackElemType::rightCorkscrewDown,
            TrackElemType::leftFlyerCorkscrewUp,
            TrackElemType::leftFlyerCorkscrewDown,
            TrackElemType::rightFlyerCorkscrewUp,
            TrackElemType::rightFlyerCorkscrewDown,
            TrackElemType::leftLargeCorkscrewUp,
            TrackElemType::leftLargeCorkscrewDown,
            TrackElemType::rightLargeCorkscrewUp,
            TrackElemType::rightLargeCorkscrewDown,
            kSpecialTrackPieceSeparator,

            // Loops
            TrackElemType::leftVerticalLoop,
            TrackElemType::rightVerticalLoop,
            TrackElemType::halfLoopUp,
            TrackElemType::halfLoopDown,
            TrackElemType::flyerHalfLoopUninvertedUp,
            TrackElemType::flyerHalfLoopInvertedDown,
            TrackElemType::flyerHalfLoopInvertedUp,
            TrackElemType::flyerHalfLoopUninvertedDown,
            TrackElemType::leftMediumHalfLoopUp,
            TrackElemType::leftMediumHalfLoopDown,
            TrackElemType::rightMediumHalfLoopUp,
            TrackElemType::rightMediumHalfLoopDown,
            TrackElemType::leftLargeHalfLoopUp,
            TrackElemType::leftLargeHalfLoopDown,
            TrackElemType::rightLargeHalfLoopUp,
            TrackElemType::rightLargeHalfLoopDown,
            TrackElemType::leftFlyerLargeHalfLoopUninvertedUp,
            TrackElemType::leftFlyerLargeHalfLoopInvertedDown,
            TrackElemType::leftFlyerLargeHalfLoopInvertedUp,
            TrackElemType::leftFlyerLargeHalfLoopUninvertedDown,
            TrackElemType::rightFlyerLargeHalfLoopUninvertedUp,
            TrackElemType::rightFlyerLargeHalfLoopInvertedDown,
            TrackElemType::rightFlyerLargeHalfLoopInvertedUp,
            TrackElemType::rightFlyerLargeHalfLoopUninvertedDown,
            TrackElemType::multiDimInvertedFlatToDown90QuarterLoop,
            TrackElemType::up90ToInvertedFlatQuarterLoop,
            TrackElemType::invertedFlatToDown90QuarterLoop,
            TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop,
            TrackElemType::multiDimFlatToDown90QuarterLoop,
            TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop,
            kSpecialTrackPieceSeparator,

            // Zero-G Rolls, Dive Loops
            TrackElemType::leftZeroGRollUp,
            TrackElemType::leftZeroGRollDown,
            TrackElemType::rightZeroGRollUp,
            TrackElemType::rightZeroGRollDown,
            TrackElemType::leftLargeZeroGRollUp,
            TrackElemType::leftLargeZeroGRollDown,
            TrackElemType::rightLargeZeroGRollUp,
            TrackElemType::rightLargeZeroGRollDown,
            TrackElemType::leftEighthDiveLoopUpToOrthogonal,
            TrackElemType::leftEighthDiveLoopDownToDiag,
            TrackElemType::rightEighthDiveLoopUpToOrthogonal,
            TrackElemType::rightEighthDiveLoopDownToDiag,
            kSpecialTrackPieceSeparator,

            // Barrel Rolls
            TrackElemType::leftBarrelRollUpToDown,
            TrackElemType::leftBarrelRollDownToUp,
            TrackElemType::rightBarrelRollUpToDown,
            TrackElemType::rightBarrelRollDownToUp,
            kSpecialTrackPieceSeparator,

            // Twists
            TrackElemType::leftTwistDownToUp,
            TrackElemType::leftTwistUpToDown,
            TrackElemType::rightTwistDownToUp,
            TrackElemType::rightTwistUpToDown,
            TrackElemType::leftFlyerTwistUp,
            TrackElemType::leftFlyerTwistDown,
            TrackElemType::rightFlyerTwistUp,
            TrackElemType::rightFlyerTwistDown,
        };

        // Update the magic number with the current number of track elements to silence.
        static_assert(EnumValue(TrackElemType::count) == 350, "Reminder to add new track element to special dropdown list");

        // ===================================================================================
        // GetNextValidTrackSegments — pipeline support code.
        //
        // The construction code is a four-stage pipeline (see comment at the top of
        // GetNextValidTrackSegments below for the full description). This anonymous-namespace
        // section holds the helpers; the public function ties them together.
        // ===================================================================================

        struct TrackPieceVariant
        {
            bool isInverted{};
            bool isCovered{};
        };

        // Construction variants the ride can be in: regular, plus inverted (if hasInvertedVariant)
        // and covered (if the relevant drawer descriptor actually populates Covered groups —
        // checking RtdFlag::hasCoveredPieces alone would miss rides whose flag is set but only
        // populate Covered for one of the inverted-vs-non-inverted sides).
        sfl::static_vector<TrackPieceVariant, 4> applicableVariants(const ::RideTypeDescriptor& rtd)
        {
            sfl::static_vector<TrackPieceVariant, 4> out;
            out.push_back({ false, false });

            const bool hasInverted = rtd.flags.has(::RtdFlag::hasInvertedVariant);
            if (hasInverted)
                out.push_back({ true, false });

            if (rtd.TrackPaintFunctions.HasCoveredPieces())
                out.push_back({ false, true });
            if (hasInverted && rtd.InvertedTrackPaintFunctions.HasCoveredPieces())
                out.push_back({ true, true });

            return out;
        }

        const ::TrackDrawerEntry& regularDrawerEntryFor(const ::RideTypeDescriptor& rtd, TrackPieceVariant v)
        {
            return v.isInverted ? rtd.InvertedTrackPaintFunctions.Regular : rtd.TrackPaintFunctions.Regular;
        }
        const ::TrackDrawerEntry& coveredDrawerEntryFor(const ::RideTypeDescriptor& rtd, TrackPieceVariant v)
        {
            return v.isInverted ? rtd.InvertedTrackPaintFunctions.Covered : rtd.TrackPaintFunctions.Covered;
        }

        // Inverted-variant rides reverse none<->upsideDown (see TrackGetActualBank2 in
        // src/openrct2/ride/Track.cpp). Kept local to avoid pulling ride_type_t into the API.
        TrackRoll flipRollForVariant(const ::RideTypeDescriptor& rtd, TrackPieceVariant v, TrackRoll roll)
        {
            if (!v.isInverted || !rtd.flags.has(::RtdFlag::hasInvertedVariant))
                return roll;
            if (roll == TrackRoll::none)
                return TrackRoll::upsideDown;
            if (roll == TrackRoll::upsideDown)
                return TrackRoll::none;
            return roll;
        }

        // Stage 2: mirrors UpdateTrackPieceWidgets (src/openrct2-ui/windows/RideConstruction.cpp).
        //
        // Two layered rule sets in the UI: widget visibility (~line 1761 onward, simple
        // IsTrackEnabled checks per button) and disabled state (~line 350 onward, contextual rules
        // combining previous/current pitch and roll). Both are encoded here.
        //
        // The bank gate is hybrid: sloped banked CURVES are gated on slopeCurveBanked /
        // slopeCurveLargeBanked explicitly, because their TEDs are inconsistently grouped (e.g.
        // leftBankToLeftQuarterTurn3TilesUp25 is in TrackGroup::flat). For non-curve banked pieces
        // the destination group annotation IS reliable (flatRollBanking / slopeRollBanking).
        //
        // Default for un-encoded transient clauses: allow. Misses surface as over-emissions, never
        // under-emissions (the kNextSelectedPiece table itself certifies geometric validity).
        bool isTransitionAllowed(const ::TrackDrawerEntry& drawerEntry, const TrackTransitionDescriptor& row)
        {
            using TG = TrackGroup;
            auto enabled = [&](TG g) { return drawerEntry.SupportsTrackGroup(g); };

            const auto curve = row.trackCurve;
            const auto pStart = row.slopeStart;
            const auto pEnd = row.slopeEnd;
            const auto rStart = row.rollStart;
            const auto rEnd = row.rollEnd;
            const bool diag = row.startsDiagonally;

            const bool pStartFlat = (pStart == TrackPitch::none);
            const bool pEndFlat = (pEnd == TrackPitch::none);
            const bool pStart25 = (pStart == TrackPitch::up25 || pStart == TrackPitch::down25);
            const bool pEnd25 = (pEnd == TrackPitch::up25 || pEnd == TrackPitch::down25);
            const bool pStart60 = (pStart == TrackPitch::up60 || pStart == TrackPitch::down60);
            const bool pStart90 = (pStart == TrackPitch::up90 || pStart == TrackPitch::down90);
            const bool pEnd90 = (pEnd == TrackPitch::up90 || pEnd == TrackPitch::down90);

            const bool isSloped = !pStartFlat || !pEndFlat;
            const bool isBanked = (rStart != TrackRoll::none) || (rEnd != TrackRoll::none);
            const bool slopeTransition = (pStart != pEnd);
            const bool bankTransition = (rStart != rEnd);

            // Straight is a basic prerequisite for any construction.
            if (!enabled(TG::straight))
                return false;

            // === Curve gate ===========================================================
            if (curve != TrackCurve::none)
            {
                const bool isLarge = (curve == TrackCurve::leftLarge || curve == TrackCurve::rightLarge);
                const bool isSmall = (curve == TrackCurve::leftSmall || curve == TrackCurve::rightSmall);
                const bool isVerySmall = (curve == TrackCurve::leftVerySmall || curve == TrackCurve::rightVerySmall);
                const bool isMid = (curve == TrackCurve::left || curve == TrackCurve::right);

                if (isVerySmall && !enabled(TG::curveVerySmall))
                    return false;
                if (isMid && !enabled(TG::curve))
                    return false;
                if (isLarge && !enabled(TG::curveLarge))
                    return false;
                if (isSmall)
                {
                    const bool atVertical = pStart90 && pEnd90;
                    if (atVertical)
                    {
                        if (!enabled(TG::curveVertical))
                            return false;
                    }
                    else
                    {
                        if (!enabled(TG::curveSmall))
                            return false;
                    }
                }

                // Disable large curves on slopes unless slopeCurveLarge is enabled AND both
                // pitches are exactly 25°.
                if (isLarge && (isSloped || slopeTransition))
                {
                    if (!enabled(TG::slopeCurveLarge))
                        return false;
                    if (!pStart25 || !pEnd25)
                        return false;
                }

                // Very small curves on slopes need slopeCurve + curveVerySmall.
                if (isVerySmall && isSloped)
                {
                    if (!enabled(TG::slopeCurve) || !enabled(TG::curveVerySmall))
                        return false;
                }

                // When slopeCurve is missing, all curves on slopes are disabled UNLESS both
                // pitches are vertical and curveVertical is enabled.
                if (!enabled(TG::slopeCurve) && isSloped)
                {
                    if (enabled(TG::curveVertical))
                    {
                        if (!(pStart90 && pEnd90))
                            return false;
                    }
                    else
                    {
                        return false;
                    }
                }

                // Same-pitch at 60°: only small curves allowed; require slopeCurveSteep.
                if (!slopeTransition && pStart60 && pStart == pEnd)
                {
                    if (!isSmall)
                        return false;
                    if (!enabled(TG::slopeCurveSteep))
                        return false;
                }

                // Same-pitch at 90°: only small curves allowed; require curveVertical.
                if (!slopeTransition && pStart90 && pStart == pEnd)
                {
                    if (!isSmall)
                        return false;
                    if (!enabled(TG::curveVertical))
                        return false;
                }
            }

            // === Slope gate ===========================================================
            if (slopeTransition)
            {
                if (pStart90 || pEnd90)
                {
                    if (!enabled(TG::slopeVertical))
                        return false;
                }
                // Flat ↔ 60. UI gates this transition strictly on flatToSteepSlope or the
                // relevant long-base group (UpdateTrackPieceWidgets, diagonal flat case ~560,
                // orthogonal flat case ~605). slopeSteepUp/Down is for 25↔60, NOT flat↔60.
                else if (
                    (pStartFlat && pEnd == TrackPitch::up60) || (pStart == TrackPitch::up60 && pEndFlat)
                    || (pStartFlat && pEnd == TrackPitch::down60) || (pStart == TrackPitch::down60 && pEndFlat))
                {
                    const bool ok = diag ? (enabled(TG::flatToSteepSlope) || enabled(TG::diagSlopeSteepLong))
                                         : (enabled(TG::flatToSteepSlope) || enabled(TG::slopeSteepLong));
                    if (!ok)
                        return false;
                }
                // 25 ↔ 60. UI gates this by direction: up25↔up60 needs slopeSteepUp;
                // down25↔down60 needs slopeSteepDown (UpdateTrackPieceWidgets cases up25 line
                // ~629, down25 line ~615; diagonal equivalents in the diag branch). Splitting
                // prevents Log-Flume-class false positives where a ride has only one direction.
                else if (
                    (pStart == TrackPitch::up25 && pEnd == TrackPitch::up60)
                    || (pStart == TrackPitch::up60 && pEnd == TrackPitch::up25))
                {
                    const bool ok = diag ? enabled(TG::diagSlopeSteepUp) : enabled(TG::slopeSteepUp);
                    if (!ok)
                        return false;
                }
                else if (
                    (pStart == TrackPitch::down25 && pEnd == TrackPitch::down60)
                    || (pStart == TrackPitch::down60 && pEnd == TrackPitch::down25))
                {
                    const bool ok = diag ? enabled(TG::diagSlopeSteepDown) : enabled(TG::slopeSteepDown);
                    if (!ok)
                        return false;
                }
                // Flat ↔ 25.
                else if ((pStartFlat && pEnd25) || (pStart25 && pEndFlat))
                {
                    const bool ok = diag ? enabled(TG::diagSlope) : enabled(TG::slope);
                    if (!ok)
                        return false;
                }
                // Other slope transitions (up25↔down25 etc.): trust the table.
            }
            else if (isSloped)
            {
                // Same-pitch continuation at non-flat.
                if (pStart90)
                {
                    if (!enabled(TG::slopeVertical))
                        return false;
                }
                else if (pStart25)
                {
                    const bool ok = diag ? enabled(TG::diagSlope) : enabled(TG::slope);
                    if (!ok)
                        return false;
                }
                else if (pStart == TrackPitch::up60)
                {
                    const bool ok = diag ? enabled(TG::diagSlopeSteepUp) : enabled(TG::slopeSteepUp);
                    if (!ok)
                        return false;
                }
                else if (pStart == TrackPitch::down60)
                {
                    const bool ok = diag ? enabled(TG::diagSlopeSteepDown) : enabled(TG::slopeSteepDown);
                    if (!ok)
                        return false;
                }
            }

            // === Bank gate ============================================================
            // Hybrid: sloped banked CURVES use the slope-curve-banked groups explicitly (their
            // TEDs are inconsistently grouped); flat-banked and sloped-non-curve-banked pieces
            // trust the destination group.
            if (bankTransition || isBanked)
            {
                bool bankOk;
                if (curve != TrackCurve::none && isSloped)
                {
                    const bool isLargeCurve = (curve == TrackCurve::leftLarge || curve == TrackCurve::rightLarge);
                    bankOk = isLargeCurve ? enabled(TG::slopeCurveLargeBanked) : enabled(TG::slopeCurveBanked);
                }
                else
                {
                    const auto& destTed = GetTrackElementDescriptor(row.trackElement);
                    bankOk = enabled(destTed.definition.group);
                }
                if (!bankOk)
                    return false;

                // 60° → flat with bank transition disables banking (transient rule).
                if (pStart60 && pEndFlat && bankTransition)
                    return false;
            }

            // Diagonal + slope + bank simultaneous restriction without slopeCurveLarge.
            if (diag && !enabled(TG::slopeCurveLarge))
            {
                if (!pEndFlat && bankTransition)
                    return false;
                if (rEnd != TrackRoll::none && slopeTransition)
                    return false;
            }

            return true;
        }

        // Stage 3: long-base rewrites — mirrors UpdateTrackPieceWidgets at lines ~4956–5004.
        TrackElemType applyLongBaseRewrites(const ::TrackDrawerEntry& regularDrawer, TrackElemType t)
        {
            if (regularDrawer.SupportsTrackGroup(TrackGroup::slopeSteepLong))
            {
                switch (t)
                {
                    case TrackElemType::flatToUp60:
                        return TrackElemType::flatToUp60LongBase;
                    case TrackElemType::up60ToFlat:
                        return TrackElemType::up60ToFlatLongBase;
                    case TrackElemType::flatToDown60:
                        return TrackElemType::flatToDown60LongBase;
                    case TrackElemType::down60ToFlat:
                        return TrackElemType::down60ToFlatLongBase;
                    default:
                        break;
                }
            }
            if (regularDrawer.SupportsTrackGroup(TrackGroup::diagSlopeSteepLong))
            {
                switch (t)
                {
                    case TrackElemType::diagFlatToUp60:
                        return TrackElemType::diagFlatToUp60LongBase;
                    case TrackElemType::diagUp60ToFlat:
                        return TrackElemType::diagUp60ToFlatLongBase;
                    case TrackElemType::diagFlatToDown60:
                        return TrackElemType::diagFlatToDown60LongBase;
                    case TrackElemType::diagDown60ToFlat:
                        return TrackElemType::diagDown60ToFlatLongBase;
                    default:
                        break;
                }
            }
            return t;
        }

        // Stage 4: covered substitution uses RAW bitset access (matching
        // RideConstruction.cpp:5008), NOT SupportsTrackGroup, so the cheat doesn't broaden it.
        bool coveredGroupEnabledRaw(const ::TrackDrawerEntry& coveredEntry, TrackGroup group)
        {
            return coveredEntry.enabledTrackGroups.get(EnumValue(group));
        }
    } // anonymous namespace

    TrackElemType GetTrackTypeFromCurve(
        TrackCurve curve, bool startsDiagonal, TrackPitch startSlope, TrackPitch endSlope, TrackRoll startBank,
        TrackRoll endBank)
    {
        for (const auto& trackDescriptor : kNextSelectedPiece)
        {
            if (trackDescriptor.trackCurve != curve)
                continue;
            if (trackDescriptor.startsDiagonally != startsDiagonal)
                continue;
            if (trackDescriptor.slopeStart != startSlope)
                continue;
            if (trackDescriptor.slopeEnd != endSlope)
                continue;
            if (trackDescriptor.rollStart != startBank)
                continue;
            if (trackDescriptor.rollEnd != endBank)
                continue;

            return trackDescriptor.trackElement;
        }

        return TrackElemType::none;
    }

    std::span<const TrackElemType> GetSpecialBuildPieces()
    {
        return kSpecialBuildPiecesData;
    }

    // Pipeline:
    //   1. Row select  — kNextSelectedPiece (this file).
    //   2. Static gates — isTransitionAllowed (this file), against the regular drawer.
    //   3. Rewrites    — applyLongBaseRewrites (this file), against the regular drawer.
    //   4. Covered     — substitute alternativeType iff base group is in raw Covered bitset.
    // Iterated across all applicable construction variants and deduplicated.
    std::vector<TrackElemType> GetNextValidTrackSegments(const ::RideTypeDescriptor& rtd, TrackElemType sourceTrackType)
    {
        const auto& srcTed = GetTrackElementDescriptor(sourceTrackType);
        const auto endPitch = srcTed.definition.pitchEnd;
        const auto endRoll = srcTed.definition.rollEnd;
        const bool endIsDiag = TrackPieceDirectionIsDiagonal(srcTed.coordinates.rotationEnd);

        std::vector<TrackElemType> result;
        BitSet<EnumValue(TrackElemType::count)> seen;
        auto emit = [&](TrackElemType t) {
            if (t == TrackElemType::none)
                return;
            const auto idx = EnumValue(t);
            if (seen.get(idx))
                return;
            seen.set(idx, true);
            result.push_back(t);
        };

        const auto variants = applicableVariants(rtd);

        // Pass A: kNextSelectedPiece, four-stage pipeline, across variants.
        for (const auto& row : kNextSelectedPiece)
        {
            if (row.startsDiagonally != endIsDiag)
                continue;
            if (row.slopeStart != endPitch)
                continue;

            for (auto variant : variants)
            {
                const auto& regularEntry = regularDrawerEntryFor(rtd, variant);
                const auto effectiveRoll = flipRollForVariant(rtd, variant, endRoll);
                if (row.rollStart != effectiveRoll)
                    continue;

                if (!isTransitionAllowed(regularEntry, row))
                    continue;

                const TrackElemType rewritten = applyLongBaseRewrites(regularEntry, row.trackElement);

                if (variant.isCovered)
                {
                    const auto& baseTed = GetTrackElementDescriptor(rewritten);
                    if (baseTed.alternativeType == TrackElemType::none)
                        continue;
                    const auto& coveredEntry = coveredDrawerEntryFor(rtd, variant);
                    if (!coveredGroupEnabledRaw(coveredEntry, baseTed.definition.group))
                        continue;
                    emit(baseTed.alternativeType);
                }
                else
                {
                    emit(rewritten);
                }
            }
        }

        // Pass B: curated special pieces, across variants.
        for (TrackElemType type : GetSpecialBuildPieces())
        {
            if (type == kSpecialTrackPieceSeparator)
                continue;
            const auto& destTed = GetTrackElementDescriptor(type);

            if (destTed.definition.pitchStart != endPitch)
                continue;
            if (TrackPieceDirectionIsDiagonal(destTed.coordinates.rotationBegin) != endIsDiag)
                continue;

            for (auto variant : variants)
            {
                const auto& regularEntry = regularDrawerEntryFor(rtd, variant);
                const auto effectiveRoll = flipRollForVariant(rtd, variant, endRoll);
                if (destTed.definition.rollStart != effectiveRoll)
                    continue;

                // Special pieces: destination group IS the gate (matches BuildSpecialElementsList).
                if (!regularEntry.SupportsTrackGroup(destTed.definition.group))
                    continue;

                // Per-piece overrides (mirror Construction.cpp's BuildSpecialElementsList).
                if (type == TrackElemType::towerBase && !rtd.flags.has(::RtdFlag::allowExtraTowerBases))
                    continue;

                if (variant.isCovered)
                {
                    if (destTed.alternativeType == TrackElemType::none)
                        continue;
                    const auto& coveredEntry = coveredDrawerEntryFor(rtd, variant);
                    if (!coveredGroupEnabledRaw(coveredEntry, destTed.definition.group))
                        continue;
                    emit(destTed.alternativeType);
                }
                else
                {
                    emit(type);
                }
            }
        }

        return result;
    }
} // namespace OpenRCT2
