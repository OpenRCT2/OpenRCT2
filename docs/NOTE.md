# Note

## 설정 저장 방식

- **DB 사용 안 함**, 그냥 INI 파일 사용
- 위치: `%USERPROFILE%\Documents\OpenRCT2\config.ini`
- 레지스트리, SQLite, AppData **전혀 안 씀**
- 자체 IniReader/IniWriter 구현으로 읽고 씀
