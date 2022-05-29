package com.rinoreyns.vsthost_android.common;

import android.content.res.AssetManager;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public class FilesCopier
{
    public static String [] platforms_prefixes  =
    {
            "armeabi-v7a-android",
            "arm64-v8a-android",
            "x86-android",
            "x86_64-android"
    };

    public static void copyFiles(String dir, String name, InputStream is) throws IOException {
        int size;
        byte[] buffer = new byte[2048];

        FileOutputStream fout = new FileOutputStream(dir+"/" +name);
        BufferedOutputStream bufferOut = new BufferedOutputStream(fout, buffer.length);

        while ((size = is.read(buffer, 0, buffer.length)) != -1) {
            bufferOut.write(buffer, 0, size);
        }
        bufferOut.flush();
        bufferOut.close();
        is.close();
        fout.close();
    }

    public static void copyAssets(String dir, String suffix, AssetManager am, String platform_id) throws Exception{
        File f = new File(dir);
        if (!f.exists() || !f.isDirectory())
            f.mkdirs();

        String [] aplist = am.list(suffix);

        for(String strf:aplist){
            try
            {
                InputStream is;
                if (strf.contains(".so"))
                {
                    is = am.open(suffix+ "/" + strf);
                }
                else
                {
                    is = am.open(strf);
                }
                copyFiles(dir, strf, is);
            }
            catch(Exception ex)
            {
                if (suffix.equals(""))
                {
                    copyAssets(dir+"/"+strf, strf, am, platform_id);
                }
                else
                {
                    if (Arrays.asList(platforms_prefixes).contains(strf))
                    {
                        copyAssets(dir+"/" + platform_id + "-linux", suffix + "/" + strf, am, platform_id);
                    }
                    else
                    {
                        copyAssets(dir+"/"+strf, suffix + "/" + strf, am, platform_id);
                    }
                }
            }
        }
    }

}
