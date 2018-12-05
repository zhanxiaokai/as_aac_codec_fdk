package media.ushow.aac_codec_fdk;

/**
 * Created by xiaokai.zhan on 2018/5/31.
 */

public class AACDecoder {

    public native int decode(String aacFilePath, String pcmFilePath);

}
