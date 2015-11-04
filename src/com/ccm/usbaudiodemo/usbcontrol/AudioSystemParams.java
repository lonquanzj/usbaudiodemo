package com.ccm.usbaudiodemo.usbcontrol;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.annotation.TargetApi;
import android.content.Context;
import android.media.AudioManager;
import android.os.Build;
import android.util.Log;

/**
 * This class illustrates how to query OpenSL config parameters on Jelly Bean MR1 while maintaining
 * backward compatibility with older versions of Android. The trick is to place the new API calls in
 * an inner class that will only be loaded if we're running on JB MR1 or later.
 */
public abstract class AudioSystemParams
{

    /**
     * @return The recommended sample rate in Hz.
     */
    public abstract int getSampleRate();

    /**
     * @return The recommended buffer size in frames.
     */
    public abstract int getBufferSize();

    /**
     * @param context, e.g., the current activity.
     * @return OpenSlParams instance for the given context.
     * @throws NoSuchMethodException 
     * @throws ClassNotFoundException 
     * @throws InvocationTargetException 
     * @throws IllegalAccessException 
     * @throws SecurityException 
     * @throws IllegalArgumentException 
     */
    public static AudioSystemParams createInstance(Context context) throws IllegalArgumentException, SecurityException, IllegalAccessException, InvocationTargetException, ClassNotFoundException, NoSuchMethodException
    {
        return (Build.VERSION.SDK_INT >= 17) //Build.VERSION_CODES.JELLY_BEAN_MR1)
                ? new JellyBeanMr1OpenSlParams(context)
        : null;
    }

    private AudioSystemParams()
    {
        // Not meant to be instantiated except here.
    }

    // Implementation for Jelly Bean MR1 or later.
    @TargetApi(17) //Build.VERSION_CODES.JELLY_BEAN_MR1)
    private static class JellyBeanMr1OpenSlParams extends AudioSystemParams {

        private final int sampleRate;
        private final int bufferSize;

        private JellyBeanMr1OpenSlParams(Context context) throws IllegalArgumentException, IllegalAccessException, InvocationTargetException, ClassNotFoundException, SecurityException, NoSuchMethodException {
            AudioManager am = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
            // Provide default values in case config lookup fails.
            int sr = 44100;
            int bs = 0;
            try {
                Class<?> audioManagerClass = Class.forName("android.media.AudioManager");
                Method getPropertyMethod = audioManagerClass.getMethod("getProperty", String.class);
                
                // If possible, query the native sample rate and buffer size.
                sr = Integer.parseInt((String) getPropertyMethod.invoke(am, "android.media.property.OUTPUT_SAMPLE_RATE")); //AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE));
                bs = Integer.parseInt((String) getPropertyMethod.invoke(am, "android.media.property.OUTPUT_FRAMES_PER_BUFFER")); //AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER));
            } catch (NumberFormatException e) {
                Log.w(getClass().getName(), "Failed to read native OpenSL config: " + e);
            }
            sampleRate = sr;
            bufferSize = bs;
        }

        @Override
        public int getSampleRate() {
            return sampleRate;
        }

        @Override
        public int getBufferSize() {
            return bufferSize;
        }
    };

//    // Default factory for Jelly Bean or older.
//    private static class DefaultOpenSlParams extends AudioSystemParams {
//        @Override
//        public int getSampleRate() {
//            return 44100;
//        }
//
//        @Override
//        public int getBufferSize() {
//            return 64;
//        }
//    };
}
