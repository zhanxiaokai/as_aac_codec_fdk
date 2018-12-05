package media.ushow.aac_codec_fdk;

/**
 * Created by xiaokai.zhan on 2018/5/31.
 */

public class AACEncoder {

    public native int encode(String pcmFilePath, String aacFilePath);

    public native int encodeAndDecodeTest(String pcmFilePath, String pcmPath2);

}
