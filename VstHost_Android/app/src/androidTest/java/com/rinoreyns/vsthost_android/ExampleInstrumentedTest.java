package com.rinoreyns.vsthost_android;

import static com.rinoreyns.vsthost_android.common.FilesCopier.copyAssets;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Test;
import org.junit.runner.RunWith;

import java.io.File;
import java.io.IOException;

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

    public native int intFromJNI(String plugin_path);
    public native String getPlatformId();

    @Test
    public void SetSimpleFlowForLib()
    {
        String copy_dest_folder = "/data/data/com.rinoreyns.vsthost_android/Devices";
        try
        {
            copyAssets(copy_dest_folder,
                    "",
                    InstrumentationRegistry.getInstrumentation().getTargetContext().getAssets(),
                    getPlatformId());
        } catch (Exception e)
        {
            e.printStackTrace();
        }

        File plugin_folder = new File(copy_dest_folder + "/adelay.vst3");

        if (plugin_folder.exists()) {
            String deleteCmd = "chmod 777 -R " + copy_dest_folder + "/adelay.vst3";
            Runtime runtime = Runtime.getRuntime();
            try {
                runtime.exec(deleteCmd);
            } catch (IOException ignored) { }
        }
        else
        {
            fail("Folder with plugin dosen't exists.");
        }

        int status = intFromJNI(copy_dest_folder);
        assertEquals(0, status);

    }
}
