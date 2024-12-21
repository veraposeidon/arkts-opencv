import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

import { common2D, drawing } from '@kit.ArkGraphics2D';
import { image } from '@kit.ImageKit';

@Entry
@ComponentV2
struct Index {
  @Local sourcePixelMap?: PixelMap
  @Local destPixelMap?: PixelMap;

  async transformImage() {
    // 源图片中选择的四个角点
    const srcPoints: Array<common2D.Point> = [
      { x: 148, y: 80 },
      { x: 437, y: 114 },
      { x: 94, y: 247 },
      { x: 423, y: 288 },
    ];

    // pts2 = np.float32([[0, 0], [320, 0], [0, 178], [320, 178]])
    const dstPoints: Array<common2D.Point> = [
      { x: 0, y: 0 },
      { x: 320, y: 0 },
      { x: 0, y: 178 },
      { x: 320, y: 178 },
    ];

    let matrix = new drawing.Matrix();
    const success: boolean = matrix.setPolyToPoly(srcPoints, dstPoints, 4);
    console.log('success', success);
    if (!success) {
      return;
    }

    const sourceImageData = getContext().resourceManager.getRawFileContentSync('card.png');
    const sourceImageBuffer = sourceImageData.buffer;
    const imageSource = image.createImageSource(sourceImageBuffer);

    const pixelMap = await imageSource.createPixelMap()

    this.sourcePixelMap = pixelMap;

    const sourceImageBufferLength = await pixelMap.getPixelBytesNumber();

    const arrayBuffer = new ArrayBuffer(sourceImageBufferLength);

    await pixelMap.readPixelsToBuffer(arrayBuffer);
    const width = pixelMap.getImageInfoSync().size.width;
    const height = pixelMap.getImageInfoSync().size.height;
    const format = pixelMap.getImageInfoSync().pixelFormat;
    console.log('format', format)

    // 灰度变换
    // const result  = testNapi.processImage({
    //   width: width,
    //   height: height,
    //   data: arrayBuffer
    // });

    // 透视变换
    const result = testNapi.transformImage({
      width: width,
      height: height,
      data: arrayBuffer,
      srcPoints: srcPoints,
      dstPoints: dstPoints,
      dstWidth: 320,
      dstHeight: 178
    });

    // 使用处理后的数据
    console.info(`Processed image: ${result.cols}x${result.rows}`)

    let opts: image.InitializationOptions = {
      editable: true,
      pixelFormat: image.PixelMapFormat.RGBA_8888,
      size: { height: result.rows, width: result.cols }
    }

    image.createPixelMap(result.byteBuffer, opts, async (error, pixelMap) => {
      if (error) {
        console.error(`Failed to create pixelMap error_code ${error.code}`);
      } else {
        console.info('Succeeded in creating pixelMap.');
        this.destPixelMap = pixelMap;
      }
    })

  }

  build() {
    Column() {
      Button("transform")
        .onClick(() => {
          this.transformImage()
          hilog.info(0x0000, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(2, 3));
          console.log(testNapi.getHelloString())
        })
      // Text("Source")
      //   .id('Source')
      //   .fontSize(20)
      // Image($rawfile('card.png'))
      //   .width('100%')
      Text("PixelMap")
        .id('PixelMap')
        .fontSize(20)
      Image(this.sourcePixelMap)
        .width('100%')
      Text("DestPixelMap")
        .id('DestPixelMap')
        .fontSize(20)
      Image(this.destPixelMap)
        .width('100%')
    }
    .height('100%')
    .width('100%')
  }
}
