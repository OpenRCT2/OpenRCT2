package org.openrct2.android;

import java.io.IOException;
import java.util.Enumeration;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

public class ZipArchive {

    private final ZipFile _zipArchive;

    public ZipArchive(String path) throws IOException {
        _zipArchive = new ZipFile(path);
    }

    public void close() {
        try {
            _zipArchive.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public int getNumFiles() {
        return _zipArchive.size();
    }

    public String getFileName(int index) {
        Enumeration<? extends ZipEntry> entries = _zipArchive.entries();

        int i = 0;
        while (entries.hasMoreElements()) {
            ZipEntry entry = entries.nextElement();
            if (index == i) {
                return entry.getName();
            }

            i++;
        }

        return null;
    }

    public long getFileSize(int index) {
        Enumeration<? extends ZipEntry> entries = _zipArchive.entries();

        int i = 0;
        while (entries.hasMoreElements()) {
            ZipEntry entry = entries.nextElement();
            if (index == i) {
                return entry.getSize();
            }

            i++;
        }

        return -1;
    }

    public void getFile(int index) {
        // Maybe based on <http://stackoverflow.com/questions/6369402/how-to-move-data-from-java-inputstream-to-a-char-in-c-with-jni>...
    }
}
