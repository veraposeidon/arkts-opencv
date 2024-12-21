import { hilog } from '@kit.PerformanceAnalysisKit';
import testNapi from 'libentry.so';

import { common2D, drawing } from '@kit.ArkGraphics2D';
import { image } from '@kit.ImageKit';

@Entry
@ComponentV2
struct Index {
  @Local pixelMap?: PixelMap

  async transformImage() {
    // 源图片尺寸
    const sourceWidth: number = 570;
    const sourceHeight: number = 405;
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

    const imageData = getContext().resourceManager.getMediaContentSync($r('app.media.card'));
    const imageBuffer = imageData.buffer;
    const imageSource = image.createImageSource(imageBuffer);

    const pixelMap = await imageSource.createPixelMap()

    this.pixelMap = pixelMap;

    const canvasRenderingContext: CanvasRenderingContext2D = new CanvasRenderingContext2D();
  }

  build() {
    Column() {
      Button("transform")
        .onClick(() => {
          this.transformImage()
          hilog.info(0x0000, 'testTag', 'Test NAPI 2 + 3 = %{public}d', testNapi.add(2, 3));
          console.log(testNapi.getHelloString())
        })
      Text("Source")
        .id('Source')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
      Image($r('app.media.card'))
        .width('100%')
      Text("PixelMap")
        .id('PixelMap')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
      Image(this.pixelMap)
        .width('100%')
    }
    .height('100%')
    .width('100%')
  }

}
