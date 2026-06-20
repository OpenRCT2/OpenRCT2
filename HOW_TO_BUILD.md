# OpenRCT2 빌드 가이드 (A to Z)

이 문서는 OpenRCT2를 Windows 환경에서 처음부터 끝까지 빌드하는 방법을 설명합니다.

---

## 1. 사전 요구사항

### 필수 도구

| 도구 | 설명 | 다운로드 |
|------|------|----------|
| **Visual Studio 2022 Build Tools** (또는 Visual Studio 2022) | C++ 컴파일러 및 MSBuild 포함 | https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022 |
| **Git** | 소스 코드 내려받기 | https://git-scm.com/download/win |

Visual Studio Build Tools 설치 시 **"C++를 사용한 데스크톱 개발"** 워크로드를 반드시 선택해야 합니다.

### 선택 도구

| 도구 | 설명 | 다운로드 |
|------|------|----------|
| **CMake** | CMake 기반 빌드 시 필요 | https://cmake.org/download/ |
| **vcpkg** | CMake 기반 빌드 시 필요 (의존성 관리) | https://vcpkg.io/ |

---

## 2. 소스 코드 내려받기

```bash
git clone https://github.com/OpenRCT2/OpenRCT2.git
cd OpenRCT2
```

---

## 3. MSBuild로 빌드하기 (권장, Windows 전용)

이 방법은 Visual Studio Build Tools만 있으면 됩니다. CMake나 vcpkg가 따로 필요하지 않습니다.

### 3.1 Visual Studio 개발자 명령 프롬프트 열기

