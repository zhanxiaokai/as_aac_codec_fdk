package media.ushow.aac_codec_fdk;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }

    private Button aacEncodeBtn;
    private Button aacDecodeBtn;

    private final String ENCODE_PCM_FILE_PATH = "/mnt/sdcard/a_songstudio/encode_pcm.pcm";
    private final String ENCODE_MONO_PCM_FILE_PATH = "/mnt/sdcard/a_songstudio/encode_mono_pcm.pcm";
    private final String ENCODE_AAC_FILE_PATH = "/mnt/sdcard/a_songstudio/encode_lc_aac.aac";
//    private final String DECODE_AAC_FILE_PATH = "/mnt/sdcard/a_songstudio/decode_aac.aac";
    private final String DECODE_AAC_FILE_PATH = "/mnt/sdcard/a_songstudio/test.aac";
    private final String DECODE_PCM_FILE_PATH = "/mnt/sdcard/a_songstudio/decode_pcm.pcm";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
        aacEncodeBtn = (Button) findViewById(R.id.aac_encode);
        aacDecodeBtn = (Button) findViewById(R.id.aac_decode);
        aacEncodeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                AACEncoder encoder = new AACEncoder();
//                int wasteTimeMills = encoder.encodeAndDecodeTest(ENCODE_PCM_FILE_PATH, DECODE_PCM_FILE_PATH);
                int wasteTimeMills = encoder.encode(ENCODE_MONO_PCM_FILE_PATH, ENCODE_AAC_FILE_PATH);
                Toast.makeText(MainActivity.this, "AAC Encoder Test waste " + wasteTimeMills + "ms", Toast.LENGTH_LONG).show();
            }
        });
        aacDecodeBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                AACDecoder decoder = new AACDecoder();
                int wasteTimeMills = decoder.decode(DECODE_AAC_FILE_PATH, DECODE_PCM_FILE_PATH);
//                int wasteTimeMills = decoder.decode(ENCODE_AAC_FILE_PATH, DECODE_PCM_FILE_PATH);
                Toast.makeText(MainActivity.this, "AAC Decoder Test waste " + wasteTimeMills + "ms", Toast.LENGTH_LONG).show();
            }
        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
