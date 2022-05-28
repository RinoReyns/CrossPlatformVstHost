package com.rinoreyns.vsthost_android;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import android.content.Context;
import android.content.res.AssetManager;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

/**
 * Instrumented test, which will execute on an Android device.
 *
 * @see <a href="http://d.android.com/tools/testing">Testing documentation</a>
 */
@RunWith(AndroidJUnit4.class)
public class ExampleInstrumentedTest {
    static {
        try {
            System.loadLibrary("VstHostLibWrapper");
        }
        catch (Exception e) {
            System.out.println("Oops!");
        }
    }
    public static String [] platforms_prefixes  =
            {
                    "armeabi-v7a-android",
                    "arm64-v8a-android",
                    "x86-android",
                    "x86_64-android"
            };
    public native int intFromJNI(String plugin_path);
    public native String getPlatformId();

    private void getAssetAppFolder(String dir, String suffix) throws Exception{
        File f = new File(dir);
        if (!f.exists() || !f.isDirectory())
            f.mkdirs();
        if (!f.exists() || !f.isDirectory())
            f.mkdirs();
        AssetManager am = InstrumentationRegistry.getInstrumentation().getTargetContext().getAssets();
        String [] aplist = am.list(suffix);

        for(String strf:aplist){
            try
            {
                InputStream is = am.open(strf);
                copyToDisk(dir, strf, is);
            }
            catch(Exception ex)
            {
                if (suffix.equals(""))
                {
                    getAssetAppFolder(dir+"/"+strf, strf);
                }
                else
                {
                    if (Arrays.asList(platforms_prefixes).contains(strf))
                    {
                        String platform_id = getPlatformId();
                        getAssetAppFolder(dir+"/" + platform_id + "-linux", suffix + "/" + strf);
                    }
                    else
                    {
                        getAssetAppFolder(dir+"/"+strf, suffix + "/" + strf);
                    }
                }
            }
        }
    }

    public void copyToDisk(String dir,String name,InputStream is) throws IOException{
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
        File file1 = new File(dir+"/" +name);
        if (!file1.exists())
        {
            fail("copy failed " + dir + "/" +name);
        }
    }

    @Test
    public void SetSimpleFlowForLib()
    {
//        String copy_dest_folder = InstrumentationRegistry.getInstrumentation().getTargetContext().getFilesDir().toString();
//        copy_dest_folder = copy_dest_folder.replace("_android", "_android.test");
//
        Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();
        String copy_dest_folder = context.getExternalCacheDir() + "/Devices";
        File file = new File(copy_dest_folder);
        if (file.exists()) {
            String deleteCmd = "rm -r " + copy_dest_folder;
            Runtime runtime = Runtime.getRuntime();
            try {
                runtime.exec(deleteCmd);
            } catch (IOException e) { }
        }

        try
        {
            getAssetAppFolder(copy_dest_folder, "");
        } catch (Exception e)
        {
            e.printStackTrace();
        }

        File file1 = new File(copy_dest_folder + "/adelay.vst3");
        if (file1.exists())
        {
            int test = intFromJNI(copy_dest_folder);
            assertEquals(0, test);
        }
        else
        {
            fail("Folder with plugin dosen't exists.");
        }
    }
}
