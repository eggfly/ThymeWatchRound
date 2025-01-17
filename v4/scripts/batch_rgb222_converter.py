
import os
import sys
from PIL import Image, ImageOps

try:
    resample_filter = Image.Resampling.LANCZOS  # Pillow 9.1.0 及以上版本
except AttributeError:
    resample_filter = Image.LANCZOS  # Pillow 9.0.0 及以下版本

def convert_image(input_image_path, output_dir):
    """将输入图片转换为 64 色预览图并生成 C 头文件。"""
    base_name = os.path.splitext(os.path.basename(input_image_path))[0]
    output_header_path = os.path.join(output_dir, f"{base_name}_data.h")
    preview_image_path = os.path.join(output_dir, f"{base_name}_64colors.png")

    # 打开图像并转换为 RGB 模式
    img = Image.open(input_image_path).convert('RGB')

    # 调整大小到 240x240
    if img.size != (240, 240):
        img = ImageOps.fit(img, (240, 240), method=resample_filter, centering=(0.5, 0.5))

    # 获取像素数据
    pixels = img.load()

    # 创建保存 RGB222 数据的列表
    rgb222_data = []

    for y in range(240):
        for x in range(240):
            r, g, b = pixels[x, y]
            # 缩小颜色深度到 2 位
            r2 = (r >> 6) & 0x03  # 取最高 2 位
            g2 = (g >> 6) & 0x03
            b2 = (b >> 6) & 0x03
            # 合并为 0b00RRGGBB 格式
            rgb222 = (r2 << 4) | (g2 << 2) | b2
            rgb222_data.append(rgb222)

    # 生成 C 头文件
    with open(output_header_path, 'w') as f:
        f.write('#pragma once\n')
        f.write('#include <stdint.h>\n\n')
        f.write('uint8_t image_data[240][240] = {\n')
        for y in range(240):
            f.write('    {')
            row_data = rgb222_data[y*240:(y+1)*240]
            f.write(', '.join(f'0x{byte:02X}' for byte in row_data))
            f.write('}')
            if y != 239:
                f.write(',\n')
            else:
                f.write('\n')
        f.write('};\n')

    # 将 RGB222 数据转换回 RGB888，用于预览
    preview_img = Image.new('RGB', (240, 240))
    preview_pixels = preview_img.load()

    for y in range(240):
        for x in range(240):
            rgb222 = rgb222_data[y*240 + x]
            # 提取 R、G、B 两位数据
            r2 = (rgb222 >> 4) & 0x03
            g2 = (rgb222 >> 2) & 0x03
            b2 = rgb222 & 0x03
            # 扩展到 8 位
            r8 = (r2 << 6) | (r2 << 4) | (r2 << 2) | r2
            g8 = (g2 << 6) | (g2 << 4) | (g2 << 2) | g2
            b8 = (b2 << 6) | (b2 << 4) | (b2 << 2) | b2
            preview_pixels[x, y] = (r8, g8, b8)

    # 保存预览图像
    preview_img.save(preview_image_path)
    print(f"已生成: {output_header_path} 和 {preview_image_path}")

def main():
    # 源目录 (默认为脚本同目录下的 staged)
    staged_dir = os.path.join(os.path.dirname(__file__), 'staged')
    if not os.path.exists(staged_dir):
        print(f"未找到目录: {staged_dir}")
        sys.exit(1)

    # 目标目录 (默认为脚本同目录下的 converted)
    converted_dir = os.path.join(os.path.dirname(__file__), 'converted')
    if not os.path.exists(converted_dir):
        os.makedirs(converted_dir)

    # 支持的文件扩展名
    valid_exts = ('.webp', '.jpg', '.jpeg', '.png')

    files_found = False
    for filename in os.listdir(staged_dir):
        file_ext = os.path.splitext(filename)[1].lower()
        if file_ext in valid_exts:
            files_found = True
            input_image_path = os.path.join(staged_dir, filename)
            convert_image(input_image_path, converted_dir)

    if not files_found:
        print("在 staged 目录下没有找到 webp、jpg 或 png 文件。")

if __name__ == '__main__':
    main()


