/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackData.h"

#include "../core/EnumUtils.hpp"
#include "../localisation/StringIds.h"
#include "Track.h"
#include "TrackPaint.h"
#include "ted/TrackElementDescriptor.h"

#include <cstdint>

using namespace OpenRCT2;

namespace OpenRCT2::TrackMetadata
{
    // clang-format off
    const TrackDescriptor gTrackDescriptors[186] = {
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

#pragma region GForces

    static int32_t EvaluatorVerticalLoop(const int16_t progress)
    {
        return (abs(progress - 155) / 2) + 28;
    }

    static int32_t EvaluatorHalfLoopUp(const int16_t progress)
    {
        return ((static_cast<uint16_t>(-(progress - 155))) / 2) + 28;
    }

    static int32_t EvaluatorHalfLoopDown(const int16_t progress)
    {
        return (progress / 2) + 28;
    }

    static int32_t EvaluatorLargeZeroGRollUp(const int16_t progress)
    {
        if (progress > 114)
            return (371 - 2 * progress);
        return 0;
    }

    static int32_t EvaluatorLargeZeroGRollDown(const int16_t progress)
    {
        if (progress < 38)
            return 67 + 2 * progress;
        return 0;
    }

    static int32_t EvaluatorLargeZeroGRollUpLeft(const int16_t progress)
    {
        return 387 - 2 * progress;
    }

    static int32_t EvaluatorLargeZeroGRollUpRight(const int16_t progress)
    {
        return 2 * progress - 387;
    }

    static int32_t EvaluatorLargeZeroGRollDownLeft(const int16_t progress)
    {
        return 83 + 2 * progress;
    }

    static int32_t EvaluatorLargeZeroGRollDownRight(const int16_t progress)
    {
        return -(83 + 2 * progress);
    }

    static int32_t EvaluatorZeroGRollUpLeft(const int16_t progress)
    {
        return 174 - progress;
    }

    static int32_t EvaluatorZeroGRollUpRight(const int16_t progress)
    {
        return progress - 174;
    }

    static int32_t EvaluatorZeroGRollDownLeft(const int16_t progress)
    {
        return 73 + progress;
    }

    static int32_t EvaluatorZeroGRollDownRight(const int16_t progress)
    {
        return -(73 + progress);
    }

    static int32_t EvaluatorDiveLoopUp(const int16_t progress)
    {
        return 385 - 2 * progress;
    }

    static int32_t EvaluatorDiveLoopDown(const int16_t progress)
    {
        return 67 + 2 * progress;
    }

    static int32_t EvaluatorDiveLoopUpLeft(const int16_t progress)
    {
        return 380 - 2 * progress;
    }

    static int32_t EvaluatorDiveLoopUpRight(const int16_t progress)
    {
        return 2 * progress - 380;
    }

    static int32_t EvaluatorDiveLoopDownLeft(const int16_t progress)
    {
        return -(62 + 2 * progress);
    }

    static int32_t EvaluatorDiveLoopDownRight(const int16_t progress)
    {
        return 62 + 2 * progress;
    }

    static int32_t EvaluatorWaterSplash(const int16_t progress)
    {
        if (progress < 32)
            return -150;
        if (progress < 64)
            return 150;
        if (progress < 96)
            return 0;
        if (progress < 128)
            return 150;
        return -150;
    }

    static int32_t EvaluatorQuarterTurn3Tiles(const int16_t progress)
    {
        return -(progress / 2) + 134;
    }

    static int32_t EvaluatorMediumHalfLoopUp(const int16_t progress)
    {
        return (244 - progress) / 4 + 51;
    }

    static int32_t EvaluatorMediumHalfLoopDown(const int16_t progress)
    {
        return (progress / 4) + 51;
    }

    static int32_t EvaluatorLargeHalfLoopUp(const int16_t progress)
    {
        return ((static_cast<uint16_t>(-(progress - 311))) / 4) + 46;
    }

    static int32_t EvaluatorLargeHalfLoopDown(const int16_t progress)
    {
        return (progress / 4) + 46;
    }

    static int32_t EvaluatorHeartLineTransferUp(const int16_t progress)
    {
        if (progress < 32)
            return 103;
        if (progress < 64)
            return -103;
        if (progress < 96)
            return 0;
        if (progress < 128)
            return 103;
        return -103;
    }

    static int32_t EvaluatorHeartLineTransferDown(const int16_t progress)
    {
        if (progress < 32)
            return -103;
        if (progress < 64)
            return 103;
        if (progress < 96)
            return 0;
        if (progress < 128)
            return -103;
        return 103;
    }

    static int32_t EvaluatorDown90QuarterLoop(const int16_t progress)
    {
        return (progress / 4) + 55;
    }

    static int32_t EvaluatorUp90QuarterLoop(const int16_t progress)
    {
        return ((static_cast<uint16_t>(-(progress - 137))) / 4) + 55;
    }

    static int32_t EvaluatorSBendLeft(const int16_t progress)
    {
        return (progress < 48) ? 98 : -98;
    }

    static int32_t EvaluatorSBendRight(const int16_t progress)
    {
        return (progress < 48) ? -98 : 98;
    }

#pragma endregion

#pragma region trackBlocks

    using PS = PaintSegment;

    static constexpr SequenceDescriptor kFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kEndStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kBeginStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kMiddleStationSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn5TilesSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedLeftQuarterTurn5TilesSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::null },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kBankedRightQuarterTurn5TilesSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kUp25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kDown25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kSBendLeftSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kSBendLeftSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kSBendLeftSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kSBendRightSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kSBendRightSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kSBendLeftSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(
                PS::top, PS::left, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsAll,                                                                           // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b0110, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq4 = {
        .clearance = { -32, -32, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsNone,          // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsNone,          // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq6 = {
        .clearance = { 0, -32, 120, 16, { 0b1001, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq7 = {
        .clearance = { 32, -32, 32, 96, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq8 = {
        .clearance = { 0, -32, 16, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsAll,                                                                                   // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftVerticalLoopSeq9 = {
        .clearance = { -32, -32, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1001, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq4 = {
        .clearance = { -32, 32, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq5 = {
        .clearance = { 0, 0, 120, 0, { 0b0000, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq6 = {
        .clearance = { 0, 32, 120, 16, { 0b0110, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq7 = {
        .clearance = { 32, 32, 32, 96, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq8 = {
        .clearance = { 0, 32, 16, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq8.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightVerticalLoopSeq9 = {
        .clearance = { -32, 32, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftVerticalLoopSeq9.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TilesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TilesSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsNone,                                                   // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn3TilesUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn3TilesUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistDownToUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistDownToUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kLeftTwistDownToUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightTwistUpToDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftTwistUpToDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kHalfLoopDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft),                            // narrow
            EnumsToFlags(PS::top, PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                                  // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight),                     // narrow
            EnumsToFlags(PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                               // wide
        } },
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCorkscrewUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCorkscrewUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight),                   // narrow
            EnumsToFlags(PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kRightCorkscrewUpSeq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -56, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsRotate(kLeftCorkscrewUpSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlat },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = -2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kTowerBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerBaseSeq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
    };

    static constexpr SequenceDescriptor kTowerSectionSeq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_1 },
    };

    static constexpr SequenceDescriptor kFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatToUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToUp60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25ToUp60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp60ToUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kUp25ToFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatToDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToDown60CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25ToDown60Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown25CoveredSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown60ToDown25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25ToFlatCoveredSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kDown25ToFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn5TilesCoveredSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendLeftSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kSBendLeftSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kSBendLeftSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendLeftCoveredSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendLeftSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendRightSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kSBendRightSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kSBendRightSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kSBendRightCoveredSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kSBendRightSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn3TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn3TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn3TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn3TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn3TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn3TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn3TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesCoveredSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn3TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, -32, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                           // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, -64, 8, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, -96, 8, 4, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, -96, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq2 = {
        .clearance = { -32, 0, 0, 4, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq3 = {
        .clearance = { -32, 32, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq5 = {
        .clearance = { 0, 64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq6 = {
        .clearance = { -32, 96, 8, 4, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpSmallSeq7 = {
        .clearance = { 0, 96, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpSmallSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, -64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, -96, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, -96, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = kLeftHalfBankedHelixUpSmallSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq2 = {
        .clearance = { -32, 0, 8, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq5 = {
        .clearance = { 0, 64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq6 = {
        .clearance = { -32, 96, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownSmallSeq7 = {
        .clearance = { 0, 96, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightHalfBankedHelixUpSmallSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsAll,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsUnimplemented,                                 // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, -64, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsUnimplemented,                                 // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, -32, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, -64, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, -96, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                           // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, -96, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                                    // narrow
            kSegmentsUnimplemented,                           // inverted
            EnumsToFlags(PS::top, PS::topLeft, PS::topRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, -128, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, -128, 8, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            EnumsToFlags(
                PS::top, PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, -128, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            kSegmentsNone,                                    // narrow
            kSegmentsUnimplemented,                           // inverted
            EnumsToFlags(PS::top, PS::topLeft, PS::topRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, -160, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, -160, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq4 = {
        .clearance = { -32, 64, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq5 = {
        .clearance = { -64, 32, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq6 = {
        .clearance = { -64, 64, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq7 = {
        .clearance = { -64, 96, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq7.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq8 = {
        .clearance = { -32, 96, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq8.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq9 = {
        .clearance = { -64, 128, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq9.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq10 = {
        .clearance = { -32, 128, 8, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq10.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq11 = {
        .clearance = { 0, 128, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq11.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq12 = {
        .clearance = { -32, 160, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq12.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixUpLargeSeq13 = {
        .clearance = { 0, 160, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHalfBankedHelixUpLargeSeq13.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, -32, 8, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, -32, 8, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, -64, 8, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, -32, 8, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, -64, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, -96, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, -96, 0, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq8.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, -128, 0, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq9.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, -128, 0, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq10.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, -128, 0, 0, { 0b0100, 0b0000 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq11.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, -160, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq12.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, -160, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -2,
        .blockedSegments = kLeftHalfBankedHelixUpLargeSeq13.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq0 = {
        .clearance = { 0, 0, 8, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq1 = {
        .clearance = { 0, 32, 8, 4, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq2 = {
        .clearance = { -32, 0, 8, 4, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq3 = {
        .clearance = { -32, 32, 8, 0, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq4 = {
        .clearance = { -32, 64, 8, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq5 = {
        .clearance = { -64, 32, 8, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq6 = {
        .clearance = { -64, 64, 8, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq7 = {
        .clearance = { -64, 96, 0, 4, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq7.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq8 = {
        .clearance = { -32, 96, 0, 4, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq8.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq9 = {
        .clearance = { -64, 128, 0, 4, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq9.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq10 = {
        .clearance = { -32, 128, 0, 0, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq10.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq11 = {
        .clearance = { 0, 128, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq11.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq12 = {
        .clearance = { -32, 160, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq12.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightHalfBankedHelixDownLargeSeq13 = {
        .clearance = { 0, 160, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 2,
        .blockedSegments = kRightHalfBankedHelixUpLargeSeq13.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kBoosterSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    // Maze sequence blocks do not have the height marker flag set because they are handled differently
    static constexpr SequenceDescriptor kMazeSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kMazeSeq1 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq2 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq3 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq4 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq5 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq6 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq7 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq8 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq9 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq10 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq11 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq12 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq13 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq14 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kMazeSeq15 = {
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kBankedLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterBankedHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kBankedRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, -64, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, -32, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, -64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq4 = {
        .clearance = { -32, 64, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq5 = {
        .clearance = { -64, 32, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeUpSeq6 = {
        .clearance = { -64, 64, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kLeftQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, -32, 0, 12, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b1101, 0 }, 0 },
        .blockedSegments = kLeftQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, -64, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = kLeftQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, -32, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, -64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = -1,
        .blockedSegments = kLeftQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq0 = {
        .clearance = { 0, 0, 0, 12, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kRightQuarterTurn5TilesSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq1 = {
        .clearance = { 0, 32, 0, 12, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq2 = {
        .clearance = { -32, 0, 0, 12, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1110, 0 }, 0 },
        .blockedSegments = kRightQuarterTurn5TilesSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = kRightQuarterTurn5TilesSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq5 = {
        .clearance = { -64, 32, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightQuarterTurn5TilesSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightQuarterHelixLargeDownSeq6 = {
        .clearance = { -64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre },
        .extraSupportRotation = 1,
        .blockedSegments = kRightQuarterTurn5TilesSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kWaterfallSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRapidsSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kOnRidePhotoSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsAllTypes(kSegmentsAll),
    };

    static constexpr SequenceDescriptor kDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kWaterSplashSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq3 = {
        .clearance = { -96, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kWaterSplashSeq4 = {
        .clearance = { -128, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq2 = {
        .clearance = { -64, 0, 16, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToUp60LongBaseSeq3 = {
        .clearance = { -96, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 64, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 80, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kWhirlpoolSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq1 = {
        .clearance = { -32, 0, 16, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToFlatLongBaseSeq3 = {
        .clearance = { -96, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp60DegLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq3 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq1 = {
        .clearance = { -32, 0, 64, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq2 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq2 = {
        .clearance = { -64, 0, 40, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq1 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatToDown60LongBaseSeq3 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up60DegToFlatLongBaseSeq0 },
        .metalSupports = { MetalSupportPlace::centre, true },
        .extraSupportRotation = 2,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq1 = {
        .clearance = { -32, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq2 = {
        .clearance = { -64, 0, -32, 32, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kCableLiftHillSeq3 = {
        .clearance = { -96, 0, -96, 64, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::centre, true },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq2 = {
        .clearance = { -64, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq3 = {
        .clearance = { -96, 0, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq4 = {
        .clearance = { -128, 0, 0, 160, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq5 = {
        .clearance = { -192, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallSlopeSeq6 = {
        .clearance = { -160, 0, 0, 208, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kReverseFreefallVerticalSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, 0 },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kDown90Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b1100 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp60ToUp90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kDown90ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0b0011 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1110,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kUp90ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1011,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1110,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDown60ToDown90Seq1 = {
        .clearance = { 32, 0, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kBrakeForDropSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::bottomCorner },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .metalSupports = { MetalSupportPlace::centre },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .metalSupports = { MetalSupportPlace::leftCorner },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                            // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagSeq4 = {
        .clearance = { -64, -32, 0, 0, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(
                PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // inverted
            kSegmentsAll,         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq3 = {
        .clearance = { -64, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalSeq4 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalSeq4 = {
        .clearance = { -32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagSeq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kDiagFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToUp60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq1 = {
        .clearance = { 0, 32, 0, 64, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq2 = {
        .clearance = { -32, 0, 0, 64, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60Seq3 = {
        .clearance = { -32, 32, 0, 64, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToFlatSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToDown60Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 32, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 32, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 32, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp25ToUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToDown60Seq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1101, 0 }, 0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 24, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 24, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagDown60ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 24, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToFlatSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankToDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagLeftBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),             // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // inverted
            kSegmentsAll,                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kDiagRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft),          // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsAll,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kLogFlumeReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kSpinningTunnelSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::disallowDoors, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                             // inverted
            kSegmentsAll,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsAll,                                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                              // inverted
            kSegmentsAll,                                                                        // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollUpToDownSeq2 = {
        .clearance = { -64, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollUpToDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftBarrelRollUpToDownSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBarrelRollDownToUpSeq2 = {
        .clearance = { -64, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBarrelRollDownToUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsUnimplemented,                                          // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankToLeftQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsAll,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankToRightQuarterTurn3TilesUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankToLeftQuarterTurn3TilesUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq1 = {
        .clearance = { 0, -32, 0, 16, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 = {
        .clearance = { -32, -32, 0, 0, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsRotate(kRightBankToRightQuarterTurn3TilesUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn3TilesDown25ToRightBankSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn3TilesDown25ToLeftBankSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kPoweredLiftSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeHalfLoopUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kRightLargeHalfLoopUpSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = kRightLargeHalfLoopUpSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightLargeHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightLargeHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kRightLargeHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kRightLargeHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftLargeHalfLoopDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightLargeHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kLeftLargeHalfLoopUpSeq6.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kLeftLargeHalfLoopUpSeq5.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kLeftLargeHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftLargeHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = kLeftLargeHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kLeftLargeHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightLargeHalfLoopDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftLargeHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq1 = {
        .clearance = { -32, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistUpSeq2 = {
        .clearance = { -64, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftFlyerTwistUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq1 = {
        .clearance = { -32, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerTwistDownSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerTwistDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::bottomLeft, PS::topLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::bottomLeft, PS::topLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -88, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq2 = {
        .clearance = { 0, 0, -104, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedDownSeq3 = {
        .clearance = { 32, 0, -120, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kFlyerHalfLoopUninvertedUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftCorkscrewUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kLeftCorkscrewUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, -32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kLeftCorkscrewUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightCorkscrewUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightCorkscrewUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewUpSeq2 = {
        .clearance = { -32, 32, 48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = kRightCorkscrewUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                       // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                             // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                              // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                              // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -24, 32, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerCorkscrewDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerCorkscrewDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq1 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq2 = {
        .clearance = { -64, 0, 16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferUpSeq3 = {
        .clearance = { 0, 0, 32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kHeartLineTransferUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq1 = {
        .clearance = { -32, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = kHeartLineTransferUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq2 = {
        .clearance = { -64, 0, -16, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kHeartLineTransferUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kHeartLineTransferDownSeq3 = {
        .clearance = { 0, 0, -32, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = kHeartLineTransferUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq2 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq3 = {
        .clearance = { -96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq4 = {
        .clearance = { -128, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightHeartLineRollSeq5 = {
        .clearance = { -160, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftHeartLineRollSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleASeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleBSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleCSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleDSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq1 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMinigolfHoleESeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -40, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -96, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -96, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kInvertedFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsNone,          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftCurvedLiftHillSeq3 = {
        .clearance = { -32, -32, 0, 8, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightCurvedLiftHillSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftCurvedLiftHillSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq1 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq2 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq3 = {
        .clearance = { -64, -32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightReverserSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq2 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq3 = {
        .clearance = { -64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq4 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightReverserSeq5 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftReverserSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq0 = {
        .clearance = { 0, 0, 0, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq1 = {
        .clearance = { 32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq2 = {
        .clearance = { -64, 0, 0, 32, { 0b1111, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustTopCapSeq3 = {
        .clearance = { -32, 0, 0, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq0 = {
        .clearance = { 0, 0, 0, 48, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownSeq1 = {
        .clearance = { 32, 0, 0, 48, { 0b1111, 0 }, 0 },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq0 = {
        .clearance = { 0, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq1 = {
        .clearance = { 32, 0, 0, 208, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq2 = {
        .clearance = { -32, 0, 0, 160, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq3 = {
        .clearance = { -64, 0, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq4 = {
        .clearance = { -96, 0, 0, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq5 = {
        .clearance = { -128, 0, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kAirThrustVerticalDownToLevelSeq6 = {
        .clearance = { -160, 0, 0, 16, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsAll,           // wide
        } },
    };

    static constexpr SequenceDescriptor kBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsNone,                                                   // narrow
            kSegmentsNone,                                                   // inverted
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, -32, 16, 16, { 0b0111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1011, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b0111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, -32, 0, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn3TileUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq0 = {
        .clearance = { 0, 0, 16, 16, { 0b1011, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq2 = {
        .clearance = { -32, 0, 16, 0, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn3TileDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn3TileDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft),                  // narrow
            EnumsToFlags(PS::top, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, -32, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // inverted
            EnumsToFlags(
                PS::top, PS::right, PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft,
                PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, -64, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,                                          // narrow
            kSegmentsNone,                                          // inverted
            EnumsToFlags(PS::right, PS::topRight, PS::bottomRight), // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, -32, 32, 16, { 0b0111, 0b0100 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsAllTypes(
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight)),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, -64, 48, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::bottomRight),               // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomRight), // inverted
            kSegmentsAll,                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq1 = {
        .clearance = { 0, 32, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq2 = {
        .clearance = { -32, 0, 16, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq4 = {
        .clearance = { -32, 64, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq5 = {
        .clearance = { -64, 32, 32, 16, { 0b1011, 0b1000 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq6 = {
        .clearance = { -64, 64, 48, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileUp25Seq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileUp25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq6.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, -32, 48, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq4.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq5.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, -32, 24, 16, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, -64, 16, 0, { 0b1000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, -32, 16, 16, { 0b0111, 0b0001 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, -64, 0, 16, { 0b1111, 0b1001 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightBankedQuarterTurn5TileUp25Seq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq0 = {
        .clearance = { 0, 0, 48, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq1 = {
        .clearance = { 0, 32, 48, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq2 = {
        .clearance = { -32, 0, 32, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq3 = {
        .clearance = { -32, 32, 24, 16, { 0b1110, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq4 = {
        .clearance = { -32, 64, 16, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq5 = {
        .clearance = { -64, 32, 16, 16, { 0b1011, 0b0010 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq6 = {
        .clearance = { -64, 64, 0, 16, { 0b1111, 0b0110 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedQuarterTurn5TileDown25Seq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightBankedQuarterTurn5TileDown25Seq7 = {
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kUp25ToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kDown25ToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedFlatToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToLeftBankedFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedFlatToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToLeftBankedFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankedUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedUp25ToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0b0011 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25DegToFlat },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kFlatToLeftBankedDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::flatToUp25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftBankedDown25ToFlatSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileUp90Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileUp90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topLeft, PS::bottomRight), // narrow
            EnumsToFlags(PS::centre, PS::topLeft, PS::bottomRight), // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq0 = {
        .clearance = { 0, 0, 0, 72, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1111,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftQuarterTurn1TileDown90Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightQuarterTurn1TileDown90Seq1 = {
        .clearance = { 0, -32, 0, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 0, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 56, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 96, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq1 = {
        .clearance = { -32, 0, -72, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq2 = {
        .clearance = { -64, 0, -128, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimFlatToDown90QuarterLoopSeq3 = {
        .clearance = { -96, 0, -128, 0, { 0b0000, 0 }, RCT_PREVIEW_TRACK_FLAG_0 | RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq0 = {
        .clearance = { 0, 0, 32, 56, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq1 = {
        .clearance = { 32, 0, 88, 32, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 = {
        .clearance = { 64, 0, 128, 16, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_IS_VERTICAL },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRotationControlToggleSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4ASeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq2 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x2Seq3 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq8 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq9 = {
        .clearance = { 64, 32, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq10 = {
        .clearance = { 64, 64, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq11 = {
        .clearance = { 64, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq12 = {
        .clearance = { 96, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq13 = {
        .clearance = { 96, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq14 = {
        .clearance = { 96, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack4x4Seq15 = {
        .clearance = { 96, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq1 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq2 = {
        .clearance = { 0, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq3 = {
        .clearance = { 0, 96, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq4 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq5 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq6 = {
        .clearance = { 32, 64, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack2x4Seq7 = {
        .clearance = { 32, 96, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x5Seq4 = {
        .clearance = { 64, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1ASeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::trackOrigin, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4BSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x1BSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW,
                   SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin, SequenceFlag::connectsToPath,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW, SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq1 = {
        .clearance = { -64, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack1x4CSeq3 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW,
                   SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::trackOrigin },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq1 = {
        .clearance = { -32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq2 = {
        .clearance = { -32, 0, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq3 = {
        .clearance = { -32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNE, SequenceFlag::entranceConnectionSE, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq4 = {
        .clearance = { 0, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionNW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq5 = {
        .clearance = { 0, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq6 = {
        .clearance = { 32, -32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSW, SequenceFlag::entranceConnectionNW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq7 = {
        .clearance = { 32, 32, 0, 0, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::entranceConnectionSE, SequenceFlag::entranceConnectionSW, SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kFlatTrack3x3Seq8 = {
        .clearance = { 32, 0, 0, 0, { 0b1111, 0 }, RCT_PREVIEW_TRACK_FLAG_1 },
        .flags = { SequenceFlag::entranceConnectionSW },
        .woodenSupports = { WoodenSupportSubType::neSw },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                       // inverted
            kSegmentsUnimplemented,                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq3 = {
        .clearance = { -32, -32, 32, 24, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = { {
            kSegmentsNone,          // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq4 = {
        .clearance = { -64, -32, 48, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewUpSeq5 = {
        .clearance = { -64, -64, 72, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq1 = {
        .clearance = { -32, 0, 8, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq2 = {
        .clearance = { -64, 0, 32, 24, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq3 = {
        .clearance = { -32, 32, 32, 24, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq4 = {
        .clearance = { -64, 32, 48, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewUpSeq5 = {
        .clearance = { -64, 64, 72, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq5.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq4.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq2 = {
        .clearance = { -32, -32, -80, 24, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq3.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b0111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq2.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq4 = {
        .clearance = { -64, -32, -104, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq1.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kLeftLargeCorkscrewDownSeq5 = {
        .clearance = { -64, -64, -112, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightLargeCorkscrewUpSeq0.blockedSegments, 1),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq0 = {
        .clearance = { 0, 0, -40, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq1 = {
        .clearance = { -32, 0, -64, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq2 = {
        .clearance = { -32, 32, -80, 24, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq3 = {
        .clearance = { -64, 0, -80, 24, { 0b1011, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq4 = {
        .clearance = { -64, 32, -104, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeCorkscrewDownSeq5 = {
        .clearance = { -64, 64, -112, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeCorkscrewDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq3 = {
        .clearance = { -64, -32, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopUpSeq4 = {
        .clearance = { -32, -32, 168, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq1 = {
        .clearance = { -32, 0, 16, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq2 = {
        .clearance = { -64, 0, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq3 = {
        .clearance = { -64, 32, 56, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopUpSeq4 = {
        .clearance = { -32, 32, 168, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightMediumHalfLoopUpSeq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = kRightMediumHalfLoopUpSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq2 = {
        .clearance = { -32, -32, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = kRightMediumHalfLoopUpSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq3 = {
        .clearance = { 0, -32, -200, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = kRightMediumHalfLoopUpSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftMediumHalfLoopDownSeq4 = {
        .clearance = { 32, -32, -216, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightMediumHalfLoopUpSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq0 = {
        .clearance = { 0, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq1 = {
        .clearance = { -32, 0, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq2 = {
        .clearance = { -32, 32, -160, 120, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq3 = {
        .clearance = { 0, 32, -200, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightMediumHalfLoopDownSeq4 = {
        .clearance = { 32, 32, -216, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftMediumHalfLoopDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 8, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 24, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftZeroGRollUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftZeroGRollUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -56, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftZeroGRollUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -48, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -56, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftZeroGRollDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq1 = {
        .clearance = { -32, 0, 56, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq2 = {
        .clearance = { -64, 0, 96, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollUpSeq3 = {
        .clearance = { -96, 0, 120, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -56, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -96, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftLargeZeroGRollDownSeq3 = {
        .clearance = { -96, 0, -152, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kLeftLargeZeroGRollUpSeq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq0 = {
        .clearance = { 0, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq1 = {
        .clearance = { -32, 0, -56, 40, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq2 = {
        .clearance = { -64, 0, -96, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightLargeZeroGRollDownSeq3 = {
        .clearance = { -96, 0, -152, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftLargeZeroGRollDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                          // narrow
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, -32, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, -32, -216, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, -32, -232, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, -32, -248, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq1 = {
        .clearance = { -32, 0, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq2 = {
        .clearance = { -64, 0, -128, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq3 = {
        .clearance = { -32, 32, -184, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq4 = {
        .clearance = { 0, 32, -216, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq5 = {
        .clearance = { 32, 32, -232, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedDownSeq6 = {
        .clearance = { 64, 32, -248, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedDownSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                 // narrow
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, -32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                // narrow
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, -32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, -32, 248, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                                                         // narrow
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq3 = {
        .clearance = { -96, 0, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq4 = {
        .clearance = { -128, 32, 120, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq5 = {
        .clearance = { -96, 32, 64, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopInvertedUpSeq6 = {
        .clearance = { -64, 32, 248, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopInvertedUpSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, -32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, -32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                         // inverted
            kSegmentsUnimplemented,                                                                         // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, -32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, -32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq2 = {
        .clearance = { -64, 0, -160, 96, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq3 = {
        .clearance = { -32, 32, -216, 192, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq4 = {
        .clearance = { 0, 32, -248, 56, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq5 = {
        .clearance = { 32, 32, -264, 40, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq5.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightFlyerLargeHalfLoopUninvertedDownSeq6 = {
        .clearance = { 64, 32, -280, 24, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftFlyerLargeHalfLoopUninvertedDownSeq6.blockedSegments),
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq1 = {
        .clearance = { -32, 0, 16, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            kSegmentsAll,           // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq2 = {
        .clearance = { -64, 0, 32, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            kSegmentsUnimplemented, // narrow
            EnumsToFlags(
                PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft,
                PS::bottomRight),   // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopInvertedUpSeq3 = {
        .clearance = { -32, 0, 120, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            kSegmentsUnimplemented,                                 // narrow
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq0 = {
        .clearance = { 0, 0, -32, 32, { 0b1111, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq1 = {
        .clearance = { -32, 0, -120, 96, { 0b0011, 0 }, 0 },
        .allowedWallEdges = 0b1011,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::bottom, PS::centre, PS::topLeft, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                       // inverted
            kSegmentsUnimplemented,                                                                       // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq2 = {
        .clearance = { 0, 0, -136, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .blockedSegments = { {
            kSegmentsAll,           // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };

    static constexpr SequenceDescriptor kFlyerHalfLoopUninvertedDownSeq3 = {
        .clearance = { 32, 0, -152, 16, { 0b1111, 0b1100 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsUnimplemented,                                 // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = { {
            EnumsToFlags(PS::centre, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                 // inverted
            kSegmentsAll,                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsAll,                                                                           // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::bottom, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                         // inverted
            kSegmentsAll,                                                                   // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsAll,                                                    // wide
        } },
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, 0 },
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                                           // inverted
            kSegmentsAll,                                                                                     // wide
        } },
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthToDiagDown25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthToDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthToDiagUp25Seq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25ToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToUp25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25ToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToDown25Seq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToLeftBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToRightBankedFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedUp25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedUp25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToLeftBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToRightBankedDown25Seq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagLeftBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq0 = {
        .clearance = { 0, 0, 0, 8, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq1 = {
        .clearance = { 0, 32, 0, 8, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq2 = {
        .clearance = { -32, 0, 0, 8, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagRightBankedDown25ToFlatSeq3 = {
        .clearance = { -32, 32, 0, 8, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagUp25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25LeftBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagDown25RightBankedSeq3 = {
        .clearance = { -32, 32, 0, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = kLeftEighthToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, -32, 32, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, -32, 32, 24, { 0b0001, 0 }, 0 },
        .blockedSegments = kLeftEighthToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq0.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq1 = {
        .clearance = { -32, 0, 16, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq1.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq2 = {
        .clearance = { -32, 32, 32, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq2.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq3 = {
        .clearance = { -64, 0, 32, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq3.blockedSegments),
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagUp25Seq4 = {
        .clearance = { -64, 32, 32, 24, { 0b0010, 0 }, 0 },
        .blockedSegments = blockedSegmentsFlipXAxis(kLeftEighthBankToDiagUp25Seq4.blockedSegments),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthBankToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, -32, -48, 16, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0010, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0011,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, -32, -48, 16, { 0b0001, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthBankToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq0 = {
        .clearance = { 0, 0, -16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthBankToDiagUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq1 = {
        .clearance = { -32, 0, -32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthBankToDiagUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq2 = {
        .clearance = { -32, 32, -48, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kRightEighthBankToDiagUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq3 = {
        .clearance = { -64, 0, -48, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kRightEighthBankToDiagUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToDiagDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b0010, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthBankToDiagUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq4.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq2.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq3.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq1.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -64, 32, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = blockedSegmentsRotate(kRightEighthBankToDiagUp25Seq0.blockedSegments, 2),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq0 = {
        .clearance = { 0, 0, 0, 16, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq4.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq2.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq3.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq3 = {
        .clearance = { -32, 32, 16, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq1.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalUp25Seq4 = {
        .clearance = { -32, 64, 32, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::nwSe },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthBankToDiagUp25Seq0.blockedSegments, 3),
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kLeftEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -64, 32, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kLeftEighthBankToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq0 = {
        .clearance = { 0, 0, -16, 24, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .woodenSupports = { WoodenSupportSubType::neSw, WoodenSupportTransitionType::up25Deg },
        .extraSupportRotation = 2,
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq1 = {
        .clearance = { 0, 32, -16, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0100,
        .woodenSupports = { WoodenSupportSubType::neSw },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq2 = {
        .clearance = { -32, 0, -16, 16, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner1 },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq3 = {
        .clearance = { -32, 32, -32, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .woodenSupports = { WoodenSupportSubType::corner3 },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kRightEighthBankToOrthogonalDown25Seq4 = {
        .clearance = { -32, 64, -48, 16, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kRightEighthBankToOrthogonalUp25Seq4.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = kDiagFlatSeq0.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq1 = {
        .clearance = { 0, 32, 0, 0, { 0b0100, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b0110,
        .woodenSupports = { WoodenSupportSubType::corner0 },
        .blockedSegments = kDiagFlatSeq1.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq2 = {
        .clearance = { -32, 0, 0, 0, { 0b0001, 0 }, RCT_PREVIEW_TRACK_FLAG_0 },
        .allowedWallEdges = 0b1001,
        .woodenSupports = { WoodenSupportSubType::corner2 },
        .blockedSegments = kDiagFlatSeq2.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagBlockBrakesSeq3 = {
        .clearance = { -32, 32, 0, 0, { 0b0010, 0 }, 0 },
        .blockedSegments = kDiagFlatSeq3.blockedSegments,
    };

    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq0 = {
        .clearance = { 0, 0, 0, 0, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq1 = {
        .clearance = { 0, 32, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq2 = {
        .clearance = { -32, 0, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq3 = {
        .clearance = { -32, 32, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq6 = {
        .clearance = { -64, 64, 8, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq7 = {
        .clearance = { -64, 96, 40, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq8 = {
        .clearance = { -96, 64, 40, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToUp60LongBaseSeq9 = {
        .clearance = { -96, 96, 40, 104, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 0, 72, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 0, 72, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 56, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::right, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented, // inverted
            kSegmentsUnimplemented, // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 80, 8, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 80, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagUp60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 80, 8, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = { {
            EnumsToFlags(PS::left, PS::centre, PS::topLeft, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                          // inverted
            kSegmentsUnimplemented,                                          // wide
        } },
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq0 = {
        .clearance = { 0, 0, 80, 8, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq9.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq1 = {
        .clearance = { 0, 32, 80, 8, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq8.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq2 = {
        .clearance = { -32, 0, 80, 8, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq7.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq3 = {
        .clearance = { -32, 32, 56, 8, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq6.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq4 = {
        .clearance = { -32, 64, 56, 32, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq5 = {
        .clearance = { -64, 32, 56, 32, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq6 = {
        .clearance = { -64, 64, 0, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq7 = {
        .clearance = { -64, 96, 0, 72, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq8 = {
        .clearance = { -96, 64, 0, 72, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagFlatToDown60LongBaseSeq9 = {
        .clearance = { -96, 96, 0, 64, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagUp60ToFlatLongBaseSeq0.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq0 = {
        .clearance = { 0, 0, 40, 104, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq9.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq1 = {
        .clearance = { 0, 32, 40, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq8.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq2 = {
        .clearance = { -32, 0, 40, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq7.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq3 = {
        .clearance = { -32, 32, 8, 64, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq6.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq4 = {
        .clearance = { -32, 64, 8, 40, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq5 = {
        .clearance = { -64, 32, 8, 40, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq6 = {
        .clearance = { -64, 64, 0, 32, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq7 = {
        .clearance = { -64, 96, 0, 16, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq8 = {
        .clearance = { -96, 64, 0, 16, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kDiagDown60ToFlatLongBaseSeq9 = {
        .clearance = { -96, 96, 0, 0, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kDiagFlatToUp60LongBaseSeq0.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = { {
            EnumsToFlags(PS::right, PS::centre, PS::topRight, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                             // inverted
            kSegmentsUnimplemented,                                             // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::centre, PS::topLeft, PS::topRight), // narrow
            kSegmentsUnimplemented,                                       // inverted
            kSegmentsUnimplemented,                                       // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = { {
            EnumsToFlags(PS::bottom, PS::centre, PS::bottomLeft, PS::bottomRight), // narrow
            kSegmentsUnimplemented,                                                // inverted
            kSegmentsUnimplemented,                                                // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -64, 32, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -96, 32, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = { {
            EnumsToFlags(PS::top, PS::left, PS::centre, PS::topLeft, PS::topRight, PS::bottomLeft), // narrow
            kSegmentsUnimplemented,                                                                 // inverted
            kSegmentsUnimplemented,                                                                 // wide
        } },
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq0 = {
        .clearance = { 0, 0, 0, 64, { 0b1101, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq0.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq1 = {
        .clearance = { -32, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq1.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq2 = {
        .clearance = { 0, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq2.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq3 = {
        .clearance = { -32, 32, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0001,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq3.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq4 = {
        .clearance = { -32, 64, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq4.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopUpToOrthogonalSeq5 = {
        .clearance = { -32, 96, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0101,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(
            blockedSegmentsFlipXAxis(kLeftEighthDiveLoopUpToOrthogonalSeq5.blockedSegments), 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq5.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq4.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b0010,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq3.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, -32, 0, 88, { 0b1000, 0 }, 0 },
        .allowedWallEdges = 0b1100,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq2.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0011,
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq1.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kLeftEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, -32, 0, 64, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kRightEighthDiveLoopUpToOrthogonalSeq0.blockedSegments, 1),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq0 = {
        .clearance = { 0, 0, 112, 24, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .flags = { SequenceFlag::hasHeightMarker },
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq5.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq1 = {
        .clearance = { -32, 0, 80, 48, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1010,
        .invertSegmentBlocking = true,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq4.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq2 = {
        .clearance = { -64, 0, 32, 80, { 0b1111, 0 }, 0 },
        .allowedWallEdges = 0b1000,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq3.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq3 = {
        .clearance = { -64, 32, 0, 88, { 0b0100, 0 }, 0 },
        .allowedWallEdges = 0b0110,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq2.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq4 = {
        .clearance = { -96, 0, 0, 88, { 0b0001, 0 }, 0 },
        .allowedWallEdges = 0b1001,
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq1.blockedSegments, 2),
    };
    static constexpr SequenceDescriptor kRightEighthDiveLoopDownToDiagSeq5 = {
        .clearance = { -96, 32, 0, 64, { 0b0010, 0 }, 0 },
        .allowedWallEdges = 0b0000,
        .flags = { SequenceFlag::hasHeightMarker },
        .blockedSegments = blockedSegmentsRotate(kLeftEighthDiveLoopUpToOrthogonalSeq0.blockedSegments, 2),
    };

    constexpr auto kTEDFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::flatCovered,
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flat,
        .flags = { TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::straight, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatSeq0 } },
    };

    constexpr auto kTEDEndStation = TrackElementDescriptor{
        .description = STR_STATION_PLATFORM,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::endStation, TrackElemType::endStation },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::endStation,
        .flags = {},
        .definition = { TrackGroup::stationEnd, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kEndStationSeq0 } },
    };

    constexpr auto kTEDBeginStation = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::endStation, TrackElemType::endStation },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::beginStation,
        .flags = {},
        .definition = { TrackGroup::stationEnd, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kBeginStationSeq0 } },
    };

    constexpr auto kTEDMiddleStation = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::endStation, TrackElemType::endStation },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::middleStation,
        .flags = {},
        .definition = { TrackGroup::stationEnd, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kMiddleStationSeq0 } },
    };

    constexpr auto kTEDUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up25Covered,
        .priceModifier = 79872,
        .mirrorElement = TrackElemType::up25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp25Seq0 } },
    };

    constexpr auto kTEDUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 64, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up60Covered,
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::up60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepUp, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp60Seq0 } },
    };

    constexpr auto kTEDFlatToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::flatToUp25Covered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::flatToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up25ToUp60Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::up25ToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepUp, TrackPitch::up60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kUp25ToUp60Seq0 } },
    };

    constexpr auto kTEDUp60ToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up60ToUp25Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::up60ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepUp, TrackPitch::up25, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kUp60ToUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::up25ToFlatCovered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::up25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToFlatSeq0 } },
    };

    constexpr auto kTEDDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down25Covered,
        .priceModifier = 79872,
        .mirrorElement = TrackElemType::down25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kDown25Seq0 } },
    };

    constexpr auto kTEDDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 64, 0, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down60Covered,
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::down60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepDown, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown60Seq0 } },
    };

    constexpr auto kTEDFlatToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::flatToDown25Covered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::flatToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down25ToDown60Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::down25ToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepDown, TrackPitch::down60, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kDown25ToDown60Seq0 } },
    };

    constexpr auto kTEDDown60ToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down60ToDown25Covered,
        .priceModifier = 96256,
        .mirrorElement = TrackElemType::down60ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slopeSteepDown, TrackPitch::down25, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kDown60ToDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::down25ToFlatCovered,
        .priceModifier = 73728,
        .mirrorElement = TrackElemType::down25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::slope, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToFlatSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .alternativeType = TrackElemType::leftQuarterTurn5TilesCovered,
        .priceModifier = 257359,
        .mirrorElement = TrackElemType::rightQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::curve, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesSeq0, kLeftQuarterTurn5TilesSeq1, kLeftQuarterTurn5TilesSeq2,
                            kLeftQuarterTurn5TilesSeq3, kLeftQuarterTurn5TilesSeq4, kLeftQuarterTurn5TilesSeq5,
                            kLeftQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .alternativeType = TrackElemType::rightQuarterTurn5TilesCovered,
        .priceModifier = 257359,
        .mirrorElement = TrackElemType::leftQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::curve, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesSeq0, kRightQuarterTurn5TilesSeq1, kRightQuarterTurn5TilesSeq2,
                            kRightQuarterTurn5TilesSeq3, kRightQuarterTurn5TilesSeq4, kRightQuarterTurn5TilesSeq5,
                            kRightQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDFlatToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::flatToRightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatToLeftBankSeq0 } },
    };

    constexpr auto kTEDFlatToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::flatToLeftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatToRightBankSeq0 } },
    };

    constexpr auto kTEDLeftBankToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::rightBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .sequenceData = { 1, { kLeftBankToFlatSeq0 } },
    };

    constexpr auto kTEDRightBankToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::leftBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .sequenceData = { 1, { kRightBankToFlatSeq0 } },
    };

    constexpr auto kTEDBankedLeftQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 273443,
        .mirrorElement = TrackElemType::bankedRightQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kBankedLeftQuarterTurn5TilesSeq0, kBankedLeftQuarterTurn5TilesSeq1,
                            kBankedLeftQuarterTurn5TilesSeq2, kBankedLeftQuarterTurn5TilesSeq3,
                            kBankedLeftQuarterTurn5TilesSeq4, kBankedLeftQuarterTurn5TilesSeq5,
                            kBankedLeftQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDBankedRightQuarterTurn5Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 273443,
        .mirrorElement = TrackElemType::bankedLeftQuarterTurn5Tiles,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kBankedRightQuarterTurn5TilesSeq0, kBankedRightQuarterTurn5TilesSeq1,
                            kBankedRightQuarterTurn5TilesSeq2, kBankedRightQuarterTurn5TilesSeq3,
                            kBankedRightQuarterTurn5TilesSeq4, kBankedRightQuarterTurn5TilesSeq5,
                            kBankedRightQuarterTurn5TilesSeq6 } },
    };

    constexpr auto kTEDLeftBankToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::rightBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankToUp25Seq0 } },
    };

    constexpr auto kTEDRightBankToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::leftBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankToUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::up25ToRightBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToLeftBankSeq0 } },
    };

    constexpr auto kTEDUp25ToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::up25ToLeftBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::right, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToRightBankSeq0 } },
    };

    constexpr auto kTEDLeftBankToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::rightBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankToDown25Seq0 } },
    };

    constexpr auto kTEDRightBankToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::leftBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankToDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::down25ToRightBank,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToLeftBankSeq0 } },
    };

    constexpr auto kTEDDown25ToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 78336,
        .mirrorElement = TrackElemType::down25ToLeftBank,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToRightBankSeq0 } },
    };

    constexpr auto kTEDLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::rightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .sequenceData = { 1, { kLeftBankSeq0 } },
    };

    constexpr auto kTEDRightBank = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::leftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kRightBankSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn5TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 64, -64, -64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::rightQuarterTurn5TilesUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesUp25Seq0, kLeftQuarterTurn5TilesUp25Seq1, kLeftQuarterTurn5TilesUp25Seq2,
                            kLeftQuarterTurn5TilesUp25Seq3, kLeftQuarterTurn5TilesUp25Seq4, kLeftQuarterTurn5TilesUp25Seq5,
                            kLeftQuarterTurn5TilesUp25Seq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 64, -64, 64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::leftQuarterTurn5TilesUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesUp25Seq0, kRightQuarterTurn5TilesUp25Seq1, kRightQuarterTurn5TilesUp25Seq2,
                            kRightQuarterTurn5TilesUp25Seq3, kRightQuarterTurn5TilesUp25Seq4, kRightQuarterTurn5TilesUp25Seq5,
                            kRightQuarterTurn5TilesUp25Seq6 } },
    };

    constexpr auto kTEDLeftQuarterTurn5TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 64, 0, -64, -64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::rightQuarterTurn5TilesDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesDown25Seq0, kLeftQuarterTurn5TilesDown25Seq1,
                            kLeftQuarterTurn5TilesDown25Seq2, kLeftQuarterTurn5TilesDown25Seq3,
                            kLeftQuarterTurn5TilesDown25Seq4, kLeftQuarterTurn5TilesDown25Seq5,
                            kLeftQuarterTurn5TilesDown25Seq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 64, 0, -64, 64 },
        .pieceLength = 130,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 313656,
        .mirrorElement = TrackElemType::leftQuarterTurn5TilesDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesDown25Seq0, kRightQuarterTurn5TilesDown25Seq1,
                            kRightQuarterTurn5TilesDown25Seq2, kRightQuarterTurn5TilesDown25Seq3,
                            kRightQuarterTurn5TilesDown25Seq4, kRightQuarterTurn5TilesDown25Seq5,
                            kRightQuarterTurn5TilesDown25Seq6 } },
    };

    constexpr auto kTEDSBendLeft = TrackElementDescriptor{
        .description = STR_S_BEND_LEFT,
        .coordinates = { 0, 0, 0, 0, -64, -32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::sBendLeftCovered,
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::sBendRight,
        .flags = {},
        .definition = { TrackGroup::sBend, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::lr,
        .lateralFactor = EvaluatorSBendLeft,
        .sequenceData = { 4, { kSBendLeftSeq0, kSBendLeftSeq1, kSBendLeftSeq2, kSBendLeftSeq3 } },
    };

    constexpr auto kTEDSBendRight = TrackElementDescriptor{
        .description = STR_S_BEND_RIGHT,
        .coordinates = { 0, 0, 0, 0, -64, 32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .alternativeType = TrackElemType::sBendRightCovered,
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::sBendLeft,
        .flags = {},
        .definition = { TrackGroup::sBend, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::rl,
        .lateralFactor = EvaluatorSBendRight,
        .sequenceData = { 4, { kSBendRightSeq0, kSBendRightSeq1, kSBendRightSeq2, kSBendRightSeq3 } },
    };

    constexpr auto kTEDLeftVerticalLoop = TrackElementDescriptor{
        .description = STR_VERTICAL_LOOP_LEFT,
        .coordinates = { 0, 0, 0, 0, -32, -32 },
        .pieceLength = 120,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::rightVerticalLoop,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::verticalLoop, TrackPitch::down25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, -48 },
        .verticalFactor = EvaluatorVerticalLoop,
        .sequenceData = { 10,
                          { kLeftVerticalLoopSeq0, kLeftVerticalLoopSeq1, kLeftVerticalLoopSeq2, kLeftVerticalLoopSeq3,
                            kLeftVerticalLoopSeq4, kLeftVerticalLoopSeq5, kLeftVerticalLoopSeq6, kLeftVerticalLoopSeq7,
                            kLeftVerticalLoopSeq8, kLeftVerticalLoopSeq9 } },
    };

    constexpr auto kTEDRightVerticalLoop = TrackElementDescriptor{
        .description = STR_VERTICAL_LOOP_RIGHT,
        .coordinates = { 0, 0, 0, 0, -32, 32 },
        .pieceLength = 120,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::leftVerticalLoop,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::verticalLoop, TrackPitch::down25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, -48 },
        .verticalFactor = EvaluatorVerticalLoop,
        .sequenceData = { 10,
                          { kRightVerticalLoopSeq0, kRightVerticalLoopSeq1, kRightVerticalLoopSeq2, kRightVerticalLoopSeq3,
                            kRightVerticalLoopSeq4, kRightVerticalLoopSeq5, kRightVerticalLoopSeq6, kRightVerticalLoopSeq7,
                            kRightVerticalLoopSeq8, kRightVerticalLoopSeq9 } },
    };

    constexpr auto kTEDLeftQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .alternativeType = TrackElemType::leftQuarterTurn3TilesCovered,
        .priceModifier = 154415,
        .mirrorElement = TrackElemType::rightQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::curveSmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesSeq0, kLeftQuarterTurn3TilesSeq1, kLeftQuarterTurn3TilesSeq2,
                            kLeftQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .alternativeType = TrackElemType::rightQuarterTurn3TilesCovered,
        .priceModifier = 154415,
        .mirrorElement = TrackElemType::leftQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::curveSmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesSeq0, kRightQuarterTurn3TilesSeq1, kRightQuarterTurn3TilesSeq2,
                            kRightQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 164065,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 4,
                          { kLeftBankedQuarterTurn3TilesSeq0, kLeftBankedQuarterTurn3TilesSeq1,
                            kLeftBankedQuarterTurn3TilesSeq2, kLeftBankedQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn3Tiles = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 164065,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn3Tiles,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flatRollBanking, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 4,
                          { kRightBankedQuarterTurn3TilesSeq0, kRightBankedQuarterTurn3TilesSeq1,
                            kRightBankedQuarterTurn3TilesSeq2, kRightBankedQuarterTurn3TilesSeq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 32, -32, -32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesUp25Seq0, kLeftQuarterTurn3TilesUp25Seq1, kLeftQuarterTurn3TilesUp25Seq2,
                            kLeftQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 32, -32, 32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesUp25Seq0, kRightQuarterTurn3TilesUp25Seq1, kRightQuarterTurn3TilesUp25Seq2,
                            kRightQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 32, 0, -32, -32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesDown25Seq0, kLeftQuarterTurn3TilesDown25Seq1,
                            kLeftQuarterTurn3TilesDown25Seq2, kLeftQuarterTurn3TilesDown25Seq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 32, 0, -32, 32 },
        .pieceLength = 77,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesDown25Seq0, kRightQuarterTurn3TilesDown25Seq1,
                            kRightQuarterTurn3TilesDown25Seq2, kRightQuarterTurn3TilesDown25Seq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn1Tile = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, 0, 0 },
        .pieceLength = 24,
        .curveChain = { TrackCurve::leftVerySmall, TrackCurve::leftVerySmall },
        .priceModifier = 51471,
        .mirrorElement = TrackElemType::rightQuarterTurn1Tile,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::curveVerySmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l5,
        .lateralFactor = EvaluatorConst<45>,
        .sequenceData = { 1, { kLeftQuarterTurn1TileSeq0 } },
    };

    constexpr auto kTEDRightQuarterTurn1Tile = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, 0, 0 },
        .pieceLength = 24,
        .curveChain = { TrackCurve::rightVerySmall, TrackCurve::rightVerySmall },
        .priceModifier = 51471,
        .mirrorElement = TrackElemType::leftQuarterTurn1Tile,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::curveVerySmall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r5,
        .lateralFactor = EvaluatorConst<-45>,
        .sequenceData = { 1, { kRightQuarterTurn1TileSeq0 } },
    };

    constexpr auto kTEDLeftTwistDownToUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftTwistUpToDown, TrackCurve::none },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::rightTwistDownToUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftTwistDownToUpSeq0, kLeftTwistDownToUpSeq1, kLeftTwistDownToUpSeq2 } },
    };

    constexpr auto kTEDRightTwistDownToUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightTwistUpToDown, TrackCurve::none },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::leftTwistDownToUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightTwistDownToUpSeq0, kRightTwistDownToUpSeq1, kRightTwistDownToUpSeq2 } },
    };

    constexpr auto kTEDLeftTwistUpToDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftTwistDownToUp },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::rightTwistUpToDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftTwistUpToDownSeq0, kLeftTwistUpToDownSeq1, kLeftTwistUpToDownSeq2 } },
    };

    constexpr auto kTEDRightTwistUpToDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightTwistDownToUp },
        .priceModifier = 212992,
        .mirrorElement = TrackElemType::leftTwistUpToDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::twist, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightTwistUpToDownSeq0, kRightTwistUpToDownSeq1, kRightTwistUpToDownSeq2 } },
    };

    constexpr auto kTEDHalfLoopUp = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, 152, -32, 0 },
        .pieceLength = 60,
        .curveChain = { TrackElemType::halfLoopDown, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::halfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoop, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none, 64 },
        .verticalFactor = EvaluatorHalfLoopUp,
        .sequenceData = { 4, { kHalfLoopUpSeq0, kHalfLoopUpSeq1, kHalfLoopUpSeq2, kHalfLoopUpSeq3 } },
    };

    constexpr auto kTEDHalfLoopDown = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, -152, 32, 0 },
        .pieceLength = 60,
        .curveChain = { TrackCurve::none, TrackElemType::halfLoopUp },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::halfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoop, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -64 },
        .verticalFactor = EvaluatorHalfLoopDown,
        .sequenceData = { 4, { kHalfLoopDownSeq0, kHalfLoopDownSeq1, kHalfLoopDownSeq2, kHalfLoopDownSeq3 } },
    };

    constexpr auto kTEDLeftCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, 80, -32, -32 },
        .pieceLength = 55,
        .curveChain = { TrackElemType::rightCorkscrewDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kLeftCorkscrewUpSeq0, kLeftCorkscrewUpSeq1, kLeftCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDRightCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, 80, -32, 32 },
        .pieceLength = 55,
        .curveChain = { TrackElemType::leftCorkscrewDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kRightCorkscrewUpSeq0, kRightCorkscrewUpSeq1, kRightCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDLeftCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, -80, -32, -32 },
        .pieceLength = 55,
        .curveChain = { TrackCurve::none, TrackElemType::rightCorkscrewUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kLeftCorkscrewDownSeq0, kLeftCorkscrewDownSeq1, kLeftCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDRightCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, -80, -32, 32 },
        .pieceLength = 55,
        .curveChain = { TrackCurve::none, TrackElemType::leftCorkscrewUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrew, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kRightCorkscrewDownSeq0, kRightCorkscrewDownSeq1, kRightCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDFlatToUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 24, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::flatToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<56>,
        .sequenceData = { 1, { kFlatToUp60Seq0 } },
    };

    constexpr auto kTEDUp60ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 24, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::up60ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-56>,
        .sequenceData = { 1, { kUp60ToFlatSeq0 } },
    };

    constexpr auto kTEDFlatToDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 24, 0, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::flatToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-56>,
        .sequenceData = { 1, { kFlatToDown60Seq0 } },
    };

    constexpr auto kTEDDown60ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 24, 0, 0, 0 },
        .pieceLength = 36,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 98304,
        .mirrorElement = TrackElemType::down60ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flatToSteepSlope, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<56>,
        .sequenceData = { 1, { kDown60ToFlatSeq0 } },
    };

    constexpr auto kTEDTowerBase = TrackElementDescriptor{
        .description = STR_ENTRY_EXIT_PLATFORM,
        .coordinates = { 0, 0, 0, 96, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::towerSection, TrackCurve::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::towerBase,
        .flags = {},
        .definition = { TrackGroup::tower, TrackPitch::tower, TrackPitch::tower, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 9,
                          { kTowerBaseSeq0, kTowerBaseSeq1, kTowerBaseSeq2, kTowerBaseSeq3, kTowerBaseSeq4, kTowerBaseSeq5,
                            kTowerBaseSeq6, kTowerBaseSeq7, kTowerBaseSeq8 } },
    };

    constexpr auto kTEDTowerSection = TrackElementDescriptor{
        .description = STR_VERTICAL_TOWER,
        .coordinates = { 0, 0, 0, 32, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::towerSection, TrackCurve::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::towerSection,
        .flags = {},
        .definition = { TrackGroup::tower, TrackPitch::tower, TrackPitch::tower, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kTowerSectionSeq0, kTowerSectionSeq1 } },
    };

    constexpr auto kTEDFlatCovered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::flatCovered,
        .flags = {},
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatCoveredSeq0 } },
    };

    constexpr auto kTEDUp25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp25CoveredSeq0 } },
    };

    constexpr auto kTEDUp60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 64, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118784,
        .mirrorElement = TrackElemType::up60Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp60CoveredSeq0 } },
    };

    constexpr auto kTEDFlatToUp25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToUp25Covered,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToUp25CoveredSeq0 } },
    };

    constexpr auto kTEDUp25ToUp60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::up25ToUp60Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kUp25ToUp60CoveredSeq0 } },
    };

    constexpr auto kTEDUp60ToUp25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::up60ToUp25Covered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kUp60ToUp25CoveredSeq0 } },
    };

    constexpr auto kTEDUp25ToFlatCovered = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::up25ToFlatCovered,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kUp25ToFlatCoveredSeq0 } },
    };

    constexpr auto kTEDDown25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kDown25CoveredSeq0 } },
    };

    constexpr auto kTEDDown60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 64, 0, 0, 0 },
        .pieceLength = 40,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118784,
        .mirrorElement = TrackElemType::down60Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kDown60CoveredSeq0 } },
    };

    constexpr auto kTEDFlatToDown25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToDown25Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToDown25CoveredSeq0 } },
    };

    constexpr auto kTEDDown25ToDown60Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::down25ToDown60Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down60, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-82>,
        .sequenceData = { 1, { kDown25ToDown60CoveredSeq0 } },
    };

    constexpr auto kTEDDown60ToDown25Covered = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 0, 0 },
        .pieceLength = 34,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 100352,
        .mirrorElement = TrackElemType::down60ToDown25Covered,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<82>,
        .sequenceData = { 1, { kDown60ToDown25CoveredSeq0 } },
    };

    constexpr auto kTEDDown25ToFlatCovered = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::down25ToFlatCovered,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kDown25ToFlatCoveredSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn5TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 261455,
        .mirrorElement = TrackElemType::rightQuarterTurn5TilesCovered,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterTurn5TilesCoveredSeq0, kLeftQuarterTurn5TilesCoveredSeq1,
                            kLeftQuarterTurn5TilesCoveredSeq2, kLeftQuarterTurn5TilesCoveredSeq3,
                            kLeftQuarterTurn5TilesCoveredSeq4, kLeftQuarterTurn5TilesCoveredSeq5,
                            kLeftQuarterTurn5TilesCoveredSeq6 } },
    };

    constexpr auto kTEDRightQuarterTurn5TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 261455,
        .mirrorElement = TrackElemType::leftQuarterTurn5TilesCovered,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterTurn5TilesCoveredSeq0, kRightQuarterTurn5TilesCoveredSeq1,
                            kRightQuarterTurn5TilesCoveredSeq2, kRightQuarterTurn5TilesCoveredSeq3,
                            kRightQuarterTurn5TilesCoveredSeq4, kRightQuarterTurn5TilesCoveredSeq5,
                            kRightQuarterTurn5TilesCoveredSeq6 } },
    };

    constexpr auto kTEDSBendLeftCovered = TrackElementDescriptor{
        .description = STR_S_BEND_LEFT,
        .coordinates = { 0, 0, 0, 0, -64, -32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 233472,
        .mirrorElement = TrackElemType::sBendRightCovered,
        .flags = {},
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::lr,
        .lateralFactor = EvaluatorSBendLeft,
        .sequenceData = { 4, { kSBendLeftCoveredSeq0, kSBendLeftCoveredSeq1, kSBendLeftCoveredSeq2, kSBendLeftCoveredSeq3 } },
    };

    constexpr auto kTEDSBendRightCovered = TrackElementDescriptor{
        .description = STR_S_BEND_RIGHT,
        .coordinates = { 0, 0, 0, 0, -64, 32 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 233472,
        .mirrorElement = TrackElemType::sBendLeftCovered,
        .flags = {},
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::rl,
        .lateralFactor = EvaluatorSBendRight,
        .sequenceData = { 4,
                          { kSBendRightCoveredSeq0, kSBendRightCoveredSeq1, kSBendRightCoveredSeq2, kSBendRightCoveredSeq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 158511,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesCovered,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesCoveredSeq0, kLeftQuarterTurn3TilesCoveredSeq1,
                            kLeftQuarterTurn3TilesCoveredSeq2, kLeftQuarterTurn3TilesCoveredSeq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesCovered = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 75,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 158511,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesCovered,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesCoveredSeq0, kRightQuarterTurn3TilesCoveredSeq1,
                            kRightQuarterTurn3TilesCoveredSeq2, kRightQuarterTurn3TilesCoveredSeq3 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixUpSmall = TrackElementDescriptor{
        .description = STR_HELIX_UP_SMALL,
        .coordinates = { 0, 2, 0, 16, 0, -96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::leftHalfBankedHelixUpSmall, TrackElemType::leftHalfBankedHelixUpSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::rightHalfBankedHelixUpSmall,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 8,
                          { kLeftHalfBankedHelixUpSmallSeq0, kLeftHalfBankedHelixUpSmallSeq1, kLeftHalfBankedHelixUpSmallSeq2,
                            kLeftHalfBankedHelixUpSmallSeq3, kLeftHalfBankedHelixUpSmallSeq4, kLeftHalfBankedHelixUpSmallSeq5,
                            kLeftHalfBankedHelixUpSmallSeq6, kLeftHalfBankedHelixUpSmallSeq7 } },
    };

    constexpr auto kTEDRightHalfBankedHelixUpSmall = TrackElementDescriptor{
        .description = STR_HELIX_UP_SMALL,
        .coordinates = { 0, 2, 0, 16, 0, 96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::rightHalfBankedHelixUpSmall, TrackElemType::rightHalfBankedHelixUpSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::leftHalfBankedHelixUpSmall,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 8,
                          { kRightHalfBankedHelixUpSmallSeq0, kRightHalfBankedHelixUpSmallSeq1,
                            kRightHalfBankedHelixUpSmallSeq2, kRightHalfBankedHelixUpSmallSeq3,
                            kRightHalfBankedHelixUpSmallSeq4, kRightHalfBankedHelixUpSmallSeq5,
                            kRightHalfBankedHelixUpSmallSeq6, kRightHalfBankedHelixUpSmallSeq7 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixDownSmall = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_SMALL,
        .coordinates = { 0, 2, 16, 0, 0, -96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::leftHalfBankedHelixDownSmall, TrackElemType::leftHalfBankedHelixDownSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::rightHalfBankedHelixDownSmall,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 8,
                          { kLeftHalfBankedHelixDownSmallSeq0, kLeftHalfBankedHelixDownSmallSeq1,
                            kLeftHalfBankedHelixDownSmallSeq2, kLeftHalfBankedHelixDownSmallSeq3,
                            kLeftHalfBankedHelixDownSmallSeq4, kLeftHalfBankedHelixDownSmallSeq5,
                            kLeftHalfBankedHelixDownSmallSeq6, kLeftHalfBankedHelixDownSmallSeq7 } },
    };

    constexpr auto kTEDRightHalfBankedHelixDownSmall = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_SMALL,
        .coordinates = { 0, 2, 16, 0, 0, 96 },
        .pieceLength = 150,
        .curveChain = { TrackElemType::rightHalfBankedHelixDownSmall, TrackElemType::rightHalfBankedHelixDownSmall },
        .priceModifier = 328192,
        .mirrorElement = TrackElemType::leftHalfBankedHelixDownSmall,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<100>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 8,
                          { kRightHalfBankedHelixDownSmallSeq0, kRightHalfBankedHelixDownSmallSeq1,
                            kRightHalfBankedHelixDownSmallSeq2, kRightHalfBankedHelixDownSmallSeq3,
                            kRightHalfBankedHelixDownSmallSeq4, kRightHalfBankedHelixDownSmallSeq5,
                            kRightHalfBankedHelixDownSmallSeq6, kRightHalfBankedHelixDownSmallSeq7 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixUpLarge = TrackElementDescriptor{
        .description = STR_HELIX_UP_LARGE,
        .coordinates = { 0, 2, 0, 16, 0, -160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::leftHalfBankedHelixUpLarge, TrackElemType::leftHalfBankedHelixUpLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::rightHalfBankedHelixUpLarge,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 14,
                          { kLeftHalfBankedHelixUpLargeSeq0, kLeftHalfBankedHelixUpLargeSeq1, kLeftHalfBankedHelixUpLargeSeq2,
                            kLeftHalfBankedHelixUpLargeSeq3, kLeftHalfBankedHelixUpLargeSeq4, kLeftHalfBankedHelixUpLargeSeq5,
                            kLeftHalfBankedHelixUpLargeSeq6, kLeftHalfBankedHelixUpLargeSeq7, kLeftHalfBankedHelixUpLargeSeq8,
                            kLeftHalfBankedHelixUpLargeSeq9, kLeftHalfBankedHelixUpLargeSeq10, kLeftHalfBankedHelixUpLargeSeq11,
                            kLeftHalfBankedHelixUpLargeSeq12, kLeftHalfBankedHelixUpLargeSeq13 } },
    };

    constexpr auto kTEDRightHalfBankedHelixUpLarge = TrackElementDescriptor{
        .description = STR_HELIX_UP_LARGE,
        .coordinates = { 0, 2, 0, 16, 0, 160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::rightHalfBankedHelixUpLarge, TrackElemType::rightHalfBankedHelixUpLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::leftHalfBankedHelixUpLarge,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 14,
                          { kRightHalfBankedHelixUpLargeSeq0, kRightHalfBankedHelixUpLargeSeq1,
                            kRightHalfBankedHelixUpLargeSeq2, kRightHalfBankedHelixUpLargeSeq3,
                            kRightHalfBankedHelixUpLargeSeq4, kRightHalfBankedHelixUpLargeSeq5,
                            kRightHalfBankedHelixUpLargeSeq6, kRightHalfBankedHelixUpLargeSeq7,
                            kRightHalfBankedHelixUpLargeSeq8, kRightHalfBankedHelixUpLargeSeq9,
                            kRightHalfBankedHelixUpLargeSeq10, kRightHalfBankedHelixUpLargeSeq11,
                            kRightHalfBankedHelixUpLargeSeq12, kRightHalfBankedHelixUpLargeSeq13 } },
    };

    constexpr auto kTEDLeftHalfBankedHelixDownLarge = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LARGE,
        .coordinates = { 0, 2, 16, 0, 0, -160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::leftHalfBankedHelixDownLarge, TrackElemType::leftHalfBankedHelixDownLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::rightHalfBankedHelixDownLarge,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 14,
                          { kLeftHalfBankedHelixDownLargeSeq0, kLeftHalfBankedHelixDownLargeSeq1,
                            kLeftHalfBankedHelixDownLargeSeq2, kLeftHalfBankedHelixDownLargeSeq3,
                            kLeftHalfBankedHelixDownLargeSeq4, kLeftHalfBankedHelixDownLargeSeq5,
                            kLeftHalfBankedHelixDownLargeSeq6, kLeftHalfBankedHelixDownLargeSeq7,
                            kLeftHalfBankedHelixDownLargeSeq8, kLeftHalfBankedHelixDownLargeSeq9,
                            kLeftHalfBankedHelixDownLargeSeq10, kLeftHalfBankedHelixDownLargeSeq11,
                            kLeftHalfBankedHelixDownLargeSeq12, kLeftHalfBankedHelixDownLargeSeq13 } },
    };

    constexpr auto kTEDRightHalfBankedHelixDownLarge = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LARGE,
        .coordinates = { 0, 2, 16, 0, 0, 160 },
        .pieceLength = 248,
        .curveChain = { TrackElemType::rightHalfBankedHelixDownLarge, TrackElemType::rightHalfBankedHelixDownLarge },
        .priceModifier = 547072,
        .mirrorElement = TrackElemType::leftHalfBankedHelixDownLarge,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedHalf, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 14,
                          { kRightHalfBankedHelixDownLargeSeq0, kRightHalfBankedHelixDownLargeSeq1,
                            kRightHalfBankedHelixDownLargeSeq2, kRightHalfBankedHelixDownLargeSeq3,
                            kRightHalfBankedHelixDownLargeSeq4, kRightHalfBankedHelixDownLargeSeq5,
                            kRightHalfBankedHelixDownLargeSeq6, kRightHalfBankedHelixDownLargeSeq7,
                            kRightHalfBankedHelixDownLargeSeq8, kRightHalfBankedHelixDownLargeSeq9,
                            kRightHalfBankedHelixDownLargeSeq10, kRightHalfBankedHelixDownLargeSeq11,
                            kRightHalfBankedHelixDownLargeSeq12, kRightHalfBankedHelixDownLargeSeq13 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileUp60 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 64, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileUp60,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l5,
        .lateralFactor = EvaluatorConst<88>,
        .sequenceData = { 1, { kLeftQuarterTurn1TileUp60Seq0 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileUp60 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 64, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileUp60,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r5,
        .lateralFactor = EvaluatorConst<-88>,
        .sequenceData = { 1, { kRightQuarterTurn1TileUp60Seq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileDown60 = TrackElementDescriptor{
        .coordinates = { 0, 3, 64, 0, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileDown60,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .spinFunction = SpinFunction::l5,
        .lateralFactor = EvaluatorConst<88>,
        .sequenceData = { 1, { kLeftQuarterTurn1TileDown60Seq0 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileDown60 = TrackElementDescriptor{
        .coordinates = { 0, 1, 64, 0, 0, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileDown60,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down60, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .spinFunction = SpinFunction::r5,
        .lateralFactor = EvaluatorConst<-88>,
        .sequenceData = { 1, { kRightQuarterTurn1TileDown60Seq0 } },
    };

    constexpr auto kTEDBrakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::brakes, TrackElemType::brakes },
        .priceModifier = 90112,
        .mirrorElement = TrackElemType::brakes,
        .flags = {},
        .definition = { TrackGroup::brakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kBrakesSeq0 } },
    };

    constexpr auto kTEDBooster = TrackElementDescriptor{
        .description = STR_BOOSTER,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::booster, TrackElemType::booster },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::booster,
        .flags = {},
        .definition = { TrackGroup::booster, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::rc,
        .sequenceData = { 1, { kBoosterSeq0 } },
    };

    constexpr auto kTEDMaze = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::maze,
        .flags = { TrackElementFlag::onlyAboveGround },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1,
                          { kMazeSeq0, kMazeSeq1, kMazeSeq2, kMazeSeq3, kMazeSeq4, kMazeSeq5, kMazeSeq6, kMazeSeq7, kMazeSeq8,
                            kMazeSeq9, kMazeSeq10, kMazeSeq11, kMazeSeq12, kMazeSeq13, kMazeSeq14, kMazeSeq15 } },
    };

    constexpr auto kTEDLeftQuarterBankedHelixLargeUp = TrackElementDescriptor{
        .description = STR_HELIX_UP_LARGE,
        .coordinates = { 0, 3, 0, 16, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterBankedHelixLargeUp, TrackElemType::leftQuarterBankedHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterBankedHelixLargeUp,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftQuarterBankedHelixLargeUpSeq0, kLeftQuarterBankedHelixLargeUpSeq1,
                            kLeftQuarterBankedHelixLargeUpSeq2, kLeftQuarterBankedHelixLargeUpSeq3,
                            kLeftQuarterBankedHelixLargeUpSeq4, kLeftQuarterBankedHelixLargeUpSeq5,
                            kLeftQuarterBankedHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDRightQuarterBankedHelixLargeUp = TrackElementDescriptor{
        .description = STR_HELIX_UP_LARGE,
        .coordinates = { 0, 1, 0, 16, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterBankedHelixLargeUp, TrackElemType::rightQuarterBankedHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterBankedHelixLargeUp,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixUpBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightQuarterBankedHelixLargeUpSeq0, kRightQuarterBankedHelixLargeUpSeq1,
                            kRightQuarterBankedHelixLargeUpSeq2, kRightQuarterBankedHelixLargeUpSeq3,
                            kRightQuarterBankedHelixLargeUpSeq4, kRightQuarterBankedHelixLargeUpSeq5,
                            kRightQuarterBankedHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDLeftQuarterBankedHelixLargeDown = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LARGE,
        .coordinates = { 0, 3, 16, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterBankedHelixLargeDown, TrackElemType::leftQuarterBankedHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterBankedHelixLargeDown,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::left,
                        TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftQuarterBankedHelixLargeDownSeq0, kLeftQuarterBankedHelixLargeDownSeq1,
                            kLeftQuarterBankedHelixLargeDownSeq2, kLeftQuarterBankedHelixLargeDownSeq3,
                            kLeftQuarterBankedHelixLargeDownSeq4, kLeftQuarterBankedHelixLargeDownSeq5,
                            kLeftQuarterBankedHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDRightQuarterBankedHelixLargeDown = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LARGE,
        .coordinates = { 0, 1, 16, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterBankedHelixLargeDown, TrackElemType::rightQuarterBankedHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterBankedHelixLargeDown,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix, TrackElementFlag::banked },
        .definition = { TrackGroup::helixDownBankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightQuarterBankedHelixLargeDownSeq0, kRightQuarterBankedHelixLargeDownSeq1,
                            kRightQuarterBankedHelixLargeDownSeq2, kRightQuarterBankedHelixLargeDownSeq3,
                            kRightQuarterBankedHelixLargeDownSeq4, kRightQuarterBankedHelixLargeDownSeq5,
                            kRightQuarterBankedHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDLeftQuarterHelixLargeUp = TrackElementDescriptor{
        .description = STR_HELIX_UP_LEFT,
        .coordinates = { 0, 3, 0, 16, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterHelixLargeUp, TrackElemType::leftQuarterHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterHelixLargeUp,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix },
        .definition = { TrackGroup::helixUpUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterHelixLargeUpSeq0, kLeftQuarterHelixLargeUpSeq1, kLeftQuarterHelixLargeUpSeq2,
                            kLeftQuarterHelixLargeUpSeq3, kLeftQuarterHelixLargeUpSeq4, kLeftQuarterHelixLargeUpSeq5,
                            kLeftQuarterHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDRightQuarterHelixLargeUp = TrackElementDescriptor{
        .description = STR_HELIX_UP_RIGHT,
        .coordinates = { 0, 1, 0, 16, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterHelixLargeUp, TrackElemType::rightQuarterHelixLargeUp },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterHelixLargeUp,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix },
        .definition = { TrackGroup::helixUpUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterHelixLargeUpSeq0, kRightQuarterHelixLargeUpSeq1, kRightQuarterHelixLargeUpSeq2,
                            kRightQuarterHelixLargeUpSeq3, kRightQuarterHelixLargeUpSeq4, kRightQuarterHelixLargeUpSeq5,
                            kRightQuarterHelixLargeUpSeq6 } },
    };

    constexpr auto kTEDLeftQuarterHelixLargeDown = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_LEFT,
        .coordinates = { 0, 3, 16, 0, -64, -64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::leftQuarterHelixLargeDown, TrackElemType::leftQuarterHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::rightQuarterHelixLargeDown,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::helix },
        .definition = { TrackGroup::helixDownUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l8,
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 7,
                          { kLeftQuarterHelixLargeDownSeq0, kLeftQuarterHelixLargeDownSeq1, kLeftQuarterHelixLargeDownSeq2,
                            kLeftQuarterHelixLargeDownSeq3, kLeftQuarterHelixLargeDownSeq4, kLeftQuarterHelixLargeDownSeq5,
                            kLeftQuarterHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDRightQuarterHelixLargeDown = TrackElementDescriptor{
        .description = STR_HELIX_DOWN_RIGHT,
        .coordinates = { 0, 1, 16, 0, -64, 64 },
        .pieceLength = 124,
        .curveChain = { TrackElemType::rightQuarterHelixLargeDown, TrackElemType::rightQuarterHelixLargeDown },
        .priceModifier = 273539,
        .mirrorElement = TrackElemType::leftQuarterHelixLargeDown,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::helix },
        .definition = { TrackGroup::helixDownUnbankedQuarter, TrackPitch::none, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r8,
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 7,
                          { kRightQuarterHelixLargeDownSeq0, kRightQuarterHelixLargeDownSeq1, kRightQuarterHelixLargeDownSeq2,
                            kRightQuarterHelixLargeDownSeq3, kRightQuarterHelixLargeDownSeq4, kRightQuarterHelixLargeDownSeq5,
                            kRightQuarterHelixLargeDownSeq6 } },
    };

    constexpr auto kTEDUp25LeftBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25RightBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .sequenceData = { 1, { kUp25LeftBankedSeq0 } },
    };

    constexpr auto kTEDUp25RightBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25LeftBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kUp25RightBankedSeq0 } },
    };

    constexpr auto kTEDWaterfall = TrackElementDescriptor{
        .description = STR_WATERFALLS,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::waterfall, TrackElemType::waterfall },
        .priceModifier = 143360,
        .mirrorElement = TrackElemType::waterfall,
        .flags = {},
        .definition = { TrackGroup::waterfall, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kWaterfallSeq0 } },
    };

    constexpr auto kTEDRapids = TrackElementDescriptor{
        .description = STR_RAPIDS,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::rapids, TrackElemType::rapids },
        .priceModifier = 118784,
        .mirrorElement = TrackElemType::rapids,
        .flags = {},
        .definition = { TrackGroup::rapids, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::sp,
        .sequenceData = { 1, { kRapidsSeq0 } },
    };

    constexpr auto kTEDOnRidePhoto = TrackElementDescriptor{
        .description = STR_ON_RIDE_PHOTO_SECTION,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 196608,
        .mirrorElement = TrackElemType::onRidePhoto,
        .flags = {},
        .definition = { TrackGroup::onridePhoto, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kOnRidePhotoSeq0 } },
    };

    constexpr auto kTEDDown25LeftBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25RightBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::left,
                        0 },
        .sequenceData = { 1, { kDown25LeftBankedSeq0 } },
    };

    constexpr auto kTEDDown25RightBanked = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25LeftBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .sequenceData = { 1, { kDown25RightBankedSeq0 } },
    };

    constexpr auto kTEDWaterSplash = TrackElementDescriptor{
        .description = STR_WATER_SPLASH,
        .coordinates = { 0, 0, 16, 16, -128, 0 },
        .pieceLength = 128,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 393216,
        .mirrorElement = TrackElemType::waterSplash,
        .flags = {},
        .definition = { TrackGroup::waterSplash, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorWaterSplash,
        .sequenceData = { 5, { kWaterSplashSeq0, kWaterSplashSeq1, kWaterSplashSeq2, kWaterSplashSeq3, kWaterSplashSeq4 } },
    };

    constexpr auto kTEDFlatToUp60LongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 88, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::flatToUp60LongBase,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<160>,
        .sequenceData = { 4,
                          { kFlatToUp60LongBaseSeq0, kFlatToUp60LongBaseSeq1, kFlatToUp60LongBaseSeq2,
                            kFlatToUp60LongBaseSeq3 } },
    };

    constexpr auto kTEDUp60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 88, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::up60ToFlatLongBase,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-160>,
        .sequenceData = { 4,
                          { kUp60ToFlatLongBaseSeq0, kUp60ToFlatLongBaseSeq1, kUp60ToFlatLongBaseSeq2,
                            kUp60ToFlatLongBaseSeq3 } },
    };

    constexpr auto kTEDWhirlpool = TrackElementDescriptor{
        .description = STR_WHIRLPOOL,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 126976,
        .mirrorElement = TrackElemType::whirlpool,
        .flags = {},
        .definition = { TrackGroup::whirlpool, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r5,
        .sequenceData = { 1, { kWhirlpoolSeq0 } },
    };

    constexpr auto kTEDDown60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 88, 0, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::down60ToFlatLongBase,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<160>,
        .sequenceData = { 4,
                          { kDown60ToFlatLongBaseSeq0, kDown60ToFlatLongBaseSeq1, kDown60ToFlatLongBaseSeq2,
                            kDown60ToFlatLongBaseSeq3 } },
    };

    constexpr auto kTEDFlatToDown60LongBase = TrackElementDescriptor{
        .coordinates = { 0, 0, 88, 0, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 376832,
        .mirrorElement = TrackElemType::flatToDown60LongBase,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-160>,
        .sequenceData = { 4,
                          { kFlatToDown60LongBaseSeq0, kFlatToDown60LongBaseSeq1, kFlatToDown60LongBaseSeq2,
                            kFlatToDown60LongBaseSeq3 } },
    };

    constexpr auto kTEDCableLiftHill = TrackElementDescriptor{
        .description = STR_CABLE_LIFT_HILL,
        .coordinates = { 0, 0, 0, -96, -96, 0 },
        .pieceLength = 138,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 643072,
        .mirrorElement = TrackElemType::cableLiftHill,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::liftHillCable, TrackPitch::down60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 4, { kCableLiftHillSeq0, kCableLiftHillSeq1, kCableLiftHillSeq2, kCableLiftHillSeq3 } },
    };

    constexpr auto kTEDReverseFreefallSlope = TrackElementDescriptor{
        .description = STR_SLOPE_UP_TO_VERTICAL,
        .coordinates = { 0, 0, 0, 240, -160, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::reverseFreefallVertical, TrackCurve::none },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::reverseFreefallSlope,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::reverseFreefall, TrackPitch::reverseFreefall, TrackPitch::none, TrackRoll::none,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<120>,
        .sequenceData = { 7,
                          { kReverseFreefallSlopeSeq0, kReverseFreefallSlopeSeq1, kReverseFreefallSlopeSeq2,
                            kReverseFreefallSlopeSeq3, kReverseFreefallSlopeSeq4, kReverseFreefallSlopeSeq5,
                            kReverseFreefallSlopeSeq6 } },
    };

    constexpr auto kTEDReverseFreefallVertical = TrackElementDescriptor{
        .description = STR_VERTICAL_TRACK,
        .coordinates = { 0, 0, 0, 80, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::reverseFreefallVertical, TrackElemType::reverseFreefallVertical },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::reverseFreefallVertical,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::reverseFreefall, TrackPitch::reverseFreefall, TrackPitch::reverseFreefall, TrackRoll::none,
                        TrackRoll::none, 0 },
        .sequenceData = { 2, { kReverseFreefallVerticalSeq0, kReverseFreefallVerticalSeq1 } },
    };

    constexpr auto kTEDUp90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 32, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::up90,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::flat, TrackPitch::up90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kUp90Seq0, kUp90Seq1 } },
    };

    constexpr auto kTEDDown90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 32, 0, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 69632,
        .mirrorElement = TrackElemType::down90,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flat, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kDown90Seq0, kDown90Seq1 } },
    };

    constexpr auto kTEDUp60ToUp90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 56, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::up60ToUp90,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flat, TrackPitch::up90, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<110>,
        .sequenceData = { 2, { kUp60ToUp90Seq0, kUp60ToUp90Seq1 } },
    };

    constexpr auto kTEDDown90ToDown60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 56, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::down90ToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flat, TrackPitch::down60, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<110>,
        .sequenceData = { 1, { kDown90ToDown60Seq0 } },
    };

    constexpr auto kTEDUp90ToUp60 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 56, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::up90ToUp60,
        .flags = { TrackElementFlag::up },
        .definition = { TrackGroup::flat, TrackPitch::up60, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-110>,
        .sequenceData = { 1, { kUp90ToUp60Seq0 } },
    };

    constexpr auto kTEDDown60ToDown90 = TrackElementDescriptor{
        .coordinates = { 0, 0, 56, 0, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 114688,
        .mirrorElement = TrackElemType::down60ToDown90,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::down90, TrackPitch::down60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-110>,
        .sequenceData = { 2, { kDown60ToDown90Seq0, kDown60ToDown90Seq1 } },
    };

    constexpr auto kTEDBrakeForDrop = TrackElementDescriptor{
        .description = STR_HOLDING_BRAKE_FOR_DROP,
        .coordinates = { 0, 0, 24, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 147456,
        .mirrorElement = TrackElemType::brakeForDrop,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::brakeForDrop, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-56>,
        .sequenceData = { 1, { kBrakeForDropSeq0 } },
    };

    constexpr auto kTEDLeftEighthToDiag = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 0, -64, -32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::rightEighthToDiag,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToDiagSeq0, kLeftEighthToDiagSeq1, kLeftEighthToDiagSeq2, kLeftEighthToDiagSeq3,
                            kLeftEighthToDiagSeq4 } },
    };

    constexpr auto kTEDRightEighthToDiag = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::leftEighthToDiag,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToDiagSeq0, kRightEighthToDiagSeq1, kRightEighthToDiagSeq2, kRightEighthToDiagSeq3,
                            kRightEighthToDiagSeq4 } },
    };

    constexpr auto kTEDLeftEighthToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::rightEighthToOrthogonal,
        .flags = { TrackElementFlag::turnLeft },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToOrthogonalSeq0, kLeftEighthToOrthogonalSeq1, kLeftEighthToOrthogonalSeq2,
                            kLeftEighthToOrthogonalSeq3, kLeftEighthToOrthogonalSeq4 } },
    };

    constexpr auto kTEDRightEighthToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 0, -32, 64 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 180151,
        .mirrorElement = TrackElemType::leftEighthToOrthogonal,
        .flags = { TrackElementFlag::turnRight },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToOrthogonalSeq0, kRightEighthToOrthogonalSeq1, kRightEighthToOrthogonalSeq2,
                            kRightEighthToOrthogonalSeq3, kRightEighthToOrthogonalSeq4 } },
    };

    constexpr auto kTEDLeftEighthBankToDiag = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 0, -64, -32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::rightEighthBankToDiag,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToDiagSeq0, kLeftEighthBankToDiagSeq1, kLeftEighthBankToDiagSeq2,
                            kLeftEighthBankToDiagSeq3, kLeftEighthBankToDiagSeq4 } },
    };

    constexpr auto kTEDRightEighthBankToDiag = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::leftEighthBankToDiag,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToDiagSeq0, kRightEighthBankToDiagSeq1, kRightEighthBankToDiagSeq2,
                            kRightEighthBankToDiagSeq3, kRightEighthBankToDiagSeq4 } },
    };

    constexpr auto kTEDLeftEighthBankToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 0, -64, 32 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::rightEighthBankToOrthogonal,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToOrthogonalSeq0, kLeftEighthBankToOrthogonalSeq1, kLeftEighthBankToOrthogonalSeq2,
                            kLeftEighthBankToOrthogonalSeq3, kLeftEighthBankToOrthogonalSeq4 } },
    };

    constexpr auto kTEDRightEighthBankToOrthogonal = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 0, -32, 64 },
        .pieceLength = 87,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 190960,
        .mirrorElement = TrackElemType::leftEighthBankToOrthogonal,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnBanked, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToOrthogonalSeq0, kRightEighthBankToOrthogonalSeq1,
                            kRightEighthBankToOrthogonalSeq2, kRightEighthBankToOrthogonalSeq3,
                            kRightEighthBankToOrthogonalSeq4 } },
    };

    constexpr auto kTEDDiagFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 92681,
        .mirrorElement = TrackElemType::diagFlat,
        .flags = { TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
    };

    constexpr auto kTEDDiagUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 111820,
        .mirrorElement = TrackElemType::diagUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagUp25Seq0, kDiagUp25Seq1, kDiagUp25Seq2, kDiagUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 64, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 147456,
        .mirrorElement = TrackElemType::diagUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up60, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagUp60Seq0, kDiagUp60Seq1, kDiagUp60Seq2, kDiagUp60Seq3 } },
    };

    constexpr auto kTEDDiagFlatToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagFlatToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4, { kDiagFlatToUp25Seq0, kDiagFlatToUp25Seq1, kDiagFlatToUp25Seq2, kDiagFlatToUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToUp60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 32, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagUp25ToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up60, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<95>,
        .sequenceData = { 4, { kDiagUp25ToUp60Seq0, kDiagUp25ToUp60Seq1, kDiagUp25ToUp60Seq2, kDiagUp25ToUp60Seq3 } },
    };

    constexpr auto kTEDDiagUp60ToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 32, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagUp60ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up25, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-95>,
        .sequenceData = { 4, { kDiagUp60ToUp25Seq0, kDiagUp60ToUp25Seq1, kDiagUp60ToUp25Seq2, kDiagUp60ToUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4, { kDiagUp25ToFlatSeq0, kDiagUp25ToFlatSeq1, kDiagUp25ToFlatSeq2, kDiagUp25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 111820,
        .mirrorElement = TrackElemType::diagDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 64, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 147456,
        .mirrorElement = TrackElemType::diagDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down60, TrackPitch::down60, TrackRoll::none,
                        TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagDown60Seq0, kDiagDown60Seq1, kDiagDown60Seq2, kDiagDown60Seq3 } },
    };

    constexpr auto kTEDDiagFlatToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagFlatToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4, { kDiagFlatToDown25Seq0, kDiagFlatToDown25Seq1, kDiagFlatToDown25Seq2, kDiagFlatToDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToDown60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 32, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagDown25ToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down60, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-95>,
        .sequenceData = { 4,
                          { kDiagDown25ToDown60Seq0, kDiagDown25ToDown60Seq1, kDiagDown25ToDown60Seq2,
                            kDiagDown25ToDown60Seq3 } },
    };

    constexpr auto kTEDDiagDown60ToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 32, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 129024,
        .mirrorElement = TrackElemType::diagDown60ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down25, TrackPitch::down60, TrackRoll::none,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<95>,
        .sequenceData = { 4,
                          { kDiagDown60ToDown25Seq0, kDiagDown60ToDown25Seq1, kDiagDown60ToDown25Seq2,
                            kDiagDown60ToDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 102400,
        .mirrorElement = TrackElemType::diagDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlope, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4, { kDiagDown25ToFlatSeq0, kDiagDown25ToFlatSeq1, kDiagDown25ToFlatSeq2, kDiagDown25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToUp60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 24, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagFlatToUp60,
        .flags = { TrackElementFlag::up, TrackElementFlag::isSteepUp, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<60>,
        .sequenceData = { 4, { kDiagFlatToUp60Seq0, kDiagFlatToUp60Seq1, kDiagFlatToUp60Seq2, kDiagFlatToUp60Seq3 } },
    };

    constexpr auto kTEDDiagUp60ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 24, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagUp60ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp,
                   TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepUp, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-60>,
        .sequenceData = { 4, { kDiagUp60ToFlatSeq0, kDiagUp60ToFlatSeq1, kDiagUp60ToFlatSeq2, kDiagUp60ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToDown60 = TrackElementDescriptor{
        .coordinates = { 4, 4, 24, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagFlatToDown60,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-60>,
        .sequenceData = { 4, { kDiagFlatToDown60Seq0, kDiagFlatToDown60Seq1, kDiagFlatToDown60Seq2, kDiagFlatToDown60Seq3 } },
    };

    constexpr auto kTEDDiagDown60ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 24, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 107008,
        .mirrorElement = TrackElemType::diagDown60ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::allowLiftHill },
        .definition = { TrackGroup::diagSlopeSteepDown, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<60>,
        .sequenceData = { 4, { kDiagDown60ToFlatSeq0, kDiagDown60ToFlatSeq1, kDiagDown60ToFlatSeq2, kDiagDown60ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::none },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagFlatToRightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagFlatToLeftBankSeq0, kDiagFlatToLeftBankSeq1, kDiagFlatToLeftBankSeq2,
                            kDiagFlatToLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagFlatToRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::none },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagFlatToLeftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagFlatToRightBankSeq0, kDiagFlatToRightBankSeq1, kDiagFlatToRightBankSeq2,
                            kDiagFlatToRightBankSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::leftLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagRightBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .sequenceData = { 4,
                          { kDiagLeftBankToFlatSeq0, kDiagLeftBankToFlatSeq1, kDiagLeftBankToFlatSeq2,
                            kDiagLeftBankToFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::rightLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagLeftBankToFlat,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagRightBankToFlatSeq0, kDiagRightBankToFlatSeq1, kDiagRightBankToFlatSeq2,
                            kDiagRightBankToFlatSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::leftLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagRightBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankToUp25Seq0, kDiagLeftBankToUp25Seq1, kDiagLeftBankToUp25Seq2,
                            kDiagLeftBankToUp25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::rightLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagLeftBankToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankToUp25Seq0, kDiagRightBankToUp25Seq1, kDiagRightBankToUp25Seq2,
                            kDiagRightBankToUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagUp25ToRightBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagUp25ToLeftBankSeq0, kDiagUp25ToLeftBankSeq1, kDiagUp25ToLeftBankSeq2,
                            kDiagUp25ToLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagUp25ToRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagUp25ToLeftBank,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::up25, TrackRoll::right, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagUp25ToRightBankSeq0, kDiagUp25ToRightBankSeq1, kDiagUp25ToRightBankSeq2,
                            kDiagUp25ToRightBankSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::leftLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagRightBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankToDown25Seq0, kDiagLeftBankToDown25Seq1, kDiagLeftBankToDown25Seq2,
                            kDiagLeftBankToDown25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::rightLarge },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagLeftBankToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankToDown25Seq0, kDiagRightBankToDown25Seq1, kDiagRightBankToDown25Seq2,
                            kDiagRightBankToDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagDown25ToRightBank,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagDown25ToLeftBankSeq0, kDiagDown25ToLeftBankSeq1, kDiagDown25ToLeftBankSeq2,
                            kDiagDown25ToLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagDown25ToRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::none },
        .priceModifier = 106496,
        .mirrorElement = TrackElemType::diagDown25ToLeftBank,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagDown25ToRightBankSeq0, kDiagDown25ToRightBankSeq1, kDiagDown25ToRightBankSeq2,
                            kDiagDown25ToRightBankSeq3 } },
    };

    constexpr auto kTEDDiagLeftBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagRightBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .sequenceData = { 4, { kDiagLeftBankSeq0, kDiagLeftBankSeq1, kDiagLeftBankSeq2, kDiagLeftBankSeq3 } },
    };

    constexpr auto kTEDDiagRightBank = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 96777,
        .mirrorElement = TrackElemType::diagLeftBank,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::none, TrackRoll::right, TrackRoll::right, 0 },
        .sequenceData = { 4, { kDiagRightBankSeq0, kDiagRightBankSeq1, kDiagRightBankSeq2, kDiagRightBankSeq3 } },
    };

    constexpr auto kTEDLogFlumeReverser = TrackElementDescriptor{
        .description = STR_REVERSER_TURNTABLE,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 16,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 282624,
        .mirrorElement = TrackElemType::logFlumeReverser,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::logFlumeReverser, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kLogFlumeReverserSeq0 } },
    };

    constexpr auto kTEDSpinningTunnel = TrackElementDescriptor{
        .description = STR_SPINNING_TUNNEL,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 217088,
        .mirrorElement = TrackElemType::spinningTunnel,
        .flags = { TrackElementFlag::banked },
        .definition = { TrackGroup::spinningTunnel, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kSpinningTunnelSeq0 } },
    };

    constexpr auto kTEDLeftBarrelRollUpToDown = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftBarrelRollDownToUp, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightBarrelRollUpToDown,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<115>,
        .sequenceData = { 3, { kLeftBarrelRollUpToDownSeq0, kLeftBarrelRollUpToDownSeq1, kLeftBarrelRollUpToDownSeq2 } },
    };

    constexpr auto kTEDRightBarrelRollUpToDown = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightBarrelRollDownToUp, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftBarrelRollUpToDown,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<-115>,
        .sequenceData = { 3, { kRightBarrelRollUpToDownSeq0, kRightBarrelRollUpToDownSeq1, kRightBarrelRollUpToDownSeq2 } },
    };

    constexpr auto kTEDLeftBarrelRollDownToUp = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_LEFT,
        .coordinates = { 0, 0, 0, -32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftBarrelRollUpToDown },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightBarrelRollDownToUp,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<115>,
        .sequenceData = { 3, { kLeftBarrelRollDownToUpSeq0, kLeftBarrelRollDownToUpSeq1, kLeftBarrelRollDownToUpSeq2 } },
    };

    constexpr auto kTEDRightBarrelRollDownToUp = TrackElementDescriptor{
        .description = STR_BARREL_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, -32, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightBarrelRollUpToDown },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftBarrelRollDownToUp,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::barrelRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown, 0 },
        .verticalFactor = EvaluatorConst<170>,
        .lateralFactor = EvaluatorConst<-115>,
        .sequenceData = { 3, { kRightBarrelRollDownToUpSeq0, kRightBarrelRollDownToUpSeq1, kRightBarrelRollDownToUpSeq2 } },
    };

    constexpr auto kTEDLeftBankToLeftQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 24, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightBankToRightQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<90>,
        .sequenceData = { 4,
                          { kLeftBankToLeftQuarterTurn3TilesUp25Seq0, kLeftBankToLeftQuarterTurn3TilesUp25Seq1,
                            kLeftBankToLeftQuarterTurn3TilesUp25Seq2, kLeftBankToLeftQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDRightBankToRightQuarterTurn3TilesUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 24, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftBankToLeftQuarterTurn3TilesUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::up25, TrackPitch::none, TrackRoll::none, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<-90>,
        .sequenceData = { 4,
                          { kRightBankToRightQuarterTurn3TilesUp25Seq0, kRightBankToRightQuarterTurn3TilesUp25Seq1,
                            kRightBankToRightQuarterTurn3TilesUp25Seq2, kRightBankToRightQuarterTurn3TilesUp25Seq3 } },
    };

    constexpr auto kTEDLeftQuarterTurn3TilesDown25ToLeftBank = TrackElementDescriptor{
        .coordinates = { 0, 3, 24, 0, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightQuarterTurn3TilesDown25ToRightBank,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<90>,
        .sequenceData = { 4,
                          { kLeftQuarterTurn3TilesDown25ToLeftBankSeq0, kLeftQuarterTurn3TilesDown25ToLeftBankSeq1,
                            kLeftQuarterTurn3TilesDown25ToLeftBankSeq2, kLeftQuarterTurn3TilesDown25ToLeftBankSeq3 } },
    };

    constexpr auto kTEDRightQuarterTurn3TilesDown25ToRightBank = TrackElementDescriptor{
        .coordinates = { 0, 1, 24, 0, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::banked },
        .definition = { TrackGroup::flat, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorQuarterTurn3Tiles,
        .lateralFactor = EvaluatorConst<-90>,
        .sequenceData = { 4,
                          { kRightQuarterTurn3TilesDown25ToRightBankSeq0, kRightQuarterTurn3TilesDown25ToRightBankSeq1,
                            kRightQuarterTurn3TilesDown25ToRightBankSeq2, kRightQuarterTurn3TilesDown25ToRightBankSeq3 } },
    };

    constexpr auto kTEDPoweredLift = TrackElementDescriptor{
        .description = STR_LAUNCHED_LIFT_HILL,
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::poweredLift, TrackElemType::poweredLift },
        .priceModifier = 96384,
        .mirrorElement = TrackElemType::poweredLift,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::poweredLift, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kPoweredLiftSeq0 } },
    };

    constexpr auto kTEDLeftLargeHalfLoopUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 280, -64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::rightLargeHalfLoopDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightLargeHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kLeftLargeHalfLoopUpSeq0, kLeftLargeHalfLoopUpSeq1, kLeftLargeHalfLoopUpSeq2,
                            kLeftLargeHalfLoopUpSeq3, kLeftLargeHalfLoopUpSeq4, kLeftLargeHalfLoopUpSeq5,
                            kLeftLargeHalfLoopUpSeq6 } },
    };

    constexpr auto kTEDRightLargeHalfLoopUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 280, -64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::leftLargeHalfLoopDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftLargeHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kRightLargeHalfLoopUpSeq0, kRightLargeHalfLoopUpSeq1, kRightLargeHalfLoopUpSeq2,
                            kRightLargeHalfLoopUpSeq3, kRightLargeHalfLoopUpSeq4, kRightLargeHalfLoopUpSeq5,
                            kRightLargeHalfLoopUpSeq6 } },
    };

    constexpr auto kTEDLeftLargeHalfLoopDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -280, 64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::rightLargeHalfLoopUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightLargeHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kLeftLargeHalfLoopDownSeq0, kLeftLargeHalfLoopDownSeq1, kLeftLargeHalfLoopDownSeq2,
                            kLeftLargeHalfLoopDownSeq3, kLeftLargeHalfLoopDownSeq4, kLeftLargeHalfLoopDownSeq5,
                            kLeftLargeHalfLoopDownSeq6 } },
    };

    constexpr auto kTEDRightLargeHalfLoopDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -280, 64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::leftLargeHalfLoopUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftLargeHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopLarge, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kRightLargeHalfLoopDownSeq0, kRightLargeHalfLoopDownSeq1, kRightLargeHalfLoopDownSeq2,
                            kRightLargeHalfLoopDownSeq3, kRightLargeHalfLoopDownSeq4, kRightLargeHalfLoopDownSeq5,
                            kRightLargeHalfLoopDownSeq6 } },
    };

    constexpr auto kTEDLeftFlyerTwistUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightFlyerTwistUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftFlyerTwistUpSeq0, kLeftFlyerTwistUpSeq1, kLeftFlyerTwistUpSeq2 } },
    };

    constexpr auto kTEDRightFlyerTwistUp = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, -16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftFlyerTwistUp,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightFlyerTwistUpSeq0, kRightFlyerTwistUpSeq1, kRightFlyerTwistUpSeq2 } },
    };

    constexpr auto kTEDLeftFlyerTwistDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_LEFT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::rightFlyerTwistDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 3, { kLeftFlyerTwistDownSeq0, kLeftFlyerTwistDownSeq1, kLeftFlyerTwistDownSeq2 } },
    };

    constexpr auto kTEDRightFlyerTwistDown = TrackElementDescriptor{
        .description = STR_IN_LINE_TWIST_RIGHT,
        .coordinates = { 0, 0, 0, 16, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 221184,
        .mirrorElement = TrackElemType::leftFlyerTwistDown,
        .flags = { TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::inlineTwistInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 3, { kRightFlyerTwistDownSeq0, kRightFlyerTwistDownSeq1, kRightFlyerTwistDownSeq2 } },
    };

    constexpr auto kTEDFlyerHalfLoopUninvertedUp = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, 120, -32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopUninvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingHalfLoopUninvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorHalfLoopUp,
        .sequenceData = { 4,
                          { kFlyerHalfLoopUninvertedUpSeq0, kFlyerHalfLoopUninvertedUpSeq1, kFlyerHalfLoopUninvertedUpSeq2,
                            kFlyerHalfLoopUninvertedUpSeq3 } },
    };

    constexpr auto kTEDFlyerHalfLoopInvertedDown = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, -120, 32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopInvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingHalfLoopInvertedDown, TrackPitch::down25, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -64 },
        .verticalFactor = EvaluatorHalfLoopDown,
        .sequenceData = { 4,
                          { kFlyerHalfLoopInvertedDownSeq0, kFlyerHalfLoopInvertedDownSeq1, kFlyerHalfLoopInvertedDownSeq2,
                            kFlyerHalfLoopInvertedDownSeq3 } },
    };

    constexpr auto kTEDLeftFlyerCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, 48, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightFlyerCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kLeftFlyerCorkscrewUpSeq0, kLeftFlyerCorkscrewUpSeq1, kLeftFlyerCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDRightFlyerCorkscrewUp = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, 48, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftFlyerCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewUninverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kRightFlyerCorkscrewUpSeq0, kRightFlyerCorkscrewUpSeq1, kRightFlyerCorkscrewUpSeq2 } },
    };

    constexpr auto kTEDLeftFlyerCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, -48, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightFlyerCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<-70>,
        .sequenceData = { 3, { kLeftFlyerCorkscrewDownSeq0, kLeftFlyerCorkscrewDownSeq1, kLeftFlyerCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDRightFlyerCorkscrewDown = TrackElementDescriptor{
        .description = STR_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, -48, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftFlyerCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewInverted, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -32 },
        .verticalFactor = EvaluatorConst<52>,
        .lateralFactor = EvaluatorConst<70>,
        .sequenceData = { 3, { kRightFlyerCorkscrewDownSeq0, kRightFlyerCorkscrewDownSeq1, kRightFlyerCorkscrewDownSeq2 } },
    };

    constexpr auto kTEDHeartLineTransferUp = TrackElementDescriptor{
        .description = STR_UPPER_TRANSFER,
        .coordinates = { 0, 2, 0, 32, 0, 0 },
        .pieceLength = 16,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::heartLineTransferUp,
        .flags = {},
        .definition = { TrackGroup::heartlineTransfer, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorHeartLineTransferUp,
        .sequenceData = { 4,
                          { kHeartLineTransferUpSeq0, kHeartLineTransferUpSeq1, kHeartLineTransferUpSeq2,
                            kHeartLineTransferUpSeq3 } },
    };

    constexpr auto kTEDHeartLineTransferDown = TrackElementDescriptor{
        .description = STR_LOWER_TRANSFER,
        .coordinates = { 0, 2, 0, -32, 0, 0 },
        .pieceLength = 16,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::heartLineTransferDown,
        .flags = {},
        .definition = { TrackGroup::heartlineTransfer, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorHeartLineTransferDown,
        .sequenceData = { 4,
                          { kHeartLineTransferDownSeq0, kHeartLineTransferDownSeq1, kHeartLineTransferDownSeq2,
                            kHeartLineTransferDownSeq3 } },
    };

    constexpr auto kTEDLeftHeartLineRoll = TrackElementDescriptor{
        .description = STR_HEARTLINE_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 0, -160, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 417792,
        .mirrorElement = TrackElemType::rightHeartLineRoll,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal,
                   TrackElementFlag::canBePartlyUnderground },
        .definition = { TrackGroup::heartlineRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<98>,
        .sequenceData = { 6,
                          { kLeftHeartLineRollSeq0, kLeftHeartLineRollSeq1, kLeftHeartLineRollSeq2, kLeftHeartLineRollSeq3,
                            kLeftHeartLineRollSeq4, kLeftHeartLineRollSeq5 } },
    };

    constexpr auto kTEDRightHeartLineRoll = TrackElementDescriptor{
        .description = STR_HEARTLINE_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 0, -160, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 417792,
        .mirrorElement = TrackElemType::leftHeartLineRoll,
        .flags = { TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal,
                   TrackElementFlag::canBePartlyUnderground },
        .definition = { TrackGroup::heartlineRoll, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .lateralFactor = EvaluatorConst<-98>,
        .sequenceData = { 6,
                          { kRightHeartLineRollSeq0, kRightHeartLineRollSeq1, kRightHeartLineRollSeq2, kRightHeartLineRollSeq3,
                            kRightHeartLineRollSeq4, kRightHeartLineRollSeq5 } },
    };

    constexpr auto kTEDMinigolfHoleA = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_A,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 159744,
        .mirrorElement = TrackElemType::minigolfHoleA,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kMinigolfHoleASeq0, kMinigolfHoleASeq1 } },
    };

    constexpr auto kTEDMinigolfHoleB = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_B,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 167936,
        .mirrorElement = TrackElemType::minigolfHoleB,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kMinigolfHoleBSeq0, kMinigolfHoleBSeq1 } },
    };

    constexpr auto kTEDMinigolfHoleC = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_C,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 172032,
        .mirrorElement = TrackElemType::minigolfHoleC,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kMinigolfHoleCSeq0, kMinigolfHoleCSeq1 } },
    };

    constexpr auto kTEDMinigolfHoleD = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_D,
        .coordinates = { 0, 1, 0, 0, -32, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 184320,
        .mirrorElement = TrackElemType::minigolfHoleE,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 3, { kMinigolfHoleDSeq0, kMinigolfHoleDSeq1, kMinigolfHoleDSeq2 } },
    };

    constexpr auto kTEDMinigolfHoleE = TrackElementDescriptor{
        .description = STR_GOLF_HOLE_E,
        .coordinates = { 0, 3, 0, 0, -32, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 184320,
        .mirrorElement = TrackElemType::minigolfHoleD,
        .flags = { TrackElementFlag::isGolfHole },
        .definition = { TrackGroup::miniGolfHole, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 3, { kMinigolfHoleESeq0, kMinigolfHoleESeq1, kMinigolfHoleESeq2 } },
    };

    constexpr auto kTEDMultiDimInvertedFlatToDown90QuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, -96, -96, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimInvertedFlatToDown90QuarterLoop,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopInvertedDown, TrackPitch::down90, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorDown90QuarterLoop,
        .sequenceData = { 4,
                          { kMultiDimInvertedFlatToDown90QuarterLoopSeq0, kMultiDimInvertedFlatToDown90QuarterLoopSeq1,
                            kMultiDimInvertedFlatToDown90QuarterLoopSeq2, kMultiDimInvertedFlatToDown90QuarterLoopSeq3 } },
    };

    constexpr auto kTEDUp90ToInvertedFlatQuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, 128, 64, 0 },
        .pieceLength = 80,
        .curveChain = { TrackElemType::invertedFlatToDown90QuarterLoop, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::up90ToInvertedFlatQuarterLoop,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoop, TrackPitch::none, TrackPitch::up90, TrackRoll::upsideDown, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorUp90QuarterLoop,
        .sequenceData = { 3,
                          { kUp90ToInvertedFlatQuarterLoopSeq0, kUp90ToInvertedFlatQuarterLoopSeq1,
                            kUp90ToInvertedFlatQuarterLoopSeq2 } },
    };

    constexpr auto kTEDInvertedFlatToDown90QuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, -128, -96, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackElemType::up90ToInvertedFlatQuarterLoop },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::invertedFlatToDown90QuarterLoop,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoop, TrackPitch::down90, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        0 },
        .verticalFactor = EvaluatorDown90QuarterLoop,
        .sequenceData = { 4,
                          { kInvertedFlatToDown90QuarterLoopSeq0, kInvertedFlatToDown90QuarterLoopSeq1,
                            kInvertedFlatToDown90QuarterLoopSeq2, kInvertedFlatToDown90QuarterLoopSeq3 } },
    };

    constexpr auto kTEDLeftCurvedLiftHill = TrackElementDescriptor{
        .description = STR_CURVED_LIFT_HILL_LEFT,
        .coordinates = { 0, 3, 0, 16, -32, -32 },
        .pieceLength = 64,
        .curveChain = { TrackElemType::leftCurvedLiftHill, TrackElemType::leftCurvedLiftHill },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::rightCurvedLiftHill,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::up },
        .definition = { TrackGroup::liftHillCurved, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::l7,
        .lateralFactor = EvaluatorConst<59>,
        .sequenceData = { 4,
                          { kLeftCurvedLiftHillSeq0, kLeftCurvedLiftHillSeq1, kLeftCurvedLiftHillSeq2,
                            kLeftCurvedLiftHillSeq3 } },
    };

    constexpr auto kTEDRightCurvedLiftHill = TrackElementDescriptor{
        .description = STR_CURVED_LIFT_HILL_RIGHT,
        .coordinates = { 0, 1, 0, 16, -32, 32 },
        .pieceLength = 64,
        .curveChain = { TrackElemType::rightCurvedLiftHill, TrackElemType::rightCurvedLiftHill },
        .priceModifier = 270226,
        .mirrorElement = TrackElemType::leftCurvedLiftHill,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::up },
        .definition = { TrackGroup::liftHillCurved, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .spinFunction = SpinFunction::r7,
        .lateralFactor = EvaluatorConst<-59>,
        .sequenceData = { 4,
                          { kRightCurvedLiftHillSeq0, kRightCurvedLiftHillSeq1, kRightCurvedLiftHillSeq2,
                            kRightCurvedLiftHillSeq3 } },
    };

    constexpr auto kTEDLeftReverser = TrackElementDescriptor{
        .description = STR_REVERSER_LEFT,
        .coordinates = { 0, 0, 0, 0, -64, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 393216,
        .mirrorElement = TrackElemType::rightReverser,
        .flags = {},
        .definition = { TrackGroup::reverser, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 6,
                          { kLeftReverserSeq0, kLeftReverserSeq1, kLeftReverserSeq2, kLeftReverserSeq3, kLeftReverserSeq4,
                            kLeftReverserSeq5 } },
    };

    constexpr auto kTEDRightReverser = TrackElementDescriptor{
        .description = STR_REVERSER_RIGHT,
        .coordinates = { 0, 0, 0, 0, -64, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 393216,
        .mirrorElement = TrackElemType::leftReverser,
        .flags = {},
        .definition = { TrackGroup::reverser, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 6,
                          { kRightReverserSeq0, kRightReverserSeq1, kRightReverserSeq2, kRightReverserSeq3, kRightReverserSeq4,
                            kRightReverserSeq5 } },
    };

    constexpr auto kTEDAirThrustTopCap = TrackElementDescriptor{
        .description = STR_TOP_SECTION,
        .coordinates = { 0, 0, 0, 0, -32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::airThrustVerticalDown, TrackElemType::reverseFreefallVertical },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::airThrustTopCap,
        .flags = {},
        .definition = { TrackGroup::slopeToFlat, TrackPitch::down90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-60>,
        .sequenceData = { 4, { kAirThrustTopCapSeq0, kAirThrustTopCapSeq1, kAirThrustTopCapSeq2, kAirThrustTopCapSeq3 } },
    };

    constexpr auto kTEDAirThrustVerticalDown = TrackElementDescriptor{
        .description = STR_VERTICAL_TRACK,
        .coordinates = { 0, 0, 80, 0, 32, 0 },
        .pieceLength = 32,
        .curveChain = { TrackElemType::airThrustVerticalDown, TrackElemType::airThrustVerticalDown },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::airThrustVerticalDown,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeToFlat, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kAirThrustVerticalDownSeq0, kAirThrustVerticalDownSeq1 } },
    };

    constexpr auto kTEDAirThrustVerticalDownToLevel = TrackElementDescriptor{
        .description = STR_SLOPE_TO_LEVEL,
        .coordinates = { 0, 0, 240, 0, -160, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackElemType::airThrustVerticalDown },
        .priceModifier = 491520,
        .mirrorElement = TrackElemType::airThrustVerticalDownToLevel,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeToFlat, TrackPitch::none, TrackPitch::down90, TrackRoll::none, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<120>,
        .sequenceData = { 7,
                          { kAirThrustVerticalDownToLevelSeq0, kAirThrustVerticalDownToLevelSeq1,
                            kAirThrustVerticalDownToLevelSeq2, kAirThrustVerticalDownToLevelSeq3,
                            kAirThrustVerticalDownToLevelSeq4, kAirThrustVerticalDownToLevelSeq5,
                            kAirThrustVerticalDownToLevelSeq6 } },
    };

    constexpr auto kTEDBlockBrakes = TrackElementDescriptor{
        .description = STR_BLOCK_BRAKES,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 94208,
        .mirrorElement = TrackElemType::blockBrakes,
        .flags = {},
        .definition = { TrackGroup::blockBrakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kBlockBrakesSeq0 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn3TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 32, -32, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn3TileUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 4,
                          { kLeftBankedQuarterTurn3TileUp25Seq0, kLeftBankedQuarterTurn3TileUp25Seq1,
                            kLeftBankedQuarterTurn3TileUp25Seq2, kLeftBankedQuarterTurn3TileUp25Seq3 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn3TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 32, -32, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn3TileUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 4,
                          { kRightBankedQuarterTurn3TileUp25Seq0, kRightBankedQuarterTurn3TileUp25Seq1,
                            kRightBankedQuarterTurn3TileUp25Seq2, kRightBankedQuarterTurn3TileUp25Seq3 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn3TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 32, 0, -32, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn3TileDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<100>,
        .sequenceData = { 4,
                          { kLeftBankedQuarterTurn3TileDown25Seq0, kLeftBankedQuarterTurn3TileDown25Seq1,
                            kLeftBankedQuarterTurn3TileDown25Seq2, kLeftBankedQuarterTurn3TileDown25Seq3 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn3TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 32, 0, -32, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 287115,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn3TileDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r7,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-100>,
        .sequenceData = { 4,
                          { kRightBankedQuarterTurn3TileDown25Seq0, kRightBankedQuarterTurn3TileDown25Seq1,
                            kRightBankedQuarterTurn3TileDown25Seq2, kRightBankedQuarterTurn3TileDown25Seq3 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn5TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 64, -64, -64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn5TileUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftBankedQuarterTurn5TileUp25Seq0, kLeftBankedQuarterTurn5TileUp25Seq1,
                            kLeftBankedQuarterTurn5TileUp25Seq2, kLeftBankedQuarterTurn5TileUp25Seq3,
                            kLeftBankedQuarterTurn5TileUp25Seq4, kLeftBankedQuarterTurn5TileUp25Seq5,
                            kLeftBankedQuarterTurn5TileUp25Seq6, kLeftBankedQuarterTurn5TileUp25Seq7 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn5TileUp25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 64, -64, 64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn5TileUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightBankedQuarterTurn5TileUp25Seq0, kRightBankedQuarterTurn5TileUp25Seq1,
                            kRightBankedQuarterTurn5TileUp25Seq2, kRightBankedQuarterTurn5TileUp25Seq3,
                            kRightBankedQuarterTurn5TileUp25Seq4, kRightBankedQuarterTurn5TileUp25Seq5,
                            kRightBankedQuarterTurn5TileUp25Seq6, kRightBankedQuarterTurn5TileUp25Seq7 } },
    };

    constexpr auto kTEDLeftBankedQuarterTurn5TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 3, 64, 0, -64, -64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::rightBankedQuarterTurn5TileDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::left, 0 },
        .spinFunction = SpinFunction::l8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<160>,
        .sequenceData = { 7,
                          { kLeftBankedQuarterTurn5TileDown25Seq0, kLeftBankedQuarterTurn5TileDown25Seq1,
                            kLeftBankedQuarterTurn5TileDown25Seq2, kLeftBankedQuarterTurn5TileDown25Seq3,
                            kLeftBankedQuarterTurn5TileDown25Seq4, kLeftBankedQuarterTurn5TileDown25Seq5,
                            kLeftBankedQuarterTurn5TileDown25Seq6, kLeftBankedQuarterTurn5TileDown25Seq7 } },
    };

    constexpr auto kTEDRightBankedQuarterTurn5TileDown25 = TrackElementDescriptor{
        .coordinates = { 0, 1, 64, 0, -64, 64 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 333259,
        .mirrorElement = TrackElemType::leftBankedQuarterTurn5TileDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurve, TrackPitch::down25, TrackPitch::down25, TrackRoll::right, TrackRoll::right, 0 },
        .spinFunction = SpinFunction::r8,
        .verticalFactor = EvaluatorConst<200>,
        .lateralFactor = EvaluatorConst<-160>,
        .sequenceData = { 7,
                          { kRightBankedQuarterTurn5TileDown25Seq0, kRightBankedQuarterTurn5TileDown25Seq1,
                            kRightBankedQuarterTurn5TileDown25Seq2, kRightBankedQuarterTurn5TileDown25Seq3,
                            kRightBankedQuarterTurn5TileDown25Seq4, kRightBankedQuarterTurn5TileDown25Seq5,
                            kRightBankedQuarterTurn5TileDown25Seq6, kRightBankedQuarterTurn5TileDown25Seq7 } },
    };

    constexpr auto kTEDUp25ToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25ToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::none, 0 },
        .sequenceData = { 1, { kUp25ToLeftBankedUp25Seq0 } },
    };

    constexpr auto kTEDUp25ToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::up25ToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::right, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kUp25ToRightBankedUp25Seq0 } },
    };

    constexpr auto kTEDLeftBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::rightBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::left, 0 },
        .sequenceData = { 1, { kLeftBankedUp25ToUp25Seq0 } },
    };

    constexpr auto kTEDRightBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 16, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::leftBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kRightBankedUp25ToUp25Seq0 } },
    };

    constexpr auto kTEDDown25ToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25ToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::left, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown25ToLeftBankedDown25Seq0 } },
    };

    constexpr auto kTEDDown25ToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::down25ToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::right, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown25ToRightBankedDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::rightBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::left,
                        0 },
        .sequenceData = { 1, { kLeftBankedDown25ToDown25Seq0 } },
    };

    constexpr auto kTEDRightBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 83968,
        .mirrorElement = TrackElemType::leftBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::right,
                        0 },
        .sequenceData = { 1, { kRightBankedDown25ToDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedFlatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankedFlatToLeftBankedUp25Seq0 } },
    };

    constexpr auto kTEDRightBankedFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedFlatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankedFlatToRightBankedUp25Seq0 } },
    };

    constexpr auto kTEDLeftBankedUp25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedUp25ToRightBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankedUp25ToLeftBankedFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedUp25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedUp25ToLeftBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankedUp25ToRightBankedFlatSeq0 } },
    };

    constexpr auto kTEDLeftBankedFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedFlatToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankedFlatToLeftBankedDown25Seq0 } },
    };

    constexpr auto kTEDRightBankedFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedFlatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankedFlatToRightBankedDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedDown25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedDown25ToRightBankedFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankedDown25ToLeftBankedFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedDown25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedDown25ToLeftBankedFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::right, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankedDown25ToRightBankedFlatSeq0 } },
    };

    constexpr auto kTEDFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToLeftBankedUp25Seq0 } },
    };

    constexpr auto kTEDFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::up25, TrackPitch::none, TrackRoll::right, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kFlatToRightBankedUp25Seq0 } },
    };

    constexpr auto kTEDLeftBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kLeftBankedUp25ToFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 8, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kRightBankedUp25ToFlatSeq0 } },
    };

    constexpr auto kTEDFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::left, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::left, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToLeftBankedDown25Seq0 } },
    };

    constexpr auto kTEDFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::right, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::flatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::down25, TrackPitch::none, TrackRoll::right, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<-103>,
        .sequenceData = { 1, { kFlatToRightBankedDown25Seq0 } },
    };

    constexpr auto kTEDLeftBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::left },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rightBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kLeftBankedDown25ToFlatSeq0 } },
    };

    constexpr auto kTEDRightBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 0, 0, 8, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::right },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::leftBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeRollBanking, TrackPitch::none, TrackPitch::down25, TrackRoll::none, TrackRoll::right,
                        0 },
        .verticalFactor = EvaluatorConst<103>,
        .sequenceData = { 1, { kRightBankedDown25ToFlatSeq0 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileUp90 = TrackElementDescriptor{
        .coordinates = { 0, 3, 0, 96, 0, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileUp90,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kLeftQuarterTurn1TileUp90Seq0, kLeftQuarterTurn1TileUp90Seq1 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileUp90 = TrackElementDescriptor{
        .coordinates = { 0, 1, 0, 96, 0, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileUp90,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::up90, TrackPitch::up90, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 2, { kRightQuarterTurn1TileUp90Seq0, kRightQuarterTurn1TileUp90Seq1 } },
    };

    constexpr auto kTEDLeftQuarterTurn1TileDown90 = TrackElementDescriptor{
        .coordinates = { 0, 3, 96, 0, 0, 32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::leftSmall, TrackCurve::leftSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::rightQuarterTurn1TileDown90,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 2, { kLeftQuarterTurn1TileDown90Seq0, kLeftQuarterTurn1TileDown90Seq1 } },
    };

    constexpr auto kTEDRightQuarterTurn1TileDown90 = TrackElementDescriptor{
        .coordinates = { 0, 1, 96, 0, 0, -32 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::rightSmall, TrackCurve::rightSmall },
        .priceModifier = 151552,
        .mirrorElement = TrackElemType::leftQuarterTurn1TileDown90,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down },
        .definition = { TrackGroup::slopeCurveSteep, TrackPitch::down90, TrackPitch::down90, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 2, { kRightQuarterTurn1TileDown90Seq0, kRightQuarterTurn1TileDown90Seq1 } },
    };

    constexpr auto kTEDMultiDimUp90ToInvertedFlatQuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, 96, 64, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopUninvertedUp, TrackPitch::none, TrackPitch::up90, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorUp90QuarterLoop,
        .sequenceData = { 3,
                          { kMultiDimUp90ToInvertedFlatQuarterLoopSeq0, kMultiDimUp90ToInvertedFlatQuarterLoopSeq1,
                            kMultiDimUp90ToInvertedFlatQuarterLoopSeq2 } },
    };

    constexpr auto kTEDMultiDimFlatToDown90QuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, -128, -96, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimFlatToDown90QuarterLoop,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopUninvertedDown, TrackPitch::down90, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorDown90QuarterLoop,
        .sequenceData = { 4,
                          { kMultiDimFlatToDown90QuarterLoopSeq0, kMultiDimFlatToDown90QuarterLoopSeq1,
                            kMultiDimFlatToDown90QuarterLoopSeq2, kMultiDimFlatToDown90QuarterLoopSeq3 } },
    };

    constexpr auto kTEDMultiDimInvertedUp90ToFlatQuarterLoop = TrackElementDescriptor{
        .description = STR_QUARTER_LOOP,
        .coordinates = { 0, 2, 0, 128, 64, 0 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::multiDimInvertedUp90ToFlatQuarterLoop,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::quarterLoopInvertedUp, TrackPitch::none, TrackPitch::up90, TrackRoll::upsideDown,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorUp90QuarterLoop,
        .sequenceData = { 3,
                          { kMultiDimInvertedUp90ToFlatQuarterLoopSeq0, kMultiDimInvertedUp90ToFlatQuarterLoopSeq1,
                            kMultiDimInvertedUp90ToFlatQuarterLoopSeq2 } },
    };

    constexpr auto kTEDRotationControlToggle = TrackElementDescriptor{
        .description = STR_SPINNING_CONTROL_TOGGLE_TRACK,
        .coordinates = { 0, 0, 0, 0, 0, 0 },
        .pieceLength = 32,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 77824,
        .mirrorElement = TrackElemType::rotationControlToggle,
        .flags = {},
        .definition = { TrackGroup::rotationControlToggle, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kRotationControlToggleSeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4A = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4A,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack1x4ASeq0, kFlatTrack1x4ASeq1, kFlatTrack1x4ASeq2, kFlatTrack1x4ASeq3 } },
    };

    constexpr auto kTEDFlatTrack2x2 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_2_X_2,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack2x2,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack2x2Seq0, kFlatTrack2x2Seq1, kFlatTrack2x2Seq2, kFlatTrack2x2Seq3 } },
    };

    constexpr auto kTEDFlatTrack4x4 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_4_X_4,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 1048576, // TrackElemType::flatTrack4x4
        .mirrorElement = TrackElemType::flatTrack4x4,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 16,
                          { kFlatTrack4x4Seq0, kFlatTrack4x4Seq1, kFlatTrack4x4Seq2, kFlatTrack4x4Seq3, kFlatTrack4x4Seq4,
                            kFlatTrack4x4Seq5, kFlatTrack4x4Seq6, kFlatTrack4x4Seq7, kFlatTrack4x4Seq8, kFlatTrack4x4Seq9,
                            kFlatTrack4x4Seq10, kFlatTrack4x4Seq11, kFlatTrack4x4Seq12, kFlatTrack4x4Seq13, kFlatTrack4x4Seq14,
                            kFlatTrack4x4Seq15 } },
    };

    constexpr auto kTEDFlatTrack2x4 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_2_X_4,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::flatTrack2x4,
        .flags = { TrackElementFlag::onlyAboveGround },
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 8,
                          { kFlatTrack2x4Seq0, kFlatTrack2x4Seq1, kFlatTrack2x4Seq2, kFlatTrack2x4Seq3, kFlatTrack2x4Seq4,
                            kFlatTrack2x4Seq5, kFlatTrack2x4Seq6, kFlatTrack2x4Seq7 } },
    };

    constexpr auto kTEDFlatTrack1x5 = TrackElementDescriptor{
        .description = STR_BASE_SIZE_5_X_1,
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 327680,
        .mirrorElement = TrackElemType::flatTrack1x5,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 5,
                          { kFlatTrack1x5Seq0, kFlatTrack1x5Seq1, kFlatTrack1x5Seq2, kFlatTrack1x5Seq3, kFlatTrack1x5Seq4 } },
    };

    constexpr auto kTEDFlatTrack1x1A = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flatTrack1x1A,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatTrack1x1ASeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4B = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4B,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kFlatTrack1x4BSeq0, kFlatTrack1x4BSeq1, kFlatTrack1x4BSeq2, kFlatTrack1x4BSeq3 } },
    };

    constexpr auto kTEDFlatTrack1x1B = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 65536,
        .mirrorElement = TrackElemType::flatTrack1x1B,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 1, { kFlatTrack1x1BSeq0 } },
    };

    constexpr auto kTEDFlatTrack1x4C = TrackElementDescriptor{
        .coordinates = { 0, 2, 0, 0, 0, 32 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 262144,
        .mirrorElement = TrackElemType::flatTrack1x4C,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, -40 },
        .sequenceData = { 4, { kFlatTrack1x4CSeq0, kFlatTrack1x4CSeq1, kFlatTrack1x4CSeq2, kFlatTrack1x4CSeq3 } },
    };

    constexpr auto kTEDFlatTrack3x3 = TrackElementDescriptor{
        .coordinates = { 0, 0, 0, 96, 32, 0 },
        .pieceLength = 0,
        .curveChain = { TrackElemType::none, TrackElemType::none },
        .priceModifier = 524288,
        .mirrorElement = TrackElemType::flatTrack3x3,
        .flags = {},
        .definition = { TrackGroup::flatRideBase, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 9,
                          { kFlatTrack3x3Seq0, kFlatTrack3x3Seq1, kFlatTrack3x3Seq2, kFlatTrack3x3Seq3, kFlatTrack3x3Seq4,
                            kFlatTrack3x3Seq5, kFlatTrack3x3Seq6, kFlatTrack3x3Seq7, kFlatTrack3x3Seq8 } },
    };

    constexpr auto kTEDLeftLargeCorkscrewUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, 112, -64, -64 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightLargeCorkscrewDown, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::rightLargeCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none,
                        32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<117>,
        .sequenceData = { 6,
                          { kLeftLargeCorkscrewUpSeq0, kLeftLargeCorkscrewUpSeq1, kLeftLargeCorkscrewUpSeq2,
                            kLeftLargeCorkscrewUpSeq3, kLeftLargeCorkscrewUpSeq4, kLeftLargeCorkscrewUpSeq5 } },
    };

    constexpr auto kTEDRightLargeCorkscrewUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, 112, -64, 64 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftLargeCorkscrewDown, TrackCurve::none },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::leftLargeCorkscrewUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::upsideDown, TrackRoll::none,
                        32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<-117>,
        .sequenceData = { 6,
                          { kRightLargeCorkscrewUpSeq0, kRightLargeCorkscrewUpSeq1, kRightLargeCorkscrewUpSeq2,
                            kRightLargeCorkscrewUpSeq3, kRightLargeCorkscrewUpSeq4, kRightLargeCorkscrewUpSeq5 } },
    };

    constexpr auto kTEDLeftLargeCorkscrewDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_LEFT,
        .coordinates = { 0, 3, 0, -112, -64, -64 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightLargeCorkscrewUp },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::rightLargeCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<-117>,
        .sequenceData = { 6,
                          { kLeftLargeCorkscrewDownSeq0, kLeftLargeCorkscrewDownSeq1, kLeftLargeCorkscrewDownSeq2,
                            kLeftLargeCorkscrewDownSeq3, kLeftLargeCorkscrewDownSeq4, kLeftLargeCorkscrewDownSeq5 } },
    };

    constexpr auto kTEDRightLargeCorkscrewDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_CORKSCREW_RIGHT,
        .coordinates = { 0, 1, 0, -112, -64, 64 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftLargeCorkscrewUp },
        .priceModifier = 360448,
        .mirrorElement = TrackElemType::leftLargeCorkscrewDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::corkscrewLarge, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -32 },
        .verticalFactor = EvaluatorConst<89>,
        .lateralFactor = EvaluatorConst<117>,
        .sequenceData = { 6,
                          { kRightLargeCorkscrewDownSeq0, kRightLargeCorkscrewDownSeq1, kRightLargeCorkscrewDownSeq2,
                            kRightLargeCorkscrewDownSeq3, kRightLargeCorkscrewDownSeq4, kRightLargeCorkscrewDownSeq5 } },
    };

    constexpr auto kTEDLeftMediumHalfLoopUp = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 216, -32, -32 },
        .pieceLength = 80,
        .curveChain = { TrackElemType::rightMediumHalfLoopDown, TrackCurve::none },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::rightMediumHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorMediumHalfLoopUp,
        .sequenceData = { 5,
                          { kLeftMediumHalfLoopUpSeq0, kLeftMediumHalfLoopUpSeq1, kLeftMediumHalfLoopUpSeq2,
                            kLeftMediumHalfLoopUpSeq3, kLeftMediumHalfLoopUpSeq4 } },
    };

    constexpr auto kTEDRightMediumHalfLoopUp = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 216, -32, 32 },
        .pieceLength = 80,
        .curveChain = { TrackElemType::leftMediumHalfLoopDown, TrackCurve::none },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::leftMediumHalfLoopUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorMediumHalfLoopUp,
        .sequenceData = { 5,
                          { kRightMediumHalfLoopUpSeq0, kRightMediumHalfLoopUpSeq1, kRightMediumHalfLoopUpSeq2,
                            kRightMediumHalfLoopUpSeq3, kRightMediumHalfLoopUpSeq4 } },
    };

    constexpr auto kTEDLeftMediumHalfLoopDown = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -216, 32, -32 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackElemType::rightMediumHalfLoopUp },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::rightMediumHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::down25, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorMediumHalfLoopDown,
        .sequenceData = { 5,
                          { kLeftMediumHalfLoopDownSeq0, kLeftMediumHalfLoopDownSeq1, kLeftMediumHalfLoopDownSeq2,
                            kLeftMediumHalfLoopDownSeq3, kLeftMediumHalfLoopDownSeq4 } },
    };

    constexpr auto kTEDRightMediumHalfLoopDown = TrackElementDescriptor{
        .description = STR_MEDIUM_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -216, 32, 32 },
        .pieceLength = 80,
        .curveChain = { TrackCurve::none, TrackElemType::leftMediumHalfLoopUp },
        .priceModifier = 589824,
        .mirrorElement = TrackElemType::leftMediumHalfLoopDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::halfLoopMedium, TrackPitch::down25, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorMediumHalfLoopDown,
        .sequenceData = { 5,
                          { kRightMediumHalfLoopDownSeq0, kRightMediumHalfLoopDownSeq1, kRightMediumHalfLoopDownSeq2,
                            kRightMediumHalfLoopDownSeq3, kRightMediumHalfLoopDownSeq4 } },
    };

    constexpr auto kTEDLeftZeroGRollUp = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::leftZeroGRollDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollUpLeft,
        .sequenceData = { 3, { kLeftZeroGRollUpSeq0, kLeftZeroGRollUpSeq1, kLeftZeroGRollUpSeq2 } },
    };

    constexpr auto kTEDRightZeroGRollUp = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackElemType::rightZeroGRollDown, TrackCurve::none },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown, TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollUpRight,
        .sequenceData = { 3, { kRightZeroGRollUpSeq0, kRightZeroGRollUpSeq1, kRightZeroGRollUpSeq2 } },
    };

    constexpr auto kTEDLeftZeroGRollDown = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, -56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::leftZeroGRollUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::rightZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollDownLeft,
        .sequenceData = { 3, { kLeftZeroGRollDownSeq0, kLeftZeroGRollDownSeq1, kLeftZeroGRollDownSeq2 } },
    };

    constexpr auto kTEDRightZeroGRollDown = TrackElementDescriptor{
        .description = STR_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, -56, -64, 0 },
        .pieceLength = 96,
        .curveChain = { TrackCurve::none, TrackElemType::rightZeroGRollUp },
        .priceModifier = 229376,
        .mirrorElement = TrackElemType::leftZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRoll, TrackPitch::down25, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        0 },
        .verticalFactor = EvaluatorConst<250>,
        .lateralFactor = EvaluatorZeroGRollDownRight,
        .sequenceData = { 3, { kRightZeroGRollDownSeq0, kRightZeroGRollDownSeq1, kRightZeroGRollDownSeq2 } },
    };

    constexpr auto kTEDLeftLargeZeroGRollUp = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, 152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::leftLargeZeroGRollDown, TrackCurve::none },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::rightLargeZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeZeroGRollUp,
        .lateralFactor = EvaluatorLargeZeroGRollUpLeft,
        .sequenceData = { 4,
                          { kLeftLargeZeroGRollUpSeq0, kLeftLargeZeroGRollUpSeq1, kLeftLargeZeroGRollUpSeq2,
                            kLeftLargeZeroGRollUpSeq3 } },
    };

    constexpr auto kTEDRightLargeZeroGRollUp = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, 152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::rightLargeZeroGRollDown, TrackCurve::none },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::leftLargeZeroGRollUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none,
                        64 },
        .verticalFactor = EvaluatorLargeZeroGRollUp,
        .lateralFactor = EvaluatorLargeZeroGRollUpRight,
        .sequenceData = { 4,
                          { kRightLargeZeroGRollUpSeq0, kRightLargeZeroGRollUpSeq1, kRightLargeZeroGRollUpSeq2,
                            kRightLargeZeroGRollUpSeq3 } },
    };

    constexpr auto kTEDLeftLargeZeroGRollDown = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_LEFT,
        .coordinates = { 0, 0, 0, -152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::leftLargeZeroGRollUp },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::rightLargeZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::down60, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorLargeZeroGRollDown,
        .lateralFactor = EvaluatorLargeZeroGRollDownLeft,
        .sequenceData = { 4,
                          { kLeftLargeZeroGRollDownSeq0, kLeftLargeZeroGRollDownSeq1, kLeftLargeZeroGRollDownSeq2,
                            kLeftLargeZeroGRollDownSeq3 } },
    };

    constexpr auto kTEDRightLargeZeroGRollDown = TrackElementDescriptor{
        .description = STR_LARGE_ZERO_G_ROLL_RIGHT,
        .coordinates = { 0, 0, 0, -152, -96, 0 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::rightLargeZeroGRollUp },
        .priceModifier = 425984,
        .mirrorElement = TrackElemType::leftLargeZeroGRollDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::zeroGRollLarge, TrackPitch::down60, TrackPitch::none, TrackRoll::none,
                        TrackRoll::upsideDown, -64 },
        .verticalFactor = EvaluatorLargeZeroGRollDown,
        .lateralFactor = EvaluatorLargeZeroGRollDownRight,
        .sequenceData = { 4,
                          { kRightLargeZeroGRollDownSeq0, kRightLargeZeroGRollDownSeq1, kRightLargeZeroGRollDownSeq2,
                            kRightLargeZeroGRollDownSeq3 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopUninvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 248, -64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::rightFlyerLargeHalfLoopInvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopUninvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopUninvertedUpSeq0, kLeftFlyerLargeHalfLoopUninvertedUpSeq1,
                            kLeftFlyerLargeHalfLoopUninvertedUpSeq2, kLeftFlyerLargeHalfLoopUninvertedUpSeq3,
                            kLeftFlyerLargeHalfLoopUninvertedUpSeq4, kLeftFlyerLargeHalfLoopUninvertedUpSeq5,
                            kLeftFlyerLargeHalfLoopUninvertedUpSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopUninvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 248, -64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::leftFlyerLargeHalfLoopInvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopUninvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::normalToInversion, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopUninvertedUpSeq0, kRightFlyerLargeHalfLoopUninvertedUpSeq1,
                            kRightFlyerLargeHalfLoopUninvertedUpSeq2, kRightFlyerLargeHalfLoopUninvertedUpSeq3,
                            kRightFlyerLargeHalfLoopUninvertedUpSeq4, kRightFlyerLargeHalfLoopUninvertedUpSeq5,
                            kRightFlyerLargeHalfLoopUninvertedUpSeq6 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopInvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -248, 64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::rightFlyerLargeHalfLoopUninvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopInvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopInvertedDownSeq0, kLeftFlyerLargeHalfLoopInvertedDownSeq1,
                            kLeftFlyerLargeHalfLoopInvertedDownSeq2, kLeftFlyerLargeHalfLoopInvertedDownSeq3,
                            kLeftFlyerLargeHalfLoopInvertedDownSeq4, kLeftFlyerLargeHalfLoopInvertedDownSeq5,
                            kLeftFlyerLargeHalfLoopInvertedDownSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopInvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -248, 64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::leftFlyerLargeHalfLoopUninvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopInvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopInvertedDownSeq0, kRightFlyerLargeHalfLoopInvertedDownSeq1,
                            kRightFlyerLargeHalfLoopInvertedDownSeq2, kRightFlyerLargeHalfLoopInvertedDownSeq3,
                            kRightFlyerLargeHalfLoopInvertedDownSeq4, kRightFlyerLargeHalfLoopInvertedDownSeq5,
                            kRightFlyerLargeHalfLoopInvertedDownSeq6 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopInvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, 280, -64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::rightFlyerLargeHalfLoopUninvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopInvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopInvertedUpSeq0, kLeftFlyerLargeHalfLoopInvertedUpSeq1,
                            kLeftFlyerLargeHalfLoopInvertedUpSeq2, kLeftFlyerLargeHalfLoopInvertedUpSeq3,
                            kLeftFlyerLargeHalfLoopInvertedUpSeq4, kLeftFlyerLargeHalfLoopInvertedUpSeq5,
                            kLeftFlyerLargeHalfLoopInvertedUpSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopInvertedUp = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, 280, -64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackElemType::leftFlyerLargeHalfLoopUninvertedDown, TrackCurve::none },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopInvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingLargeHalfLoopInvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorLargeHalfLoopUp,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopInvertedUpSeq0, kRightFlyerLargeHalfLoopInvertedUpSeq1,
                            kRightFlyerLargeHalfLoopInvertedUpSeq2, kRightFlyerLargeHalfLoopInvertedUpSeq3,
                            kRightFlyerLargeHalfLoopInvertedUpSeq4, kRightFlyerLargeHalfLoopInvertedUpSeq5,
                            kRightFlyerLargeHalfLoopInvertedUpSeq6 } },
    };

    constexpr auto kTEDLeftFlyerLargeHalfLoopUninvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_LEFT,
        .coordinates = { 0, 2, 0, -280, 64, -32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::rightFlyerLargeHalfLoopInvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::rightFlyerLargeHalfLoopUninvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kLeftFlyerLargeHalfLoopUninvertedDownSeq0, kLeftFlyerLargeHalfLoopUninvertedDownSeq1,
                            kLeftFlyerLargeHalfLoopUninvertedDownSeq2, kLeftFlyerLargeHalfLoopUninvertedDownSeq3,
                            kLeftFlyerLargeHalfLoopUninvertedDownSeq4, kLeftFlyerLargeHalfLoopUninvertedDownSeq5,
                            kLeftFlyerLargeHalfLoopUninvertedDownSeq6 } },
    };

    constexpr auto kTEDRightFlyerLargeHalfLoopUninvertedDown = TrackElementDescriptor{
        .description = STR_LARGE_HALF_LOOP_RIGHT,
        .coordinates = { 0, 2, 0, -280, 64, 32 },
        .pieceLength = 100,
        .curveChain = { TrackCurve::none, TrackElemType::leftFlyerLargeHalfLoopInvertedUp },
        .priceModifier = 884736,
        .mirrorElement = TrackElemType::leftFlyerLargeHalfLoopUninvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        .definition = { TrackGroup::flyingLargeHalfLoopUninvertedDown, TrackPitch::down25, TrackPitch::none,
                        TrackRoll::upsideDown, TrackRoll::none, -64 },
        .verticalFactor = EvaluatorLargeHalfLoopDown,
        .sequenceData = { 7,
                          { kRightFlyerLargeHalfLoopUninvertedDownSeq0, kRightFlyerLargeHalfLoopUninvertedDownSeq1,
                            kRightFlyerLargeHalfLoopUninvertedDownSeq2, kRightFlyerLargeHalfLoopUninvertedDownSeq3,
                            kRightFlyerLargeHalfLoopUninvertedDownSeq4, kRightFlyerLargeHalfLoopUninvertedDownSeq5,
                            kRightFlyerLargeHalfLoopUninvertedDownSeq6 } },
    };

    constexpr auto kTEDFlyerHalfLoopInvertedUp = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, 152, -32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopInvertedUp,
        .flags = { TrackElementFlag::up, TrackElementFlag::inversionToNormal, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::flyingHalfLoopInvertedUp, TrackPitch::none, TrackPitch::up25, TrackRoll::upsideDown,
                        TrackRoll::none, 64 },
        .verticalFactor = EvaluatorHalfLoopUp,
        .sequenceData = { 4,
                          { kFlyerHalfLoopInvertedUpSeq0, kFlyerHalfLoopInvertedUpSeq1, kFlyerHalfLoopInvertedUpSeq2,
                            kFlyerHalfLoopInvertedUpSeq3 } },
    };

    constexpr auto kTEDFlyerHalfLoopUninvertedDown = TrackElementDescriptor{
        .description = STR_HALF_LOOP,
        .coordinates = { 0, 2, 0, -152, 32, 0 },
        .pieceLength = 64,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 294912,
        .mirrorElement = TrackElemType::flyerHalfLoopUninvertedDown,
        .flags = { TrackElementFlag::down, TrackElementFlag::normalToInversion },
        .definition = { TrackGroup::flyingHalfLoopUninvertedDown, TrackPitch::down25, TrackPitch::none, TrackRoll::upsideDown,
                        TrackRoll::none, -64 },
        .verticalFactor = EvaluatorHalfLoopDown,
        .sequenceData = { 4,
                          { kFlyerHalfLoopUninvertedDownSeq0, kFlyerHalfLoopUninvertedDownSeq1,
                            kFlyerHalfLoopUninvertedDownSeq2, kFlyerHalfLoopUninvertedDownSeq3 } },
    };

    constexpr auto kTEDLeftEighthToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToDiagUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToDiagUp25Seq0, kLeftEighthToDiagUp25Seq1, kLeftEighthToDiagUp25Seq2,
                            kLeftEighthToDiagUp25Seq3, kLeftEighthToDiagUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToDiagUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToDiagUp25Seq0, kRightEighthToDiagUp25Seq1, kRightEighthToDiagUp25Seq2,
                            kRightEighthToDiagUp25Seq3, kRightEighthToDiagUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, -48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToDiagDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToDiagDown25Seq0, kLeftEighthToDiagDown25Seq1, kLeftEighthToDiagDown25Seq2,
                            kLeftEighthToDiagDown25Seq3, kLeftEighthToDiagDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToDiagDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToDiagDown25Seq0, kRightEighthToDiagDown25Seq1, kRightEighthToDiagDown25Seq2,
                            kRightEighthToDiagDown25Seq3, kRightEighthToDiagDown25Seq4 } },
    };

    constexpr auto kTEDLeftEighthToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToOrthogonalUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToOrthogonalUp25Seq0, kLeftEighthToOrthogonalUp25Seq1, kLeftEighthToOrthogonalUp25Seq2,
                            kLeftEighthToOrthogonalUp25Seq3, kLeftEighthToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToOrthogonalUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::none, 16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToOrthogonalUp25Seq0, kRightEighthToOrthogonalUp25Seq1,
                            kRightEighthToOrthogonalUp25Seq2, kRightEighthToOrthogonalUp25Seq3,
                            kRightEighthToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::rightEighthToOrthogonalDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::l9,
        .lateralFactor = EvaluatorConst<137>,
        .sequenceData = { 5,
                          { kLeftEighthToOrthogonalDown25Seq0, kLeftEighthToOrthogonalDown25Seq1,
                            kLeftEighthToOrthogonalDown25Seq2, kLeftEighthToOrthogonalDown25Seq3,
                            kLeftEighthToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, -48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 219559,
        .mirrorElement = TrackElemType::leftEighthToOrthogonalDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLarge, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        -16 },
        .spinFunction = SpinFunction::r9,
        .lateralFactor = EvaluatorConst<-137>,
        .sequenceData = { 5,
                          { kRightEighthToOrthogonalDown25Seq0, kRightEighthToOrthogonalDown25Seq1,
                            kRightEighthToOrthogonalDown25Seq2, kRightEighthToOrthogonalDown25Seq3,
                            kRightEighthToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDDiagUp25ToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25ToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::none,
                        0 },
        .sequenceData = { 4,
                          { kDiagUp25ToLeftBankedUp25Seq0, kDiagUp25ToLeftBankedUp25Seq1, kDiagUp25ToLeftBankedUp25Seq2,
                            kDiagUp25ToLeftBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagUp25ToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25ToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagUp25ToRightBankedUp25Seq0, kDiagUp25ToRightBankedUp25Seq1, kDiagUp25ToRightBankedUp25Seq2,
                            kDiagUp25ToRightBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagRightBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::none, TrackRoll::left,
                        0 },
        .sequenceData = { 4,
                          { kDiagLeftBankedUp25ToUp25Seq0, kDiagLeftBankedUp25ToUp25Seq1, kDiagLeftBankedUp25ToUp25Seq2,
                            kDiagLeftBankedUp25ToUp25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedUp25ToUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagLeftBankedUp25ToUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagRightBankedUp25ToUp25Seq0, kDiagRightBankedUp25ToUp25Seq1, kDiagRightBankedUp25ToUp25Seq2,
                            kDiagRightBankedUp25ToUp25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25ToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagDown25ToLeftBankedDown25Seq0, kDiagDown25ToLeftBankedDown25Seq1,
                            kDiagDown25ToLeftBankedDown25Seq2, kDiagDown25ToLeftBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagDown25ToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25ToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::none, 0 },
        .sequenceData = { 4,
                          { kDiagDown25ToRightBankedDown25Seq0, kDiagDown25ToRightBankedDown25Seq1,
                            kDiagDown25ToRightBankedDown25Seq2, kDiagDown25ToRightBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagRightBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::left, 0 },
        .sequenceData = { 4,
                          { kDiagLeftBankedDown25ToDown25Seq0, kDiagLeftBankedDown25ToDown25Seq1,
                            kDiagLeftBankedDown25ToDown25Seq2, kDiagLeftBankedDown25ToDown25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedDown25ToDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagLeftBankedDown25ToDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagRightBankedDown25ToDown25Seq0, kDiagRightBankedDown25ToDown25Seq1,
                            kDiagRightBankedDown25ToDown25Seq2, kDiagRightBankedDown25ToDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedFlatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedFlatToLeftBankedUp25Seq0, kDiagLeftBankedFlatToLeftBankedUp25Seq1,
                            kDiagLeftBankedFlatToLeftBankedUp25Seq2, kDiagLeftBankedFlatToLeftBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedFlatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankedFlatToRightBankedUp25Seq0, kDiagRightBankedFlatToRightBankedUp25Seq1,
                            kDiagRightBankedFlatToRightBankedUp25Seq2, kDiagRightBankedFlatToRightBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedUp25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedUp25ToRightBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedUp25ToLeftBankedFlatSeq0, kDiagLeftBankedUp25ToLeftBankedFlatSeq1,
                            kDiagLeftBankedUp25ToLeftBankedFlatSeq2, kDiagLeftBankedUp25ToLeftBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedUp25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedUp25ToLeftBankedFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankedUp25ToRightBankedFlatSeq0, kDiagRightBankedUp25ToRightBankedFlatSeq1,
                            kDiagRightBankedUp25ToRightBankedFlatSeq2, kDiagRightBankedUp25ToRightBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagLeftBankedFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::
            diagRightBankedFlatToRightBankedDown25, // TrackElemType::diagRightBankedFlatToRightBankedDown25
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::left,
                        TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedFlatToLeftBankedDown25Seq0, kDiagLeftBankedFlatToLeftBankedDown25Seq1,
                            kDiagLeftBankedFlatToLeftBankedDown25Seq2, kDiagLeftBankedFlatToLeftBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagRightBankedFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedFlatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankedFlatToRightBankedDown25Seq0, kDiagRightBankedFlatToRightBankedDown25Seq1,
                            kDiagRightBankedFlatToRightBankedDown25Seq2, kDiagRightBankedFlatToRightBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedDown25ToLeftBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::
            diagRightBankedDown25ToRightBankedFlat, // TrackElemType::diagRightBankedDown25ToRightBankedFlat
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedDown25ToLeftBankedFlatSeq0, kDiagLeftBankedDown25ToLeftBankedFlatSeq1,
                            kDiagLeftBankedDown25ToLeftBankedFlatSeq2, kDiagLeftBankedDown25ToLeftBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedDown25ToRightBankedFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedDown25ToLeftBankedFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankedDown25ToRightBankedFlatSeq0, kDiagRightBankedDown25ToRightBankedFlatSeq1,
                            kDiagRightBankedDown25ToRightBankedFlatSeq2, kDiagRightBankedDown25ToRightBankedFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToLeftBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToRightBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::left, TrackRoll::none,
                        0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagFlatToLeftBankedUp25Seq0, kDiagFlatToLeftBankedUp25Seq1, kDiagFlatToLeftBankedUp25Seq2,
                            kDiagFlatToLeftBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagFlatToRightBankedUp25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToLeftBankedUp25,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagFlatToRightBankedUp25Seq0, kDiagFlatToRightBankedUp25Seq1, kDiagFlatToRightBankedUp25Seq2,
                            kDiagFlatToRightBankedUp25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::none, TrackRoll::left,
                        0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedUp25ToFlatSeq0, kDiagLeftBankedUp25ToFlatSeq1, kDiagLeftBankedUp25ToFlatSeq2,
                            kDiagLeftBankedUp25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedUp25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 8, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedUp25ToFlat,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::up25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagRightBankedUp25ToFlatSeq0, kDiagRightBankedUp25ToFlatSeq1, kDiagRightBankedUp25ToFlatSeq2,
                            kDiagRightBankedUp25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToLeftBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToRightBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::left,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagFlatToLeftBankedDown25Seq0, kDiagFlatToLeftBankedDown25Seq1, kDiagFlatToLeftBankedDown25Seq2,
                            kDiagFlatToLeftBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagFlatToRightBankedDown25 = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagFlatToLeftBankedDown25,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::none, TrackRoll::right,
                        TrackRoll::none, 0 },
        .verticalFactor = EvaluatorConst<-113>,
        .sequenceData = { 4,
                          { kDiagFlatToRightBankedDown25Seq0, kDiagFlatToRightBankedDown25Seq1,
                            kDiagFlatToRightBankedDown25Seq2, kDiagFlatToRightBankedDown25Seq3 } },
    };

    constexpr auto kTEDDiagLeftBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagRightBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::left, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagLeftBankedDown25ToFlatSeq0, kDiagLeftBankedDown25ToFlatSeq1, kDiagLeftBankedDown25ToFlatSeq2,
                            kDiagLeftBankedDown25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagRightBankedDown25ToFlat = TrackElementDescriptor{
        .coordinates = { 4, 4, 8, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 108800,
        .mirrorElement = TrackElemType::diagLeftBankedDown25ToFlat,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::none, TrackPitch::down25, TrackRoll::none,
                        TrackRoll::right, 0 },
        .verticalFactor = EvaluatorConst<113>,
        .sequenceData = { 4,
                          { kDiagRightBankedDown25ToFlatSeq0, kDiagRightBankedDown25ToFlatSeq1,
                            kDiagRightBankedDown25ToFlatSeq2, kDiagRightBankedDown25ToFlatSeq3 } },
    };

    constexpr auto kTEDDiagUp25LeftBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25RightBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        0 },
        .sequenceData = { 4,
                          { kDiagUp25LeftBankedSeq0, kDiagUp25LeftBankedSeq1, kDiagUp25LeftBankedSeq2,
                            kDiagUp25LeftBankedSeq3 } },
    };

    constexpr auto kTEDDiagUp25RightBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 16, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagUp25LeftBanked,
        .flags = { TrackElementFlag::up, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagUp25RightBankedSeq0, kDiagUp25RightBankedSeq1, kDiagUp25RightBankedSeq2,
                            kDiagUp25RightBankedSeq3 } },
    };

    constexpr auto kTEDDiagDown25LeftBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25RightBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, 0 },
        .sequenceData = { 4,
                          { kDiagDown25LeftBankedSeq0, kDiagDown25LeftBankedSeq1, kDiagDown25LeftBankedSeq2,
                            kDiagDown25LeftBankedSeq3 } },
    };

    constexpr auto kTEDDiagDown25RightBanked = TrackElementDescriptor{
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 118809,
        .mirrorElement = TrackElemType::diagDown25LeftBanked,
        .flags = { TrackElementFlag::down, TrackElementFlag::banked, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, 0 },
        .sequenceData = { 4,
                          { kDiagDown25RightBankedSeq0, kDiagDown25RightBankedSeq1, kDiagDown25RightBankedSeq2,
                            kDiagDown25RightBankedSeq3 } },
    };

    constexpr auto kTEDLeftEighthBankToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, 48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToDiagUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToDiagUp25Seq0, kLeftEighthBankToDiagUp25Seq1, kLeftEighthBankToDiagUp25Seq2,
                            kLeftEighthBankToDiagUp25Seq3, kLeftEighthBankToDiagUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToDiagUp25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToDiagUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToDiagUp25Seq0, kRightEighthBankToDiagUp25Seq1, kRightEighthBankToDiagUp25Seq2,
                            kRightEighthBankToDiagUp25Seq3, kRightEighthBankToDiagUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthBankToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 7, 0, -48, -64, -32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToDiagDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, -16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToDiagDown25Seq0, kLeftEighthBankToDiagDown25Seq1, kLeftEighthBankToDiagDown25Seq2,
                            kLeftEighthBankToDiagDown25Seq3, kLeftEighthBankToDiagDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToDiagDown25 = TrackElementDescriptor{
        .coordinates = { 0, 4, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToDiagDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, -16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToDiagDown25Seq0, kRightEighthBankToDiagDown25Seq1,
                            kRightEighthBankToDiagDown25Seq2, kRightEighthBankToDiagDown25Seq3,
                            kRightEighthBankToDiagDown25Seq4 } },
    };

    constexpr auto kTEDLeftEighthBankToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, 48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToOrthogonalUp25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::left, TrackRoll::left,
                        16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToOrthogonalUp25Seq0, kLeftEighthBankToOrthogonalUp25Seq1,
                            kLeftEighthBankToOrthogonalUp25Seq2, kLeftEighthBankToOrthogonalUp25Seq3,
                            kLeftEighthBankToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToOrthogonalUp25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, 48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToOrthogonalUp25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::up,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::up25, TrackPitch::up25, TrackRoll::right,
                        TrackRoll::right, 16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToOrthogonalUp25Seq0, kRightEighthBankToOrthogonalUp25Seq1,
                            kRightEighthBankToOrthogonalUp25Seq2, kRightEighthBankToOrthogonalUp25Seq3,
                            kRightEighthBankToOrthogonalUp25Seq4 } },
    };

    constexpr auto kTEDLeftEighthBankToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 0, 0, -48, -64, 32 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::leftLarge, TrackCurve::leftLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::rightEighthBankToOrthogonalDown25,
        .flags = { TrackElementFlag::turnLeft, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::left,
                        TrackRoll::left, -16 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<200>,
        .sequenceData = { 5,
                          { kLeftEighthBankToOrthogonalDown25Seq0, kLeftEighthBankToOrthogonalDown25Seq1,
                            kLeftEighthBankToOrthogonalDown25Seq2, kLeftEighthBankToOrthogonalDown25Seq3,
                            kLeftEighthBankToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDRightEighthBankToOrthogonalDown25 = TrackElementDescriptor{
        .coordinates = { 4, 1, 0, -48, -32, 64 },
        .pieceLength = 92,
        .curveChain = { TrackCurve::rightLarge, TrackCurve::rightLarge },
        .priceModifier = 233281,
        .mirrorElement = TrackElemType::leftEighthBankToOrthogonalDown25,
        .flags = { TrackElementFlag::turnRight, TrackElementFlag::turnSloped, TrackElementFlag::down,
                   TrackElementFlag::startsAtHalfHeight, TrackElementFlag::allowLiftHill, TrackElementFlag::curveAllowsLift },
        .definition = { TrackGroup::slopeCurveLargeBanked, TrackPitch::down25, TrackPitch::down25, TrackRoll::right,
                        TrackRoll::right, -16 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorConst<270>,
        .lateralFactor = EvaluatorConst<-200>,
        .sequenceData = { 5,
                          { kRightEighthBankToOrthogonalDown25Seq0, kRightEighthBankToOrthogonalDown25Seq1,
                            kRightEighthBankToOrthogonalDown25Seq2, kRightEighthBankToOrthogonalDown25Seq3,
                            kRightEighthBankToOrthogonalDown25Seq4 } },
    };

    constexpr auto kTEDDiagBrakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackElemType::diagBrakes, TrackElemType::diagBrakes },
        .priceModifier = 123456,
        .mirrorElement = TrackElemType::diagBrakes,
        .flags = {},
        .definition = { TrackGroup::diagBrakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagBrakesSeq0, kDiagBrakesSeq1, kDiagBrakesSeq2, kDiagBrakesSeq3 } },
    };

    constexpr auto kTEDDiagBlockBrakes = TrackElementDescriptor{
        .description = STR_BLOCK_BRAKES,
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 123456,
        .mirrorElement = TrackElemType::diagBlockBrakes,
        .flags = {},
        .definition = { TrackGroup::diagBlockBrakes, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagBlockBrakesSeq0, kDiagBlockBrakesSeq1, kDiagBlockBrakesSeq2, kDiagBlockBrakesSeq3 } },
    };

    constexpr auto kTEDDown25Brakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 0, 0, 16, 0, 0, 0 },
        .pieceLength = 33,
        .curveChain = { TrackElemType::down25Brakes, TrackElemType::down25Brakes },
        .priceModifier = 109824,
        .mirrorElement = TrackElemType::down25Brakes,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::inclinedBrakes, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 1, { kDown25Seq0 } },
    };

    constexpr auto kTEDDiagBooster = TrackElementDescriptor{
        .description = STR_BOOSTER,
        .coordinates = { 4, 4, 0, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackElemType::diagBooster, TrackElemType::diagBooster },
        .priceModifier = 123456,
        .mirrorElement = TrackElemType::diagBooster,
        .flags = {},
        .definition = { TrackGroup::diagBooster, TrackPitch::none, TrackPitch::none, TrackRoll::none, TrackRoll::none, 0 },
        .sequenceData = { 4, { kDiagFlatSeq0, kDiagFlatSeq1, kDiagFlatSeq2, kDiagFlatSeq3 } },
    };

    constexpr auto kTEDDiagFlatToUp60LongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 88, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagFlatToUp60LongBase,
        .flags = { TrackElementFlag::up, TrackElementFlag::isSteepUp },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::up60, TrackPitch::none, TrackRoll::none, TrackRoll::none, 16 },
        .verticalFactor = EvaluatorConst<180>,
        .sequenceData = { 10,
                          { kDiagFlatToUp60LongBaseSeq0, kDiagFlatToUp60LongBaseSeq1, kDiagFlatToUp60LongBaseSeq2,
                            kDiagFlatToUp60LongBaseSeq3, kDiagFlatToUp60LongBaseSeq4, kDiagFlatToUp60LongBaseSeq5,
                            kDiagFlatToUp60LongBaseSeq6, kDiagFlatToUp60LongBaseSeq7, kDiagFlatToUp60LongBaseSeq8,
                            kDiagFlatToUp60LongBaseSeq9 } },
    };

    constexpr auto kTEDDiagUp60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 0, 88, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagUp60ToFlatLongBase,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::isSteepUp },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::up60, TrackRoll::none, TrackRoll::none, 48 },
        .verticalFactor = EvaluatorConst<-180>,
        .sequenceData = { 10,
                          { kDiagUp60ToFlatLongBaseSeq0, kDiagUp60ToFlatLongBaseSeq1, kDiagUp60ToFlatLongBaseSeq2,
                            kDiagUp60ToFlatLongBaseSeq3, kDiagUp60ToFlatLongBaseSeq4, kDiagUp60ToFlatLongBaseSeq5,
                            kDiagUp60ToFlatLongBaseSeq6, kDiagUp60ToFlatLongBaseSeq7, kDiagUp60ToFlatLongBaseSeq8,
                            kDiagUp60ToFlatLongBaseSeq9 } },
    };

    constexpr auto kTEDDiagFlatToDown60LongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 88, 0, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagFlatToDown60LongBase,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::none,
                        -32 },
        .verticalFactor = EvaluatorConst<-180>,
        .sequenceData = { 10,
                          { kDiagFlatToDown60LongBaseSeq0, kDiagFlatToDown60LongBaseSeq1, kDiagFlatToDown60LongBaseSeq2,
                            kDiagFlatToDown60LongBaseSeq3, kDiagFlatToDown60LongBaseSeq4, kDiagFlatToDown60LongBaseSeq5,
                            kDiagFlatToDown60LongBaseSeq6, kDiagFlatToDown60LongBaseSeq7, kDiagFlatToDown60LongBaseSeq8,
                            kDiagFlatToDown60LongBaseSeq9 } },
    };

    constexpr auto kTEDDiagDown60ToFlatLongBase = TrackElementDescriptor{
        .coordinates = { 4, 4, 88, 0, -96, 96 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackCurve::none },
        .priceModifier = 399690,
        .mirrorElement = TrackElemType::diagDown60ToFlatLongBase,
        .flags = { TrackElementFlag::down },
        .definition = { TrackGroup::slopeSteepLong, TrackPitch::none, TrackPitch::down60, TrackRoll::none, TrackRoll::none,
                        -24 },
        .verticalFactor = EvaluatorConst<180>,
        .sequenceData = { 10,
                          { kDiagDown60ToFlatLongBaseSeq0, kDiagDown60ToFlatLongBaseSeq1, kDiagDown60ToFlatLongBaseSeq2,
                            kDiagDown60ToFlatLongBaseSeq3, kDiagDown60ToFlatLongBaseSeq4, kDiagDown60ToFlatLongBaseSeq5,
                            kDiagDown60ToFlatLongBaseSeq6, kDiagDown60ToFlatLongBaseSeq7, kDiagDown60ToFlatLongBaseSeq8,
                            kDiagDown60ToFlatLongBaseSeq9 } },
    };

    constexpr auto kTEDLeftEighthDiveLoopUpToOrthogonal = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_LEFT,
        .coordinates = { 4, 0, 0, 152, -96, 32 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::rightEighthDiveLoopDownToDiag, TrackCurve::none },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::rightEighthDiveLoopUpToOrthogonal,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::normalToInversion,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none, 56 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorDiveLoopUp,
        .lateralFactor = EvaluatorDiveLoopUpLeft,
        .sequenceData = { 6,
                          { kLeftEighthDiveLoopUpToOrthogonalSeq0, kLeftEighthDiveLoopUpToOrthogonalSeq1,
                            kLeftEighthDiveLoopUpToOrthogonalSeq2, kLeftEighthDiveLoopUpToOrthogonalSeq3,
                            kLeftEighthDiveLoopUpToOrthogonalSeq4, kLeftEighthDiveLoopUpToOrthogonalSeq5 } },
    };

    constexpr auto kTEDRightEighthDiveLoopUpToOrthogonal = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_RIGHT,
        .coordinates = { 4, 1, 0, 152, -32, 96 },
        .pieceLength = 165,
        .curveChain = { TrackElemType::leftEighthDiveLoopDownToDiag, TrackCurve::none },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::leftEighthDiveLoopUpToOrthogonal,
        .flags = { TrackElementFlag::up, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::normalToInversion,
                   TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::none, TrackPitch::up60, TrackRoll::upsideDown, TrackRoll::none, 56 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorDiveLoopUp,
        .lateralFactor = EvaluatorDiveLoopUpRight,
        .sequenceData = { 6,
                          { kRightEighthDiveLoopUpToOrthogonalSeq0, kRightEighthDiveLoopUpToOrthogonalSeq1,
                            kRightEighthDiveLoopUpToOrthogonalSeq2, kRightEighthDiveLoopUpToOrthogonalSeq3,
                            kRightEighthDiveLoopUpToOrthogonalSeq4, kRightEighthDiveLoopUpToOrthogonalSeq5 } },
    };

    constexpr auto kTEDLeftEighthDiveLoopDownToDiag = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_LEFT,
        .coordinates = { 0, 7, 152, 0, -96, -32 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::rightEighthDiveLoopUpToOrthogonal },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::rightEighthDiveLoopDownToDiag,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -56 },
        .spinFunction = SpinFunction::l9,
        .verticalFactor = EvaluatorDiveLoopDown,
        .lateralFactor = EvaluatorDiveLoopDownLeft,
        .sequenceData = { 6,
                          { kLeftEighthDiveLoopDownToDiagSeq0, kLeftEighthDiveLoopDownToDiagSeq1,
                            kLeftEighthDiveLoopDownToDiagSeq2, kLeftEighthDiveLoopDownToDiagSeq3,
                            kLeftEighthDiveLoopDownToDiagSeq4, kLeftEighthDiveLoopDownToDiagSeq5 } },
    };

    constexpr auto kTEDRightEighthDiveLoopDownToDiag = TrackElementDescriptor{
        .description = STR_DIVE_LOOP_RIGHT,
        .coordinates = { 0, 4, 152, 0, -96, 32 },
        .pieceLength = 165,
        .curveChain = { TrackCurve::none, TrackElemType::leftEighthDiveLoopUpToOrthogonal },
        .priceModifier = 458752,
        .mirrorElement = TrackElemType::leftEighthDiveLoopDownToDiag,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight, TrackElementFlag::inversionToNormal },
        .definition = { TrackGroup::diveLoop, TrackPitch::down60, TrackPitch::none, TrackRoll::none, TrackRoll::upsideDown,
                        -56 },
        .spinFunction = SpinFunction::r9,
        .verticalFactor = EvaluatorDiveLoopDown,
        .lateralFactor = EvaluatorDiveLoopDownRight,
        .sequenceData = { 6,
                          { kRightEighthDiveLoopDownToDiagSeq0, kRightEighthDiveLoopDownToDiagSeq1,
                            kRightEighthDiveLoopDownToDiagSeq2, kRightEighthDiveLoopDownToDiagSeq3,
                            kRightEighthDiveLoopDownToDiagSeq4, kRightEighthDiveLoopDownToDiagSeq5 } },
    };

    constexpr auto kTEDDiagDown25Brakes = TrackElementDescriptor{
        .description = STR_BRAKES,
        .coordinates = { 4, 4, 16, 0, -32, 32 },
        .pieceLength = 45,
        .curveChain = { TrackElemType::diagDown25Brakes, TrackElemType::diagDown25Brakes },
        .priceModifier = 109824,
        .mirrorElement = TrackElemType::diagDown25Brakes,
        .flags = { TrackElementFlag::down, TrackElementFlag::startsAtHalfHeight },
        .definition = { TrackGroup::inclinedBrakes, TrackPitch::down25, TrackPitch::down25, TrackRoll::none, TrackRoll::none,
                        0 },
        .sequenceData = { 4, { kDiagDown25Seq0, kDiagDown25Seq1, kDiagDown25Seq2, kDiagDown25Seq3 } },
    };

    static constexpr auto kTrackElementDescriptors = std::to_array<TrackElementDescriptor>({
        kTEDFlat,
        kTEDEndStation,
        kTEDBeginStation,
        kTEDMiddleStation,
        kTEDUp25,
        kTEDUp60,
        kTEDFlatToUp25,
        kTEDUp25ToUp60,
        kTEDUp60ToUp25,
        kTEDUp25ToFlat,
        kTEDDown25,
        kTEDDown60,
        kTEDFlatToDown25,
        kTEDDown25ToDown60,
        kTEDDown60ToDown25,
        kTEDDown25ToFlat,
        kTEDLeftQuarterTurn5Tiles,
        kTEDRightQuarterTurn5Tiles,
        kTEDFlatToLeftBank,
        kTEDFlatToRightBank,
        kTEDLeftBankToFlat,
        kTEDRightBankToFlat,
        kTEDBankedLeftQuarterTurn5Tiles,
        kTEDBankedRightQuarterTurn5Tiles,
        kTEDLeftBankToUp25,
        kTEDRightBankToUp25,
        kTEDUp25ToLeftBank,
        kTEDUp25ToRightBank,
        kTEDLeftBankToDown25,
        kTEDRightBankToDown25,
        kTEDDown25ToLeftBank,
        kTEDDown25ToRightBank,
        kTEDLeftBank,
        kTEDRightBank,
        kTEDLeftQuarterTurn5TilesUp25,
        kTEDRightQuarterTurn5TilesUp25,
        kTEDLeftQuarterTurn5TilesDown25,
        kTEDRightQuarterTurn5TilesDown25,
        kTEDSBendLeft,
        kTEDSBendRight,
        kTEDLeftVerticalLoop,
        kTEDRightVerticalLoop,
        kTEDLeftQuarterTurn3Tiles,
        kTEDRightQuarterTurn3Tiles,
        kTEDLeftBankedQuarterTurn3Tiles,
        kTEDRightBankedQuarterTurn3Tiles,
        kTEDLeftQuarterTurn3TilesUp25,
        kTEDRightQuarterTurn3TilesUp25,
        kTEDLeftQuarterTurn3TilesDown25,
        kTEDRightQuarterTurn3TilesDown25,
        kTEDLeftQuarterTurn1Tile,
        kTEDRightQuarterTurn1Tile,
        kTEDLeftTwistDownToUp,
        kTEDRightTwistDownToUp,
        kTEDLeftTwistUpToDown,
        kTEDRightTwistUpToDown,
        kTEDHalfLoopUp,
        kTEDHalfLoopDown,
        kTEDLeftCorkscrewUp,
        kTEDRightCorkscrewUp,
        kTEDLeftCorkscrewDown,
        kTEDRightCorkscrewDown,
        kTEDFlatToUp60,
        kTEDUp60ToFlat,
        kTEDFlatToDown60,
        kTEDDown60ToFlat,
        kTEDTowerBase,
        kTEDTowerSection,
        kTEDFlatCovered,
        kTEDUp25Covered,
        kTEDUp60Covered,
        kTEDFlatToUp25Covered,
        kTEDUp25ToUp60Covered,
        kTEDUp60ToUp25Covered,
        kTEDUp25ToFlatCovered,
        kTEDDown25Covered,
        kTEDDown60Covered,
        kTEDFlatToDown25Covered,
        kTEDDown25ToDown60Covered,
        kTEDDown60ToDown25Covered,
        kTEDDown25ToFlatCovered,
        kTEDLeftQuarterTurn5TilesCovered,
        kTEDRightQuarterTurn5TilesCovered,
        kTEDSBendLeftCovered,
        kTEDSBendRightCovered,
        kTEDLeftQuarterTurn3TilesCovered,
        kTEDRightQuarterTurn3TilesCovered,
        kTEDLeftHalfBankedHelixUpSmall,
        kTEDRightHalfBankedHelixUpSmall,
        kTEDLeftHalfBankedHelixDownSmall,
        kTEDRightHalfBankedHelixDownSmall,
        kTEDLeftHalfBankedHelixUpLarge,
        kTEDRightHalfBankedHelixUpLarge,
        kTEDLeftHalfBankedHelixDownLarge,
        kTEDRightHalfBankedHelixDownLarge,
        kTEDLeftQuarterTurn1TileUp60,
        kTEDRightQuarterTurn1TileUp60,
        kTEDLeftQuarterTurn1TileDown60,
        kTEDRightQuarterTurn1TileDown60,
        kTEDBrakes,
        kTEDBooster,
        kTEDMaze,
        kTEDLeftQuarterBankedHelixLargeUp,
        kTEDRightQuarterBankedHelixLargeUp,
        kTEDLeftQuarterBankedHelixLargeDown,
        kTEDRightQuarterBankedHelixLargeDown,
        kTEDLeftQuarterHelixLargeUp,
        kTEDRightQuarterHelixLargeUp,
        kTEDLeftQuarterHelixLargeDown,
        kTEDRightQuarterHelixLargeDown,
        kTEDUp25LeftBanked,
        kTEDUp25RightBanked,
        kTEDWaterfall,
        kTEDRapids,
        kTEDOnRidePhoto,
        kTEDDown25LeftBanked,
        kTEDDown25RightBanked,
        kTEDWaterSplash,
        kTEDFlatToUp60LongBase,
        kTEDUp60ToFlatLongBase,
        kTEDWhirlpool,
        kTEDDown60ToFlatLongBase,
        kTEDFlatToDown60LongBase,
        kTEDCableLiftHill,
        kTEDReverseFreefallSlope,
        kTEDReverseFreefallVertical,
        kTEDUp90,
        kTEDDown90,
        kTEDUp60ToUp90,
        kTEDDown90ToDown60,
        kTEDUp90ToUp60,
        kTEDDown60ToDown90,
        kTEDBrakeForDrop,
        kTEDLeftEighthToDiag,
        kTEDRightEighthToDiag,
        kTEDLeftEighthToOrthogonal,
        kTEDRightEighthToOrthogonal,
        kTEDLeftEighthBankToDiag,
        kTEDRightEighthBankToDiag,
        kTEDLeftEighthBankToOrthogonal,
        kTEDRightEighthBankToOrthogonal,
        kTEDDiagFlat,
        kTEDDiagUp25,
        kTEDDiagUp60,
        kTEDDiagFlatToUp25,
        kTEDDiagUp25ToUp60,
        kTEDDiagUp60ToUp25,
        kTEDDiagUp25ToFlat,
        kTEDDiagDown25,
        kTEDDiagDown60,
        kTEDDiagFlatToDown25,
        kTEDDiagDown25ToDown60,
        kTEDDiagDown60ToDown25,
        kTEDDiagDown25ToFlat,
        kTEDDiagFlatToUp60,
        kTEDDiagUp60ToFlat,
        kTEDDiagFlatToDown60,
        kTEDDiagDown60ToFlat,
        kTEDDiagFlatToLeftBank,
        kTEDDiagFlatToRightBank,
        kTEDDiagLeftBankToFlat,
        kTEDDiagRightBankToFlat,
        kTEDDiagLeftBankToUp25,
        kTEDDiagRightBankToUp25,
        kTEDDiagUp25ToLeftBank,
        kTEDDiagUp25ToRightBank,
        kTEDDiagLeftBankToDown25,
        kTEDDiagRightBankToDown25,
        kTEDDiagDown25ToLeftBank,
        kTEDDiagDown25ToRightBank,
        kTEDDiagLeftBank,
        kTEDDiagRightBank,
        kTEDLogFlumeReverser,
        kTEDSpinningTunnel,
        kTEDLeftBarrelRollUpToDown,
        kTEDRightBarrelRollUpToDown,
        kTEDLeftBarrelRollDownToUp,
        kTEDRightBarrelRollDownToUp,
        kTEDLeftBankToLeftQuarterTurn3TilesUp25,
        kTEDRightBankToRightQuarterTurn3TilesUp25,
        kTEDLeftQuarterTurn3TilesDown25ToLeftBank,
        kTEDRightQuarterTurn3TilesDown25ToRightBank,
        kTEDPoweredLift,
        kTEDLeftLargeHalfLoopUp,
        kTEDRightLargeHalfLoopUp,
        kTEDLeftLargeHalfLoopDown,
        kTEDRightLargeHalfLoopDown,
        kTEDLeftFlyerTwistUp,
        kTEDRightFlyerTwistUp,
        kTEDLeftFlyerTwistDown,
        kTEDRightFlyerTwistDown,
        kTEDFlyerHalfLoopUninvertedUp,
        kTEDFlyerHalfLoopInvertedDown,
        kTEDLeftFlyerCorkscrewUp,
        kTEDRightFlyerCorkscrewUp,
        kTEDLeftFlyerCorkscrewDown,
        kTEDRightFlyerCorkscrewDown,
        kTEDHeartLineTransferUp,
        kTEDHeartLineTransferDown,
        kTEDLeftHeartLineRoll,
        kTEDRightHeartLineRoll,
        kTEDMinigolfHoleA,
        kTEDMinigolfHoleB,
        kTEDMinigolfHoleC,
        kTEDMinigolfHoleD,
        kTEDMinigolfHoleE,
        kTEDMultiDimInvertedFlatToDown90QuarterLoop,
        kTEDUp90ToInvertedFlatQuarterLoop,
        kTEDInvertedFlatToDown90QuarterLoop,
        kTEDLeftCurvedLiftHill,
        kTEDRightCurvedLiftHill,
        kTEDLeftReverser,
        kTEDRightReverser,
        kTEDAirThrustTopCap,
        kTEDAirThrustVerticalDown,
        kTEDAirThrustVerticalDownToLevel,
        kTEDBlockBrakes,
        kTEDLeftBankedQuarterTurn3TileUp25,
        kTEDRightBankedQuarterTurn3TileUp25,
        kTEDLeftBankedQuarterTurn3TileDown25,
        kTEDRightBankedQuarterTurn3TileDown25,
        kTEDLeftBankedQuarterTurn5TileUp25,
        kTEDRightBankedQuarterTurn5TileUp25,
        kTEDLeftBankedQuarterTurn5TileDown25,
        kTEDRightBankedQuarterTurn5TileDown25,
        kTEDUp25ToLeftBankedUp25,
        kTEDUp25ToRightBankedUp25,
        kTEDLeftBankedUp25ToUp25,
        kTEDRightBankedUp25ToUp25,
        kTEDDown25ToLeftBankedDown25,
        kTEDDown25ToRightBankedDown25,
        kTEDLeftBankedDown25ToDown25,
        kTEDRightBankedDown25ToDown25,
        kTEDLeftBankedFlatToLeftBankedUp25,
        kTEDRightBankedFlatToRightBankedUp25,
        kTEDLeftBankedUp25ToLeftBankedFlat,
        kTEDRightBankedUp25ToRightBankedFlat,
        kTEDLeftBankedFlatToLeftBankedDown25,
        kTEDRightBankedFlatToRightBankedDown25,
        kTEDLeftBankedDown25ToLeftBankedFlat,
        kTEDRightBankedDown25ToRightBankedFlat,
        kTEDFlatToLeftBankedUp25,
        kTEDFlatToRightBankedUp25,
        kTEDLeftBankedUp25ToFlat,
        kTEDRightBankedUp25ToFlat,
        kTEDFlatToLeftBankedDown25,
        kTEDFlatToRightBankedDown25,
        kTEDLeftBankedDown25ToFlat,
        kTEDRightBankedDown25ToFlat,
        kTEDLeftQuarterTurn1TileUp90,
        kTEDRightQuarterTurn1TileUp90,
        kTEDLeftQuarterTurn1TileDown90,
        kTEDRightQuarterTurn1TileDown90,
        kTEDMultiDimUp90ToInvertedFlatQuarterLoop,
        kTEDMultiDimFlatToDown90QuarterLoop,
        kTEDMultiDimInvertedUp90ToFlatQuarterLoop,
        kTEDRotationControlToggle,
        kTEDFlatTrack1x4A,
        kTEDFlatTrack2x2,
        kTEDFlatTrack4x4,
        kTEDFlatTrack2x4,
        kTEDFlatTrack1x5,
        kTEDFlatTrack1x1A,
        kTEDFlatTrack1x4B,
        kTEDFlatTrack1x1B,
        kTEDFlatTrack1x4C,
        kTEDFlatTrack3x3,
        kTEDLeftLargeCorkscrewUp,
        kTEDRightLargeCorkscrewUp,
        kTEDLeftLargeCorkscrewDown,
        kTEDRightLargeCorkscrewDown,
        kTEDLeftMediumHalfLoopUp,
        kTEDRightMediumHalfLoopUp,
        kTEDLeftMediumHalfLoopDown,
        kTEDRightMediumHalfLoopDown,
        kTEDLeftZeroGRollUp,
        kTEDRightZeroGRollUp,
        kTEDLeftZeroGRollDown,
        kTEDRightZeroGRollDown,
        kTEDLeftLargeZeroGRollUp,
        kTEDRightLargeZeroGRollUp,
        kTEDLeftLargeZeroGRollDown,
        kTEDRightLargeZeroGRollDown,
        kTEDLeftFlyerLargeHalfLoopUninvertedUp,
        kTEDRightFlyerLargeHalfLoopUninvertedUp,
        kTEDLeftFlyerLargeHalfLoopInvertedDown,
        kTEDRightFlyerLargeHalfLoopInvertedDown,
        kTEDLeftFlyerLargeHalfLoopInvertedUp,
        kTEDRightFlyerLargeHalfLoopInvertedUp,
        kTEDLeftFlyerLargeHalfLoopUninvertedDown,
        kTEDRightFlyerLargeHalfLoopUninvertedDown,
        kTEDFlyerHalfLoopInvertedUp,
        kTEDFlyerHalfLoopUninvertedDown,
        kTEDLeftEighthToDiagUp25,
        kTEDRightEighthToDiagUp25,
        kTEDLeftEighthToDiagDown25,
        kTEDRightEighthToDiagDown25,
        kTEDLeftEighthToOrthogonalUp25,
        kTEDRightEighthToOrthogonalUp25,
        kTEDLeftEighthToOrthogonalDown25,
        kTEDRightEighthToOrthogonalDown25,
        kTEDDiagUp25ToLeftBankedUp25,
        kTEDDiagUp25ToRightBankedUp25,
        kTEDDiagLeftBankedUp25ToUp25,
        kTEDDiagRightBankedUp25ToUp25,
        kTEDDiagDown25ToLeftBankedDown25,
        kTEDDiagDown25ToRightBankedDown25,
        kTEDDiagLeftBankedDown25ToDown25,
        kTEDDiagRightBankedDown25ToDown25,
        kTEDDiagLeftBankedFlatToLeftBankedUp25,
        kTEDDiagRightBankedFlatToRightBankedUp25,
        kTEDDiagLeftBankedUp25ToLeftBankedFlat,
        kTEDDiagRightBankedUp25ToRightBankedFlat,
        kTEDDiagLeftBankedFlatToLeftBankedDown25,
        kTEDDiagRightBankedFlatToRightBankedDown25,
        kTEDDiagLeftBankedDown25ToLeftBankedFlat,
        kTEDDiagRightBankedDown25ToRightBankedFlat,
        kTEDDiagFlatToLeftBankedUp25,
        kTEDDiagFlatToRightBankedUp25,
        kTEDDiagLeftBankedUp25ToFlat,
        kTEDDiagRightBankedUp25ToFlat,
        kTEDDiagFlatToLeftBankedDown25,
        kTEDDiagFlatToRightBankedDown25,
        kTEDDiagLeftBankedDown25ToFlat,
        kTEDDiagRightBankedDown25ToFlat,
        kTEDDiagUp25LeftBanked,
        kTEDDiagUp25RightBanked,
        kTEDDiagDown25LeftBanked,
        kTEDDiagDown25RightBanked,
        kTEDLeftEighthBankToDiagUp25,
        kTEDRightEighthBankToDiagUp25,
        kTEDLeftEighthBankToDiagDown25,
        kTEDRightEighthBankToDiagDown25,
        kTEDLeftEighthBankToOrthogonalUp25,
        kTEDRightEighthBankToOrthogonalUp25,
        kTEDLeftEighthBankToOrthogonalDown25,
        kTEDRightEighthBankToOrthogonalDown25,
        kTEDDiagBrakes,
        kTEDDiagBlockBrakes,
        kTEDDown25Brakes,
        kTEDDiagBooster,
        kTEDDiagFlatToUp60LongBase,
        kTEDDiagUp60ToFlatLongBase,
        kTEDDiagFlatToDown60LongBase,
        kTEDDiagDown60ToFlatLongBase,
        kTEDLeftEighthDiveLoopUpToOrthogonal,
        kTEDRightEighthDiveLoopUpToOrthogonal,
        kTEDLeftEighthDiveLoopDownToDiag,
        kTEDRightEighthDiveLoopDownToDiag,
        kTEDDiagDown25Brakes,
    });
    static_assert(kTrackElementDescriptors.size() == EnumValue(TrackElemType::count));

#pragma endregion

    const TrackElementDescriptor& GetTrackElementDescriptor(TrackElemType type)
    {
        if (EnumValue(type) >= kTrackElementDescriptors.size())
            return kTrackElementDescriptors[0];
        return kTrackElementDescriptors[EnumValue(type)];
    }

} // namespace OpenRCT2::TrackMetadata
