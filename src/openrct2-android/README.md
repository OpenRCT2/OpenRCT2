
Building on Linux / Windows Subsystem for Linux (WSL)
---------------------------------------------

At the first time, open `bash` and paste:

```
apt-get install openjdk-8-jre-headless openjdk-8-jdk-headless wget unzip
wget https://dl.google.com/android/repository/sdk-tools-linux-3859397.zip
unzip sdk-tools-linux-3859397.zip -d /home/android-sdk
rm sdk-tools-linux-3859397.zip
/home/android-sdk/tools/bin/sdkmanager "platform-tools" "platforms;android-25" "ndk-bundle"
```
*`sdkmanager` shows no progress while downloading and installing so be patient and wait... (it could take few minutes)*

---------------------------------------------
Make sure that `gradlew` file have LF end of lines and not CRLF.
Now you should be able to build it. Open `bash` in `src/openrct2-android` and paste:
```
ANDROID_HOME=/home/android-sdk ./gradlew build
```
After succesful build there are **.apk** files compiled in `src/openrct2-android/app/build/outputs/apk`.