시작 메뉴에서 **"Developer Command Prompt for VS 2022"** 를 검색하여 실행합니다.
또는 일반 명령 프롬프트(cmd.exe)에서 직접 환경 변수를 설정합니다:

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
```

> 참고: Visual Studio (IDE)를 설치했다면 경로가 `BuildTools` 대신 `Community` / `Professional` / `Enterprise`일 수 있습니다.

### 3.2 빌드 실행

```cmd
msbuild openrct2.proj /p:Configuration=Release /p:Platform=x64 /m
```

| 옵션 | 설명 |
|------|------|
| `/p:Configuration=Release` | 릴리스 빌드 (Debug로 변경 가능) |
| `/p:Platform=x64` | 64비트 빌드 |
| `/m` | 멀티코어 병렬 빌드 (속도 향상) |

### 3.3 Debug 빌드

```cmd
msbuild openrct2.proj /p:Configuration=Debug /p:Platform=x64 /m
```

### 3.4 빌드 출력 확인

빌드가 완료되면 `bin\` 디렉터리에 결과물이 생성됩니다:

| 파일 | 설명 |
|------|------|
| `bin\openrct2.exe` | GUI 실행 파일 (게임) |
| `bin\openrct2-cli.exe` | CLI 실행 파일 (서버/도구) |
| `bin\tests.exe` | 테스트 실행 파일 |
| `bin\libopenrct2.lib` | 정적 라이브러리 |
| `bin\data\` | 언어 파일, 셰이더, 그래픽 데이터 등 |

---

## 4. CMake로 빌드하기 (크로스 플랫폼)

CMake는 Linux, macOS, Windows에서 모두 사용할 수 있는 빌드 시스템입니다.

### 4.1 Visual Studio + CMake (CMakeSettings.json 사용)

이 방법은 Visual Studio 2022에서 CMakeSettings.json을 읽어 자동으로 구성합니다.

1. Visual Studio 2022에서 `OpenRCT2` 폴더 열기 (파일 > 열기 > 폴더)
2. Visual Studio가 자동으로 CMake 구성을 수행합니다.
3. 빌드 메뉴에서 **모두 빌드** 선택

### 4.2 명령줄 CMake + MSVC

```cmd
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64
cmake -B out -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build out
```

> 참고: CMake 방식은 vcpkg를 통해 의존성을 관리합니다. `VCPKG_ROOT` 환경 변수가 설정되어 있어야 하며, `x64-windows-static` 트리플릿이 설치되어 있어야 합니다.

### 4.3 Linux / macOS

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

자세한 내용은 공식 위키를 참고하세요:
- https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-Linux
- https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-macOS-using-CMake
- https://github.com/OpenRCT2/OpenRCT2/wiki/Building-OpenRCT2-on-Windows

---

## 5. 빌드 과정 설명

MSBuild(`openrct2.proj`)로 빌드할 때 내부적으로 일어나는 일들:

1. **의존성 다운로드** (`DownloadAllDependencies`):
   - `lib\` 폴더에 미리 빌드된 서드파티 라이브러리(zip)를 내려받고 압축 해제
   - `assets.json`에 정의된 타이틀 시퀀스, 오브젝트, OpenSFX, OpenMusic 등을 `bin\data\`에 내려받음
2. **C++ 컴파일**: `src/openrct2/`, `src/openrct2-ui/`, `src/openrct2-cli/` 소스 코드 컴파일
3. **리소스 파일 복사**: 언어 파일(`data/language/`), 셰이더(`data/shaders/`), 시나리오 패치 등을 `bin\data\`로 복사
4. **스프라이트 빌드**: `openrct2-cli sprite build` 명령어로 `g2.dat`, `fonts.dat`, `tracks.dat`, `palettes.dat` 생성
5. **테스트 빌드** (선택사항): `test/tests/` 디렉터리의 테스트 코드 컴파일

---

## 6. 자주 묻는 질문

### Q1. "MSBuild.exe를 찾을 수 없습니다" 오류가 발생합니다.

Visual Studio Build Tools가 설치되었는지 확인하세요. 설치 후에도 명령 프롬프트에서 인식되지 않으면 **Developer Command Prompt for VS 2022**를 사용하거나, `vcvarsall.bat`를 실행한 후 빌드하세요.

### Q2. 의존성 다운로드가 실패합니다.

`openrct2.deps.targets` 파일에서 GitHub Releases URL로 의존성을 내려받습니다. 방화벽이나 프록시 설정을 확인하고, 재시도하려면 `.dependencies` 파일을 삭제한 후 다시 빌드하세요:

```cmd
del .dependencies
msbuild openrct2.proj /p:Configuration=Release /p:Platform=x64 /m
```

### Q3. 빌드는 성공했는데 `openrct2.exe` 실행이 안 됩니다.

OpenRCT2는 실행에 RollerCoaster Tycoon 2의 원본 게임 파일이 필요합니다. Steam 또는 GOG.com에서 RCT2를 구매한 후, 게임 파일이 있는 경로를 OpenRCT2에 지정해야 합니다.

### Q4. Debug 빌드와 Release 빌드의 차이는?

- **Debug**: 디버깅 정보 포함, 최적화 없음, 빌드 속도 빠름, 실행 속도 느림
- **Release**: 최적화 적용, 디버깅 정보 제한, 빌드 속도 느림, 실행 속도 빠름

---

## 7. 빌드 정리 (Clean)

```cmd
msbuild openrct2.proj /t:Clean /p:Configuration=Release /p:Platform=x64
```

또는 `bin\`, `obj\` 디렉터리를 직접 삭제해도 됩니다.

---

## 8. 고급: 추가 MSBuild 옵션

```cmd
# 특정 프로젝트만 빌드
msbuild src\openrct2\libopenrct2.vcxproj /p:Configuration=Release /p:Platform=x64

# 테스트만 빌드
msbuild test\tests\tests.vcxproj /p:Configuration=Release /p:Platform=x64

# 커스텀 버전 정보 주입
msbuild openrct2.proj /p:Configuration=Release /p:Platform=x64 /p:OPENRCT2_BRANCH=develop /p:OPENRCT2_SHA1_SHORT=abc1234
```

---

더 자세한 정보는 [OpenRCT2 공식 위키](https://github.com/OpenRCT2/OpenRCT2/wiki)를 참고하세요.
