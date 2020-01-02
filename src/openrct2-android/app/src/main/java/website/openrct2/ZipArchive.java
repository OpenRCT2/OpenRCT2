package io.openrct2;

import android.util.Log;

import org.apache.commons.io.IOUtils;

import java.io.IOException;
import java.io.InputStream;
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

    private ZipEntry getZipEntry(int index) {
        Enumeration<? extends ZipEntry> entries = _zipArchive.entries();

        int i = 0;
        while (entries.hasMoreElements()) {
            ZipEntry entry = entries.nextElement();
            if (index == i) {
                return entry;
            }

            i++;
        }

        return null;
    }

    public String getFileName(int index) {
        ZipEntry entry = getZipEntry(index);

        if (entry != null) {
            return entry.getName();
        }

        return null;
    }

    public long getFileSize(int index) {
        ZipEntry entry = getZipEntry(index);

        if (entry != null) {
            return entry.getSize();
        }

        return -1;
    }

    public int getFileIndex(String path) {
        Enumeration<? extends ZipEntry> entries = _zipArchive.entries();

        int i = 0;
        while (entries.hasMoreElements()) {
            ZipEntry entry = entries.nextElement();
            if (entry.getName().equalsIgnoreCase(path)) {
                return i;
            }

            i++;
        }

        return -1;
    }

    public long getFile(int index) throws IOException {
        ZipEntry entry = getZipEntry(index);

        if (entry == null) {
            return 0;
        }

        InputStream inputStream = _zipArchive.getInputStream(entry);

        int numBytesToRead = (int) entry.getSize();
        if (numBytesToRead == -1) {
            Log.e("ZipArchive", "Unknown length for zip entry");
            return 0;
        }

        byte[] inBuffer = new byte[numBytesToRead];
        IOUtils.read(inputStream, inBuffer);

        return allocBytes(inBuffer, numBytesToRead);
    }

    native static long allocBytes(byte[] input, int size);
}
