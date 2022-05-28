package com.rinoreyns.vsthost_android;

import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.navigation.NavController;
import androidx.navigation.Navigation;
import androidx.navigation.ui.AppBarConfiguration;
import androidx.navigation.ui.NavigationUI;

import com.google.android.material.snackbar.Snackbar;
import com.rinoreyns.vsthost_android.databinding.ActivityMainBinding;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {

    private AppBarConfiguration appBarConfiguration;
    private ActivityMainBinding binding;
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
    public static String [] platforms_prefixes  =
            {
                    "armeabi-v7a-android",
                    "arm64-v8a-android",
                    "x86-android",
                    "x86_64-android"
            };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        setSupportActionBar(binding.toolbar);

        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        appBarConfiguration = new AppBarConfiguration.Builder(navController.getGraph()).build();
        NavigationUI.setupActionBarWithNavController(this, navController, appBarConfiguration);

        binding.fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        String copy_dest_folder = getFilesDir().toString() + "/Devices";
        File file = new File(copy_dest_folder);

//        if (file.exists()) {
//            String deleteCmd = "rm -r " + copy_dest_folder;
//            Runtime runtime = Runtime.getRuntime();
//            try {
//                runtime.exec(deleteCmd);
//            } catch (IOException e) { }
//        }

        try
        {
            getAssetAppFolder(copy_dest_folder, "");
        } catch (Exception e)
        {
            e.printStackTrace();
        }
        int test = intFromJNI( copy_dest_folder );
        System.out.println(test);
    }

    private void getAssetAppFolder(String dir, String suffix) throws Exception{
        File f = new File(dir);
        if (!f.exists() || !f.isDirectory())
            f.mkdirs();

        AssetManager am = getAssets();
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
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public boolean onSupportNavigateUp() {
        NavController navController = Navigation.findNavController(this, R.id.nav_host_fragment_content_main);
        return NavigationUI.navigateUp(navController, appBarConfiguration)
                || super.onSupportNavigateUp();
    }
}
