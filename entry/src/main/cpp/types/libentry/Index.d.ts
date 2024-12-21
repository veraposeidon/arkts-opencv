import resourceManager from '@ohos.resourceManager';

export interface ImageBuffer {
  width: number;
  height: number;
  data: ArrayBuffer;
}

export interface PixelInfo {
  rows: number;
  cols: number;
  buffSize: number;
  byteBuffer: ArrayBuffer;
}

export const add: (a: number, b: number) => number;
export const getHelloString: () => string;
export const processImage: (imageBuffer: ImageBuffer) => PixelInfo;
