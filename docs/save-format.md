# File

All strings are null terminated UTF-8.

Array32:
  length:       uint32
  element-size: uint32
  data:         blob

  An element-size of 0 indicates varying. E.g. for strings.

string-table:   []
  lcode:        string      (E.g. "en-GB")
  value:        string

## File

Header:
  magic:                uint32
  target-version:       uint32
  min-version:          uint32
  num-chunks:           uint32
  uncompressed-size:    uint64
  compression:          uint32
  sha1:                 byte[20]

Chunks:
  id:                   uint32
  offset:               uint64
  length:               uint64

## Chunks

Authoring:
  engine:               string              E.g. "openrct2 v0.1.2 (Linux)"
  authors:              string[]
  notes:                string
  date-started:         timestamp
  date-modified:        timestamp

Objects:                object[]
  type:                 uint16
  id:                   string
  version:              string

Scenario:
  category:                     uint32
  name:                         string-table
  park-name:                    string-table
  details:                      string-table
  objective-type:               uint32
  objective-year:               uint16
  objective-guests:             uint32
  objective-rating:             uint16
  objective-ride-excitement:    uint16
  objective-ride-length:        uint16
  objective-park-value:         money32
  objective-ride-profit:        money32
  objective-shop-profit:        money32
  completed-company-value:      money32
  completed-name:               string

General:
  ticks:                        uint32
  elapsed-months:               uint32
  current-day:                  uint16
  rand:                         uint32[2]
  initial-cash:                 money32
  guest-initial-cash:           money16
  guest-initial-hunger:         uint8
  guest-initial-thirst:         uint8
  guest-spawns:                 xy32
  land-price:                   money32
  construction-rights-price:    money32

Interface:
  main-viewport:
    x:                          uint16
    y:                          uint16
    zoom:                       uint8
    rotation:                   uint8
  last-entrance-style:          uint32

Climate:
    climate:                uint8
    climate-update-timer:   uint16
    current:
      weather:              uint8
      temperature:          uint8
      effect:               uint8
      gloom:                uint8
      rain-level:           uint8
    next:
      (same as above)

Park:
  name:                     string-id
  cash:                     money32
  loan:                     money32
  loan-max:                 money32
  interest:                 uint16
  flags:                    uint64
  entrance-fee:             money32
  rating:                   uint16
  rating-casualty-penalty:  uint16
  current-expenditure:      money32
  current-profit:           money32
  total-admissions:         uint32
  income-from-admissions:   money32
  handyman-colour:          uint8
  mechanic-colour:          uint8
  security-colour:          uint8
  campaigns:
    weeks-left:             uint16
    ride-index:             uint32
  research-funding:         uint8
  research-enabled:         uint32
  research-progress-stage:  uint8
  research-progress:        uint16
  research-last-item:       uint32
  research-next-item:       uint32

  rating-warning-days:      uint16
  peep-warning-throttle:    uint8[16]
  awards:


History:
  rating-history:           uint16[]
  guests-history:           uint32[]
  cash-history:             money32[]
  weekly-profit-history:    money32[]
  park-value-history:       money32[]
  expenditure-history:      money32[][]

Inventions:
  

Tiles:
  map-size:                 xy32
  tile-elements:            tile-element[]

Things:

Rides:

Banners:

Animations:

Staff:

Strings:                    string[]

Editor:
  step:                     uint8

Derived:
  park-size:                uint32
  guests-in-park:           uint32
  guests-heading-for-park:  uint32
  company-value:            money32
  park-value:               money32


## Tile Element

tile-element-base:
  type:                     uint8
  flags:                    uint8
  base-z:                   uint8
  clear-z:                  uint8

surface-element:
  slope:                    uint8
  water-height:             uint8
  ownership:                uint8
  grass-length:             uint8
  surface-object-id:        uint8
  edge-object-id:           uint8
  (spare: 6)

footpath-element:
  object-id:                uint16
  edges:                    uint8
  flags2:                   uint8
  addition:                 uint16
  addition-status:          uint8
  station-index:            uint8
  ride-index:               uint32
  (spare: -2)

track-element:
  type:                     uint16          (straight, 25 deg up, brakes etc.)
  sequence:                 uint8
  style:                    uint8           (wooden, intamin, b&m etc.)
  station-index:            uint8
  colour:                   uint8
  flags:                    uint8           (station / on-ride)
  (spare: 5)

track-element (maze):
  type:                     uint16
  maze-entry:               uint16
  (spare: 8)

entrance-element:
  object-id:                uint16
  footpath-object-id:       uint16
  ride-index:               uint32
  station-index:            uint8
  type:                     uint8
  (spare: 2)

scenery-element:
  object-id:                uint32
  age:                      uint8
  colours:                  uint8[2]
  (spare: 5)

scenery-large-element:
  object-id:                uint32
  colour:                   uint8[3]

wall-element:
  object-id:                uint32
  colours:                  uint8[3]
  animation:                uint8
  banner-index:             uint32
  (spare: 0)

banner-element:
  object-id:                uint16
  index:                    uint32
  flags:                    uint8
  position:                 uint8
  (spare: 4)


## Limits

Ride index:                     uint32
Banner index:                   uint32
Station index:                  uint8

Ride object index:              uint16
Terrain (surface) object index: uint8
Terrain (edge) object index:    uint8
Entrance object index:          uint16 (can be for each type if necessary)
Footpath object index:          uint16
Footpath addition object index: uint16
Scenery object index:           uint32
Scenery (large) object index:   uint32
Wall object index:              uint32
Banner object index:            uint16
