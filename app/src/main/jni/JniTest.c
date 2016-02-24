#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>


#define  LOG_TAG    "libibmphotophun"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)



/***
 * uint8_t max 255
 *
 * */
typedef struct {
    uint8_t alpha;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} argb;


int abs(int x);

JNIEXPORT void JNICALL
Java_example_myapplication_1test_MainActivity_convertToGray
        (JNIEnv *env, jobject obj, jobject bitmapcolor, jobject bitmapgray) {

    AndroidBitmapInfo infocolor;
    void *pixelscolor;
    AndroidBitmapInfo infogray;
    void *pixelsgray;
    int ret;
    int y;
    int x;

    LOGI("convertToGray");
    if ((ret = AndroidBitmap_getInfo(env, bitmapcolor, &infocolor)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }


    if ((ret = AndroidBitmap_getInfo(env, bitmapgray, &infogray)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }


    LOGI("color image :: width is %d; height is %d; stride is %d; format is %d;flags is%d",
         infocolor.width, infocolor.height, infocolor.stride, infocolor.format, infocolor.flags);
    if (infocolor.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {

        LOGE("Bitmap format is not RGBA_8888 !");
        return;
    }


    LOGI("gray image :: width is %d; height is %d; stride is %d; format is %d;flags is%d",
         infogray.width, infogray.height, infogray.stride, infogray.format, infogray.flags);
    if (infogray.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }


    if ((ret = AndroidBitmap_lockPixels(env, bitmapcolor, &pixelscolor)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmapgray, &pixelsgray)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    // modify pixels with image processing algorithm

    for (y = 0; y < infocolor.height; y++) {
        argb *line = (argb *) pixelscolor;
        uint8_t *grayline = (uint8_t *) pixelsgray;
        for (x = 0; x < infocolor.width; x++) {
            grayline[x] = 0.3 * line[x].red + 0.59 * line[x].green + 0.11 * line[x].blue;

//            if (x == 100 || x == 101 || x == 102 || x == 103 || x == 104 || x == 105) {
//                grayline[x] = 255;
//
//
//            }
        }


        pixelscolor = (char *) pixelscolor + infocolor.stride;
        pixelsgray = (char *) pixelsgray + infogray.stride;
    }

    LOGI("unlocking pixels");
    AndroidBitmap_unlockPixels(env, bitmapcolor);
    AndroidBitmap_unlockPixels(env, bitmapgray);


}

JNIEXPORT void JNICALL
Java_example_myapplication_1test_MainActivity_changeBrightness(JNIEnv *env, jobject obj,
                                                               int direction, jobject bitmap) {

    AndroidBitmapInfo infogray;
    void *pixelsgray;
    int ret;
    int y;
    int x;
    uint8_t save;


    if ((ret = AndroidBitmap_getInfo(env, bitmap, &infogray)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }


    LOGI("gray image :: width is %d; height is %d; stride is %d; format is %d;flags is%d",
         infogray.width, infogray.height, infogray.stride, infogray.format, infogray.flags);
    if (infogray.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }


    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixelsgray)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

// modify pixels with image processing algorithm


    LOGI("time to modify pixels....");
    for (y = 0; y < infogray.height; y++) {
        uint8_t *grayline = (uint8_t *) pixelsgray;
        int v;
        for (x = 0; x < infogray.width; x++) {
            v = (int) grayline[x];

            if (direction == 1)
                v -= 5;
            else
                v += 5;
            if (v >= 255) {
                grayline[x] = 255;
            } else if (v <= 0) {
                grayline[x] = 0;
            } else {
                grayline[x] = (uint8_t) v;
            }
        }

        pixelsgray = (char *) pixelsgray + infogray.stride;
    }

    AndroidBitmap_unlockPixels(env, bitmap);

}

JNIEXPORT void JNICALL
Java_example_myapplication_1test_MainActivity_findEdges(JNIEnv
                                                        *env, jobject obj, jobject bitmapgray,
                                                        jobject bitmapedges) {
    AndroidBitmapInfo infogray;
    void *pixelsgray;
    AndroidBitmapInfo infoedges;
    void *pixelsedge;
    int ret;
    int y;
    int x;
    int sumX, sumY, sum;
    int i, j;
    int Gx[3][3];
    int Gy[3][3];
    uint8_t *graydata;
    uint8_t *edgedata;


    LOGI("findEdges running");
//
    Gx[0][0] = -1;
    Gx[0][1] = 0;
    Gx[0][2] = 1;
    Gx[1][0] = -2;
    Gx[1][1] = 0;
    Gx[1][2] = 2;
    Gx[2][0] = -1;
    Gx[2][1] = 0;
    Gx[2][2] = 1;



//    Gx[0][0] = -1;
//    Gx[0][1] = 0;
//    Gx[0][2] = 0;
//    Gx[1][0] = -2;
//    Gx[1][1] = 0;
//    Gx[1][2] = 0;
//    Gx[2][0] = -1;
//    Gx[2][1] = 0;
//    Gx[2][2] = 0;


    Gy[0][0] = 1;
    Gy[0][1] = 2;
    Gy[0][2] = 1;
    Gy[1][0] = 0;
    Gy[1][1] = 0;
    Gy[1][2] = 0;
    Gy[2][0] = -1;
    Gy[2][1] = -2;
    Gy[2][2] = -1;


    if ((ret = AndroidBitmap_getInfo(env, bitmapgray, &infogray)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }


    if ((ret = AndroidBitmap_getInfo(env, bitmapedges, &infoedges)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }


    LOGI("gray image :: width is %d; height is %d; stride is %d; format is %d;flags is%d",
         infogray.width, infogray.height, infogray.stride, infogray.format, infogray.flags);
    if (infogray.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }

    LOGI("color image :: width is %d; height is %d; stride is %d; format is %d;flags is%d",
         infoedges.width, infoedges.height, infoedges.stride, infoedges.format, infoedges.flags);
    if (infoedges.format != ANDROID_BITMAP_FORMAT_A_8) {
        LOGE("Bitmap format is not A_8 !");
        return;
    }


    if ((
                ret = AndroidBitmap_lockPixels(env, bitmapgray, &pixelsgray)
        ) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    if ((
                ret = AndroidBitmap_lockPixels(env, bitmapedges, &pixelsedge)
        ) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }


// modify pixels with image processing algorithm


    LOGI("time to modify pixels....");

    graydata = (uint8_t *) pixelsgray;
    edgedata = (uint8_t *) pixelsedge;

    for (
            y = 0;
            y <= infogray.height - 1; y++) {
        for (
                x = 0;
                x < infogray.width - 1; x++) {
            sumX = 0;
            sumY = 0;
// check boundaries
            //排除邊角
            if (y == 0 || y == infogray.height - 1) {
                sum = 0;
            } else if (x == 0 || x == infogray.width - 1) {
                sum = 0;
            } else {
                //其餘色階
// calc X gradient
                //3
                for (
                        i = -1;
                        i <= 1; i++) {
                    for (
                        //3
                            j = -1;
                            j <= 1; j++) {

                        //sumx 累加 ＝(指標＋寬度+(-1,0,1)+寬度＊邊)＊gx
                        sumX += (int) ((*(graydata + x + i + (y + j) * infogray.stride)) * 1);
                    }
                }

// calc Y gradient
                for (
                        i = -1;
                        i <= 1; i++) {
                    for (
                            j = -1;
                            j <= 1; j++) {
                        sumY += (int) ((*(graydata + x + i + (y + j) * infogray.stride)) * Gy[i + 1][j + 1]);
                    }
                }

                sum = abs(sumX) + abs(sumY);
                LOGI("sumx = %d" ,sumX);
                LOGI("sumy = %d",sumY);

            }

            if (sum > 255)
                sum = 255;
            if (sum < 0)
                sum = 0;



            *(edgedata + x + y * infogray.width) = 255 - (uint8_t) sum;


        }
    }

    AndroidBitmap_unlockPixels(env, bitmapgray);
    AndroidBitmap_unlockPixels(env, bitmapedges);


}

