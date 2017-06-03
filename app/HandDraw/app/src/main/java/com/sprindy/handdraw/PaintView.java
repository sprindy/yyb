package com.sprindy.handdraw;


import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.PorterDuff;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class PaintView extends View {
    private Paint mPaint;
    private Path mPath;
    private Bitmap mBitmap;
    private Canvas mCanvas;
//    private BleController bleController;

    private int screenWidth, screenHeight;
    private float currentX, currentY;

    private int[][] paintArray = new int[32][32];
    public int[] fontArray = new int[128];

    int xspace;
    int yspace;

    public PaintView(Context context, int screenWidth, int screenHeight) {
        super(context);
        this.screenWidth = screenWidth;
        this.screenHeight = screenHeight;
        xspace = screenWidth/32;
        yspace = screenHeight/32;
        init();
    }

//    public BleController getBleController() {
//        return bleController;
//    }
//
//    public void setBleController(BleController bleController) {
//        this.bleController = bleController;
//    }

    private void init() {
        mPaint = new Paint();
        mPaint.setAntiAlias(true); // 去除锯齿
        mPaint.setStrokeWidth(5);
        mPaint.setStyle(Paint.Style.STROKE);
        mPaint.setColor(Color.RED);

        mPath = new Path();

        mBitmap = Bitmap.createBitmap(screenWidth, screenHeight, Config.ARGB_8888);
        mCanvas = new Canvas(mBitmap);
//      mCanvas.drawColor(Color.WHITE);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        canvas.drawBitmap(mBitmap, 0, 0, null);
        canvas.drawPath(mPath, mPaint);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        float x = event.getX();
        float y = event.getY();


        switch (event.getAction()) {
            case MotionEvent.ACTION_DOWN:
                currentX = x;
                currentY = y;
                mPath.moveTo(currentX, currentY);
                break;
            case MotionEvent.ACTION_MOVE:
                currentX = x;
                currentY = y;
                calXY((int)x,(int)y);
                mPath.quadTo(currentX, currentY, x, y); // 画线
                break;
            case MotionEvent.ACTION_UP:
                mCanvas.drawPath(mPath, mPaint);
                break;
        }

        invalidate();
        return true;
    }

    private void calXY(int pointX,int pointY)
    {
        int x = pointX/xspace;
        int y = pointY/yspace;
        Log.e("wangrui", "xspace = " + xspace +"yspace = " + yspace + "x = " + x  + "   y = " + y + "pointX = " + pointX + "pointY = " + pointY);
        if(x<32&&x>=0&&y>=0&&y<32){

            paintArray[x][y] = 1;
        }

    }

    public Bitmap getPaintBitmap() {
        return resizeImage(mBitmap, 100, 100);
    }

    public Path getPath() {
        return mPath;
    }

    // 缩放
    public Bitmap resizeImage(Bitmap bitmap, int width, int height) {
        int originWidth = bitmap.getWidth();
        int originHeight = bitmap.getHeight();

        float scaleWidth = ((float) width) / originWidth;
        float scaleHeight = ((float) height) / originHeight;

        Matrix matrix = new Matrix();
        matrix.postScale(scaleWidth, scaleHeight);
        Bitmap resizedBitmap = Bitmap.createBitmap(bitmap, 0, 0, originWidth,
                originHeight, matrix, true);


        int picWidth = resizedBitmap.getWidth();
        int picHight = resizedBitmap.getHeight();
        int pieceCount = 0;// 碎片 的个数
        int[] pixels = new int[picWidth * picHight];
        resizedBitmap.getPixels(pixels, 0, picWidth, 0, 0, picWidth, picHight);
//        Log.e("wangrui",pixels.toString());



//
//        int bytes = resizedBitmap.getByteCount();
//        ByteBuffer buffer = ByteBuffer.allocate(bytes);
//        resizedBitmap.copyPixelsToBuffer(buffer);
//        byte[] array = buffer.array();
//        Log.e("wangrui","wangrui array.length = " + array.length);
//        String s = new String();
//        try {
//            Log.e("wangrui","wangrui array = " + new String(array,"UTF-8"));
//        } catch (UnsupportedEncodingException e) {
//            e.printStackTrace();
//        }
        StringBuffer sb = new StringBuffer();
//        for (int i = 0;i < paintArray.length;i++) {
//            Log.e("wangrui", (pixels[i] >> 16 & 0xFF) +"");
//            if((pixels[i] >> 16 & 0xFF)==255)
//            {
//                sb.append("1");
//            }else{
//                sb.append("0");
//            }
//
//            //sb.append(pixels[i]+"");
//        }
////

//        }

        for(int i=0;i<32;i++)
        {
            StringBuffer sb1 = new StringBuffer();
            for (int j=0;j<32;j++)
            {
                if(paintArray[i][j]==1)
                {
                    sb1.append("*");
                }else
                {
                    sb1.append("-");
                }
//                sb.append(paintArray[i][j]);
            }
            Log.e("wangrui", sb1.toString()+"");
        }
        byte[] bytes = new byte[128];
//        int [] fontArray = new int[128];
        int k = 0;
        for(int i=0;i<32;i++)
        {
            for (int j=0;j<4;j++)
            {
                fontArray[k++] = (int) (paintArray[i][8*j] << 7
                        | paintArray[i][8*j +1] << 6
                        | paintArray[i][8*j +2] << 5
                        | paintArray[i][8*j +3] << 4
                        | paintArray[i][8*j +4]  << 3
                        | paintArray[i][8*j +5] << 2
                        | paintArray[i][8*j +6]  << 1
                        | paintArray[i][8*j +7]);
            }
        }
//        bleController.setFontArray(fontArray);
        String bin = bin2HexStr(bytes);
        Log.e("wangrui" , bin.toString());

        Log.e("wangrui", sb.toString().length()+"");
        Log.e("wangrui", sb.toString()+"");

//
//        for (int i = 1;i < array.length;i = i+4) {
//            Log.e("wangrui","wangrui " + array[i]);
//        }


        return resizedBitmap;
    }

    public int[] getFontArray() {
        return fontArray;
    }

    //清除画板
    public void clear() {
        paintArray = new int[32][32];
        if (mCanvas != null) {
            mPath.reset();
            mCanvas.drawColor(Color.TRANSPARENT, PorterDuff.Mode.CLEAR);
            invalidate();
        }
    }

    public String bin2HexStr(byte[] bytes){
        String hexStr =  "0123456789ABCDEF";
        String result = "";
        String hex = "";
        for(int i=0;i<bytes.length;i++){
            //字节高4位
            hex = String.valueOf(hexStr.charAt((bytes[i]&0xF0)>>4));
            //字节低4位
            hex += String.valueOf(hexStr.charAt(bytes[i]&0x0F));
            result = result + "0x" + hex + ",";  //+" "
        }
        return result;
    }
}
