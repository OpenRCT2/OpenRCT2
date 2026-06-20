# OpenRCT2 텍스트 렌더링 완전 분석 (A to Z)

이 문서는 OpenRCT2에서 텍스트가 화면에 그려지는 전 과정을 설명합니다.
초보자도 이해할 수 있는 개요부터 핵심 코드 깊숙한 곳까지 다룹니다.

---

## 목차

- [1. 개요: 텍스트가 화면에 나오기까지](#1-개요-텍스트가-화면에-나오기까지)
- [2. 아키텍처 조망](#2-아키텍처-조망)
- [3. 폰트 시스템: Sprite vs TrueType](#3-폰트-시스템-sprite-vs-truetype)
- [4. 언어별 폰트 처리](#4-언어별-폰트-처리)
- [5. 폰트 로딩 과정](#5-폰트-로딩-과정)
- [6. 텍스트 렌더링 파이프라인](#6-텍스트-렌더링-파이프라인)
- [7. TTF 렌더링 심층 분석 (FreeType2)](#7-ttf-렌더링-심층-분석-freetype2)
- [8. 드로잉 엔진별 TTF 출력](#8-드로잉-엔진별-ttf-출력)
- [9. UI 스케일링 (windowScale / enlargedUi)](#9-ui-스케일링-windowscale--enlargedui)
- [10. 한국어 폰트 깨짐 문제 분석](#10-한국어-폰트-깨짐-문제-분석)
- [11. 문제 해결 가이드](#11-문제-해결-가이드)
- [12. 주요 파일 인덱스](#12-주요-파일-인덱스)

---

## 1. 개요: 텍스트가 화면에 나오기까지

OpenRCT2에서 "가나다"라는 글자가 화면에 나타나기까지의 단계를 간단히 요약하면:

```
"가나다" 문자열
    → Localisation (언어팩에서 번역)
    → Drawing.String.cpp (포맷 코드 해석)
    → TTF.cpp / TTFSDLPort.cpp (FreeType2 로 글꼴 렌더링 → 비트맵 생성)
    → drawRect.frag (OpenGL 셰이더) 또는 X8DrawingEngine.cpp (소프트웨어)
    → 프레임버퍼 → 화면 출력
```

**핵심 요약:**
- 게임 내 모든 문자열은 **LocalisationService**에서 현재 언어에 맞게 불러옴
- 영어/서유럽어는 **Sprite font** (고정 비트맵, fonts.dat) 사용
- 한국어/중국어/일본어/아랍어는 **TrueType font** (FreeType2, 시스템 폰트) 사용
- 출력 방식은 **소프트웨어(X8)** 또는 **OpenGL** 엔진에 따라 다름

---

## 2. 아키텍처 조망

### 2.1 계층 구조

```
Layer 1: Localisation / Language
  └─ Language.cpp, Language.h              ← 언어 감지, 폰트 패밀리 결정
Layer 2: Text Layout
  └─ Drawing.String.cpp / .h               ← 문자열 처리, 포맷코드, 폰트 선택
Layer 3: Font Loading
  └─ Fonts.cpp / Fonts.h                   ← 폰트 설정, 로딩
  └─ FontFamilies.cpp / FontFamilies.h     ← 언어별 폰트 우선순위 목록
Layer 4: TTF Engine
  └─ TTF.cpp / TTF.h                       ← 캐시, 표면 관리
  └─ TTFSDLPort.cpp                        ← FreeType2 직접 렌더링
Layer 5: Drawing Engine
  └─ X8DrawingEngine.cpp / .h              ← 소프트웨어 렌더러
  └─ OpenGLDrawingEngine.cpp / .h          ← OpenGL 가속 렌더러
  └─ HardwareDisplayDrawingEngine.cpp      ← SDL2 하드웨어 렌더러
Layer 6: Display
  └─ drawrect.frag / drawrect.vert         ← OpenGL 셰이더
  └─ SDL2 창 / 스왑 체인
```

### 2.2 핵심 데이터 흐름

```
processString()
  → processStringLiteral()
    → shouldUseSpriteForCodepoint() 체크
    → Sprite: drawStringRawSprite()
        → FontSpriteGetCodepointSprite()
        → GfxDrawGlyph() → X8 / OpenGL 의 DrawGlyph()
    → TTF: drawStringRawTTF()
        → TTFSurfaceCacheGetOrAdd()
            → TTFRender() → TTF_RenderUTF8()
        → IDrawingContext::DrawTTFBitmap()
```

---

## 3. 폰트 시스템: Sprite vs TrueType

OpenRCT2는 **두 가지 폰트 시스템**을 상황에 따라 혼용합니다.

### 3.1 Sprite Font (스프라이트 폰트)

| 항목 | 내용 |
|------|------|
| 데이터 | `fonts.dat` (g2.dat에 포함) |
| 글리프 수 | 224개 (SPR_FONTS_GLYPH_COUNT) |
| 스타일 | tiny / small / medium (3세트) |
| 적용 언어 | 영어, 불어, 독어, 스페인어 등 서유럽 전부 |
| 커버 범위 | ASCII (32-255), Latin-1, Cyrillic, 특수문자/이모지 |
| 한글 커버 | **없음** |
| 파일 | `Font.cpp`, `Font.h`, `Drawing.Sprite.cpp` |

**특징:**
- 8비트 팔레트 인덱스 이미지 (256색)
- 고정 픽셀 크기 (small=10px, medium=10px, tiny=6px)
- 크기 변경 불가, 확대 시 픽셀 깨짐
- G1Element 구조체로 관리

### 3.2 TrueType Font (TTF)

| 항목 | 내용 |
|------|------|
| 엔진 | FreeType2 |
| 구현 | SDL2_ttf 포트 (`TTFSDLPort.cpp`) |
| 적용 언어 | 한국어, 일본어, 중국어, 아랍어, 베트남어 |
| 폰트 파일 | OS에 설치된 시스템 폰트 사용 |
| 폰트 경로 | `Platform.GetFontPath()` → Windows: `%WINDIR%\Fonts\` |

**특징:**
- 벡터 폰트 → 모든 크기에서 선명
- 힌팅(hinting) 지원으로 작은 크기에서도 가독성 유지
- 한글/한자 완벽 지원
- 시스템 폰트에 의존적 (폰트 없으면 Sprite로 폴백)

---

## 4. 언어별 폰트 처리

### 4.1 언어 → 폰트 패밀리 매핑

`Language.cpp`에서 각 언어에 폰트 패밀리를 연결합니다:

```cpp
// Language.cpp
{ "ko-KR", "Korean", "Korean", LANGUAGE_UNDEFINED, FAMILY(&TTFFamilyKorean), false },
{ "ja-JP", "Japanese", "日本語", LANGUAGE_UNDEFINED, FAMILY(&TTFFamilyJapanese), false },
...
{ "en-US", "English", "English", LANGUAGE_ENGLISH_UK, FAMILY(&TTFFamilyOpenRCT2Sprite), false },
```

- `font_family` 필드가 `kFamilyOpenRCT2Sprite`이면 Sprite 폰트
- 그 외에는 TTF 폰트 (CJK, Arabic 등)

### 4.2 한국어 폰트 패밀리

`FontFamilies.cpp`:

```cpp
TTFontFamily const TTFFamilyKorean = {
    &TTFFontGulim,          // 1순위: gulim.ttc
    &TTFFontNanum,          // 2순위: NanumGothic.ttf
    &TTFFontMalgun,         // 3순위: malgun.ttf
    &TTFFontNotoSansCJK,    // 4순위: NotoSansCJK-Regular.ttc
};
```

**Fallback 체인:**
1. 패밀리 목록 순서대로 시도
2. 모두 실패 → `TTFFamilySansSerif` (Arial 등)
3. 그것도 실패 → **Sprite font** (한글 깨짐, □로 표시됨)

### 4.3 폰트 디스크립터

`Fonts.cpp`:

```cpp
TTFFontSetDescriptor TTFFontGulim = { {
    // { filename, font_name, ptSize, offset_x, offset_y, height, hinting_threshold }
    { "gulim.ttc", "Gulim", 12, 1, 0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },  // small
    { "gulim.ttc", "Gulim", 12, 1, 0, 15, HINTING_THRESHOLD_MEDIUM, nullptr },  // medium
    { "gulim.ttc", "Gulim", 10, 1, 0, 10, HINTING_DISABLED,         nullptr },  // tiny
} };
```

**필드 설명:**
| 필드 | 설명 |
|------|------|
| `filename` | 윈도우 폰트 폴더 내 파일명 |
| `font_name` | 폰트 페이스 이름 (로깅용) |
| `ptSize` | **포인트 사이즈 (고정, windowScale 영향 안 받음)** |
| `offset_x/y` | 렌더링 위치 보정 |
| `height` | 라인 높이 |
| `hinting_threshold` | 안티앨리어싱 임계값 (0, 40, 60) |

---

## 5. 폰트 로딩 과정

### 5.1 TryLoadFonts()

`Fonts.cpp:172-220`:

```
TryLoadFonts()
  ├─ 현 언어의 font_family 확인
  ├─ kFamilyOpenRCT2Sprite 이면 → LoadSpriteFont() (Sprite 모드)
  └─ TTF 패밀리면:
      ├─ 커스텀 폰트 설정 있으면 → LoadCustomConfigFont()
      ├─ 패밀리 목록 순회 → LoadFont() (첫 성공 시 반환)
      ├─ TTFFamilySansSerif 로 재시도
      └─ 전부 실패 → LoadSpriteFont() (폴백)
```

### 5.2 LoadFont()

```cpp
static bool LoadFont(LocalisationService& localisationService, TTFFontSetDescriptor* font)
{
    TTFDispose();                          // 기존 폰트 해제
    localisationService.UseTrueTypeFont(true);
    gCurrentTTFFontSet = font;
    return TTFInitialise();
}
```

### 5.3 TTFInitialise()

`TTF.cpp:95-132`:

1. `TTF_Init()` → FreeType2 라이브러리 초기화
2. 세 가지 스타일(small/medium/tiny) 각각에 대해:
   - `Platform::GetFontPath()` → 시스템 폰트 경로 탐색
   - `TTF_OpenFont(path, ptSize)` → 폰트 파일 열기
3. `TTFToggleHinting()` → 힌팅 설정
4. 표면/폭 캐시 준비

### 5.4 TTF_OpenFont()

`TTFSDLPort.cpp:350-545`:

1. 파일 스트림 열기
2. `FT_Open_Face()` → FreeType2 페이스 생성
3. 문자 맵 설정 (Unicode 우선)
4. `FT_Set_Char_Size(face, 0, ptsize * 64, 0, 0)` → 72 DPI 기준 pt 설정
5. 폰트 메트릭 계산 (ascent, descent, height, lineskip, underline)
6. 기본 스타일 설정

---

## 6. 텍스트 렌더링 파이프라인

### 6.1 processString() → 문자열 처리 진입점

`Drawing.String.cpp:688`:

```
processString(rt, text, info)
  ├─ 텍스트를 FmtString 으로 파싱 (포맷 코드 분리)
  ├─ 각 토큰 처리:
  │   ├─ 리터럴 문자열 → processStringLiteral()
  │   ├─ 포맷 코드 (색상, 폰트 변경 등) → processFormatCode()
  │   └─ 개별 유니코드 → processStringCodepoint()
  └─ info.current 위치 업데이트
```

### 6.2 processStringLiteral() → Sprite/TTF 분기

`Drawing.String.cpp:619`:

```
processStringLiteral()
  ├─ TTF 모드이고 (fontDesc->font != nullptr 이면):
  │   ├─ 각 codepoint를 shouldUseSpriteForCodepoint() 로 검사
  │   │   (특수문자: ➡✅❌ 등은 Sprite 로 처리)
  │   ├─ 연속된 TTF 가능 codepoint → drawStringRawTTF()
  │   └─ Sprite 전용 문자 → drawStringRawSprite()
  └─ Sprite 모드이면:
      └─ drawStringRawSprite()
```

### 6.3 Sprite Path: drawStringRawSprite()

```
drawStringRawSprite()
  → FontSpriteGetCodepointSprite(codepoint)  ← codepoint → sprite ID
  → GfxDrawGlyph(rt, imageId, x, y, palette)
    → X8:  GfxDrawSpritePaletteSetSoftware()  ← 팔레트 매핑 후 메모리 복사
    → GL:  OpenGLDrawingContext::DrawGlyph()  ← 텍스처 업로드
```

**Sprite는 팔레트 인덱스(0-255)** 로 되어 있어서, 팔레트를 바꾸면 글자색이 바뀜.

### 6.4 TTF Path: drawStringRawTTF()

`Drawing.String.cpp:483-515`:

```
drawStringRawTTF(rt, text, info)
  ├─ TTFInitialise() 확인
  ├─ fontDesc = TTFGetFontFromSpriteBase(info.fontStyle)  ← small/medium/tiny
  ├─ noDraw(폭 측정) → TTFGetWidthCacheGetOrAdd() → 폭만 반환
  ├─ TTFSurfaceCacheGetOrAdd(font, text)  ← 캐시에서 기존 표면 찾기
  │   └─ miss: TTFRender() → TTF_RenderUTF8() → FreeType2 로 실제 렌더링
  ├─ DrawTTFBitmap(rt, info, surface, drawX + offset_x, drawY + offset_y, hintThresh)
  └─ info.current.x += surface.w (폭만큼 커서 이동)
```

---

## 7. TTF 렌더링 심층 분석 (FreeType2)

### 7.1 TTF_RenderUTF8()

`TTFSDLPort.cpp:1272-1408`:

전체 문자열을 하나의 **grayscale 비트맵**으로 렌더링:

```
TTF_RenderUTF8(font, text, shaded)
  ├─ TTF_SizeUTF8() → 전체 텍스트의 폭/높이 계산
  ├─ 표면 할당 (width × height × 1byte)
  ├─ 각 문자별:
  │   ├─ Find_Glyph() → glyph 캐시에서 찾거나 로드
  │   ├─ 케rn 적용 (FT_Get_Kerning)
  │   ├─ yoffset + minx 로 표면 내 위치 계산
  │   ├─ 비트맵 데이터 복사 *dst++ |= *src++  (OR로 합성)
  │   └─ advance 만큼 x 이동
  └─ underline/strikethrough 처리
```

**`shaded` 파라미터:**
- `true`: `CACHED_PIXMAP` (8비트 grayscale, 안티앨리어싱)
- `false`: `CACHED_BITMAP` (1비트 모노크롬)

**실제 결정 로직** (`TTF.cpp:352`):

```cpp
TTF_RenderUTF8(font, text, TTF_GetFontHinting(font) != 0);
```
→ 힌팅이 활성화되어 있으면 **shaded** (grayscale, 부드럽게)
→ 힌팅이 꺼져있으면 **비shaded** (mono, 깔끔하게)

### 7.2 Load_Glyph() - 개별 글리프 로딩

`TTFSDLPort.cpp:583-828`:

```
Load_Glyph(font, ch, cached, want)
  ├─ FT_Get_Char_Index(face, ch)  ← character → glyph index
  ├─ FT_Load_Glyph(face, index, FT_LOAD_DEFAULT | font->hinting)
  │   ← ★★★★★ 중요: FT_LOAD_DEFAULT 는 임베디드 비트맵을 우선 사용!
  │   ← hinting: 0(기본) / LIGHT / MONO / NO_HINTING
  ├─ 메트릭 계산 (minx, maxx, miny, maxy, advance)
  ├─ 이탤릭 변환 (FT_Outline_Transform)
  ├─ 아웃라인 효과 (FT_Stroker)
  ├─ FT_Render_Glyph(glyph, ft_render_mode_normal)
  │   ← 실제 픽셀 데이터 생성
  └─ 비트맵 데이터를 캐시에 복사
    └─ mono(1비트) / gray2 / gray4 / gray8 자동 변환
```

### 7.3 힌팅 (Hinting)

**FreeType2 힌팅 모드** (`TTF_SetFontHinting`):

```cpp
void TTF_SetFontHinting(TTF_Font* font, int hinting)
{
    if (hinting == TTF_HINTING_LIGHT)  // 1
        font->hinting = FT_LOAD_TARGET_ALT(FT_RENDER_MODE_LIGHT);
    else if (hinting == TTF_HINTING_MONO)  // 2
        font->hinting = FT_LOAD_TARGET_ALT(FT_RENDER_MODE_MONO);
    else if (hinting == TTF_HINTING_NONE)  // 3
        font->hinting = FT_LOAD_NO_HINTING;
    else  // 0 (기본)
        font->hinting = 0;
}
```

**한국어 폰트 힌팅 설정:**

| 폰트 | small/medium hinting | tiny hinting |
|------|---------------------|--------------|
| Gulim | `HINTING_THRESHOLD_MEDIUM` (60) | `HINTING_DISABLED` (0) |
| Malgun | `HINTING_THRESHOLD_LOW` (40) | `HINTING_THRESHOLD_LOW` (40) |
| Nanum | `HINTING_THRESHOLD_LOW` (40) | `HINTING_DISABLED` (0) |

**hinting_threshold 의 실제 역할** (`DrawTTFBitmapInternal`):
- 힌팅 활성화 시 grayscale 값이 threshold 이하이면 투명 처리
- threshold 초과이면 `BlendColours()` 혼합
- 180 초과이면 완전 불투명
- 즉 **안티앨리어싱 끝부분을 잘라내는** 용도 (더 선명하게)

### 7.4 Embedded Bitmap (임베디드 비트맵) 문제

**중요:** `FT_LOAD_DEFAULT`는 FreeType2에서 **임베디드 비트맵 스트라이크를 아웃라인보다 우선**합니다.

`TTFSDLPort.cpp:720-728`:

```cpp
/* FT_IS_SCALABLE() means that the font is in outline format,
 * but does not imply that outline is rendered as 8-bit
 * grayscale, because embedded bitmap/graymap is preferred
 * (see FT_LOAD_DEFAULT section of FreeType2 API Reference).
 * FT_Render_Glyph() can return two-color bitmap or 4/16/256-
 * color graymap according to the format of embedded bitmap/
 * graymap. */
```

**영향:**
- 한국어 폰트(Gulim, Malgun 등)는 12pt/12px에 **최적화된 임베디드 비트맵** 포함
- 1x에서는 이 비트맵이 1:1 매핑되어 깔끔
- windowScale > 1.0 에서는 작은 캔버스에 12px 비트맵을 그리고 → **업스케일** → 줄/깨짐 발생

---

## 8. 드로잉 엔진별 TTF 출력

### 8.1 X8 (소프트웨어) 엔진

`X8DrawingEngine.cpp:654-678`:

```cpp
void X8DrawingContext::DrawTTFBitmap(rt, info, surface, x, y, hintingThreshold)
{
    // 아웃라인 효과: 상하좌우 1px씩 bgColor 로 그림
    DrawTTFBitmapInternal<false>(rt, bgColor, surface, x+1, y, 0);
    DrawTTFBitmapInternal<false>(rt, bgColor, surface, x-1, y, 0);
    // ... (4방향 + inset)

    // 실제 글자:
    DrawTTFBitmapInternal<true>(rt, fgColor, surface, x, y, hintingThreshold);
}
```

**DrawTTFBitmapInternal** (`X8DrawingEngine.cpp:584-651`):

```
소스 (grayscale 8bit) → 대상 (PaletteIndex 8bit) 로 1:1 픽셀 복사
- rt.zoom_level == 0 이어야 함 (assert)
- non-zero 픽셀: colour로 채움 (또는 BlendColours)
- zero 픽셀: 투명 유지
```

### 8.2 OpenGL 엔진

`OpenGLDrawingEngine.cpp:1166-1260`:

```cpp
void OpenGLDrawingContext::DrawTTFBitmap(rt, info, surface, x, y, hintingThreshold)
{
    // TTF 표면을 OpenGL 텍스처로 업로드 (texture cache)
    // -- 텍스처 슬롯 1023개를 순환 사용 --

    // 아웃라인/inset: DrawRectCommand 4개 (FLAG_TTF_TEXT)
    command.flags = DrawRectCommand::FLAG_TTF_TEXT;
    command.zoom = 1.0f;  // ★ 고정! rt.zoom_level 무시

    // 실제 글자:
    command.flags = DrawRectCommand::FLAG_TTF_TEXT | (hintingThreshold << 8);
    command.colour = info.palette.fill;
    command.zoom = 1.0f;  // ★ 항상 1.0
}
```

**셰이더 처리** (`drawrect.frag:45-79`):

```glsl
vec2 fragCoord = vec2(floor(gl_FragCoord.x), fScreenHeight - floor(gl_FragCoord.y) - 1);
vec2 position = (fragCoord - fPosition) * fZoom;

// position 으로 텍스처 샘플링
float colourU = (fTexColour.x + position.x) / fTexColour.z;
float colourV = (fTexColour.y + position.y) / fTexColour.w;
texel = texture(uTexture, vec3(colourU, colourV, fTexColourAtlas)).r;

if ((fFlags & FLAG_TTF_TEXT) == 0) {
    texel += fColour;  // 일반 스프라이트: 팔레트 인덱스 오프셋
} else {
    // TTF 텍스트: texel 을 그대로 색상값으로 사용 (grayscale → colour 매핑)
    if (hint_thresh > 0u) {
        bool solidColor = texel > 180u;
        texel = (texel > hint_thresh) ? fColour : 0u;
        texel = texel << 8;
        if (solidColor) texel += 1u;
    } else {
        texel = fColour;  // 힌팅 없으면 모든 non-zero texel을 같은 색으로
    }
}
```

**핵심 차이:**
- 일반 스프라이트: `texel + fColour` → 팔레트 인덱스 계산
- TTF 텍스트: `texel = fColour` → grayscale 값 무시, 단일 색상으로 칠함

### 8.3 HardwareDisplay (SDL2) 엔진

`HardwareDisplayDrawingEngine.cpp`:

- X8 소프트웨어 렌더러를 SDL2 텍스처로 변환
- `CopyBitsToTexture()` → PaletteIndex 배열을 SDL RGBA 픽셀로 변환
- `SDL_RenderCopy()` → 화면에 출력

---

## 9. UI 스케일링 (windowScale / enlargedUi)

### 9.1 windowScale (연속 스케일)

| 항목 | 값 |
|------|-----|
| 설정 키 | `window_scale` (float) |
| 범위 | 0.5 ~ 5.0 |
| 기본값 | 1.0 (비 Android) |
| 조작 | Ctrl+Plus/Minus, 옵션 창 |

**작동 방식** (`UiContext.cpp:843-847`):

```cpp
void OnResize(int32_t width, int32_t height)
{
    _width  = static_cast<int32_t>(width  / Config::Get().general.windowScale);
    _height = static_cast<int32_t>(height / Config::Get().general.windowScale);
}
```

예) 1920x1080 창, windowScale=2.0
→ 논리적 캔버스 = **960x540**
→ TTF 폰트는 12pt 로 이 작은 캔버스에 렌더링
→ 캔버스를 2배 확대해서 1920x1080 으로 출력

**스케일 품질** (`UiContext.cpp:605-609`):

```cpp
if (windowScale == floor(windowScale))  // 정수 스케일 (1, 2, 3...)
    _scaleQuality = nearestNeighbour;    // 픽셀 복제 (깔끔)
else                                     // 분수 스케일 (1.5, 2.25...)
    _scaleQuality = smoothNearestNeighbour → linear;  // 선형 보간 (흐림)
```

### 9.2 enlargedUi (토글식 확대)

| 항목 | 값 |
|------|-----|
| 설정 키 | `enlarged_ui` (bool) |
| 영향 | 위젯 크기, 버튼, 드롭다운, 창 제목 |
| 폰트 | **영향 없음** (폰트 크기는 변경 안 함) |

UI 요소의 크기만 키우는 간단한 "터치 모드".

### 9.3 근본적인 문제

**TTF 포인트 사이즈는 windowScale 에 비례하지 않음:**

```
windowScale=1.0:  12pt → 960x540 캔버스 → 960x540 출력 = 선명
windowScale=2.0:  12pt → 1920x1080 캔버스 → 실제로는 960x540 에 그린 후 2배 확대
                   → 24pt처럼 보이지만 12pt 디테일만 있음 = 픽셀 깨짐
```

원인: `Fonts.cpp`의 `ptSize` 값이 **하드코딩**되어 있음.
수정해야 할 로직이 없음 - `TTFInitialise()` 호출 시점에 `ptSize * windowScale` 계산이 필요.

---

## 10. 한국어 폰트 깨짐 문제 분석

### 10.1 증상

- 1x (windowScale=1.0): 정상
- 1.5x, 2x 이상: 한글 글자에 **세로줄, 가로줄**이 생기고 깨져 보임
- 영문/숫자는 상대적으로 덜 깨짐

### 10.2 원인

**3가지 요인이 복합적으로 작용:**

#### 원인 A: 임베디드 비트맵 사용

`TTFSDLPort.cpp:603`에서 `FT_LOAD_DEFAULT` 사용 → FreeType2가 아웃라인 대신 **임베디드 비트맵**을 선택.
한국어 폰트(`gulim.ttc` 등)는 12pt에 최적화된 비트맵 포함.
이 비트맵을 업스케일하면 전형적인 **비트맵 확대 아티팩트** 발생.

#### 원인 B: 캔버스 업스케일

`UiContext.cpp:846-847`에서 `OnResize()`가 논리적 캔버스를 축소.
폰트는 계속 12pt로 렌더링 → 축소된 캔버스에서 폰트가 상대적으로 커짐.
전체 캔버스를 물리 해상도로 확대할 때 폰트도 함께 확대됨.

#### 원인 C: 최적화된 힌팅 무력화

`FT_LOAD_DEFAULT` + `FT_RENDER_MODE_LIGHT` 조합으로 렌더링된 12px 비트맵은
원래 12px 디스플레이에 맞춰 힌팅되어 있음. 이걸 24px로 확대하면
픽셀 경계에 정렬된 스템(세로줄)이 비대칭으로 확대되어 깨짐.

### 10.3 해결 방안

| 방안 | 난이도 | 영향 | 설명 |
|------|--------|------|------|
| **A: `FT_LOAD_NO_BITMAP` 추가** | **하** | 낮음 | 아웃라인 렌더링 강제, 스케일링에 강함 |
| **B: ptSize *= windowScale** | **중** | 중간 | 근본적 해결, 캐시 무효화 필요 |
| **C: 품질 설정에 따른 폰트 크기 선택** | **상** | 중간 | nearest: 정수배 pt, linear: 1x pt + 셰이더 |

#### 방안 A 상세: FT_LOAD_NO_BITMAP

`TTFSDLPort.cpp:603`:
```cpp
// BEFORE
error = FT_Load_Glyph(face, cached->index, FT_LOAD_DEFAULT | font->hinting);

// AFTER
error = FT_Load_Glyph(face, cached->index, FT_LOAD_NO_BITMAP | font->hinting);
```

**장점:** 1줄 변경, 모든 스케일에서 벡터 렌더링 → 일관된 품질
**단점:** 임베디드 비트맵 특유의 저해상도 선명함을 잃음 (1x에서 약간 흐려질 수 있음)

#### 방안 B 상세: ptSize 동적 조정

`TTF.cpp:108-125`:
```cpp
bool TTFInitialise()
{
    // ...
    for (int32_t i = 0; i < FontStyleCount; i++)
    {
        TTFFontDescriptor* fontDesc = &(gCurrentTTFFontSet->size[i]);
        auto fontPath = Platform::GetFontPath(*fontDesc);

        // BEFORE
        // fontDesc->font = TTFOpenFont(fontPath.c_str(), fontDesc->ptSize);

        // AFTER  (windowScale 반영)
        float scale = Config::Get().general.windowScale;
        int32_t scaledPtSize = static_cast<int32_t>(fontDesc->ptSize * scale);
        fontDesc->font = TTFOpenFont(fontPath.c_str(), scaledPtSize);
    }
    // ...
}
```

또한 `TriggerResize()` 호출 시 폰트를 다시 로드하거나, 캐시를 비워야 함:
```cpp
TTFSurfaceCacheDisposeAll();
TTFGetWidthCacheDisposeAll();
```

**장점:** 모든 스케일에서 네이티브 해상도로 렌더링
**단점:** 폰트 리로딩 비용, 캐시 무효화, UI 레이아웃 재계산 필요

---

## 11. 문제 해결 가이드

### 11.1 폰트가 전혀 안 나와요 (□로 표시)

```
1. 현재 언어 확인: Language.cpp
2. TryLoadFonts() 가 Sprite 폰트로 폴백했는지 확인
3. 시스템에 Gulim / Malgun / NanumGothic 폰트가 설치되어 있는지 확인
4. `%WINDIR%\Fonts\` 에 gulim.ttc 존재 확인
5. 로그 확인: "Unable to load TrueType font" 메시지
```

### 11.2 확대 시 폰트가 깨져요

```
원인: windowScale > 1.0 에서 TTF ptSize 미조정 + 임베디드 비트맵
해결:
  1. 쉬운 방법: TTFSDLPort.cpp 에 FT_LOAD_NO_BITMAP 추가
  2. 정확한 방법: TTFInitialise() 에서 ptSize *= windowScale
```

### 11.3 특정 언어에서만 폰트가 달라요

```
원인: FontFamilies.cpp 의 폰트 우선순위 차이
해결: TTFFamilyKorean 등에서 폰트 순서나 ptSize 조정
```

### 11.4 캐시 문제로 폰트 변경이 적용 안 돼요

```
TTFSurfaceCacheDisposeAll()  ← 모든 렌더링된 텍스트 비우기
TTFGetWidthCacheDisposeAll() ← 모든 폭 캐시 비우기
```

### 11.5 OpenGL / 소프트웨어 엔진 간 차이가 나요

```
X8 엔진:   팔레트 기반, zoom_level 0 가정
OpenGL:    텍스처 기반, command.zoom = 1.0f 고정
차이점: X8은 BlendColours 지원, OpenGL은 hintingThreshold 로 대체
```

---

## 12. 주요 파일 인덱스

### 폰트 설정
| 파일 | 내용 |
|------|------|
| `src/openrct2/interface/Fonts.cpp` | 폰트 디스크립터, 로딩 함수, HINTING_* 상수 |
| `src/openrct2/interface/Fonts.h` | TTFFontSetDescriptor extern 선언 |
| `src/openrct2/interface/FontFamilies.cpp` | 언어별 TTFFamily 목록 |
| `src/openrct2/interface/FontFamilies.h` | TTFFamily alias, extern |

### 텍스트 렌더링
| 파일 | 내용 |
|------|------|
| `src/openrct2/drawing/Drawing.String.cpp` | processString, ttfDrawString, drawStringRawTTF |
| `src/openrct2/drawing/Drawing.String.h` | TextDrawInfo, TextDrawFlags |
| `src/openrct2/drawing/Text.cpp` | drawText, drawTextWrapped, StaticLayout |
| `src/openrct2/drawing/Text.h` | TextPaint |
| `src/openrct2/drawing/Font.cpp` | 스프라이트 폰트, glyph → sprite 매핑 |
| `src/openrct2/drawing/Font.h` | TTFFontDescriptor, FontStyle |

### FreeType2/TTF
| 파일 | 내용 |
|------|------|
| `src/openrct2/drawing/TTF.cpp` | TTFInitialise, 캐시, hinting 토글 |
| `src/openrct2/drawing/TTF.h` | TTFSurface, TTF API 선언 |
| `src/openrct2/drawing/TTFSDLPort.cpp` | SDL2_ttf 포트, FreeType2 직접 호출 |

### 드로잉 엔진
| 파일 | 내용 |
|------|------|
| `src/openrct2/drawing/X8DrawingEngine.cpp` | DrawTTFBitmapInternal (소프트웨어) |
| `src/openrct2-ui/drawing/engines/opengl/OpenGLDrawingEngine.cpp` | DrawTTFBitmap (OpenGL) |
| `src/openrct2-ui/drawing/engines/HardwareDisplayDrawingEngine.cpp` | SDL2 하드웨어 렌더러 |
| `data/shaders/drawrect.frag` | OpenGL 프래그먼트 셰이더 (TTF 텍스처 처리) |
| `data/shaders/drawrect.vert` | OpenGL 버텍스 셰이더 |

### 언어/설정
| 파일 | 내용 |
|------|------|
| `src/openrct2/localisation/Language.cpp` | 언어 → 폰트 패밀리 매핑 |
| `src/openrct2/localisation/Language.h` | LANGUAGE_KOREAN enum |
| `src/openrct2/localisation/LocalisationService.h` | UseTrueTypeFont() |
| `src/openrct2/config/Config.h` | windowScale, enlargedUi |
| `src/openrct2-ui/UiContext.cpp` | OnResize, TriggerResize, 스케일 품질 |
