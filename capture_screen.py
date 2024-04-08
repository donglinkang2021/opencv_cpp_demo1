from PIL import ImageGrab, ImageTk
import tkinter as tk
import time
import os
import threading

class ScreenCapture:
    def __init__(self):
        self.start_x, self.start_y = 0, 0
        self.end_x, self.end_y = 0, 0
        self.bbox = None
        self.bbox_rect = None

    def on_click(self, event):
        self.start_x, self.start_y = event.x, event.y

    def on_drag(self, event):
        self.end_x, self.end_y = event.x, event.y
        # 更新边框显示
        if self.bbox_rect:
            self.canvas.delete(self.bbox_rect)
        self.bbox_rect = self.canvas.create_rectangle(self.start_x, self.start_y, self.end_x, self.end_y,
                                                      outline='red', width=2)

    def on_release(self, event):
        # 删除边框显示
        if self.bbox_rect:
            self.canvas.delete(self.bbox_rect)
        # 截取选中区域
        x1, y1 = min(self.start_x, self.end_x), min(self.start_y, self.end_y)
        x2, y2 = max(self.start_x, self.end_x), max(self.start_y, self.end_y)
        self.bbox = (x1, y1, x2, y2)
        self.root.destroy()  # 关闭窗口

    def get_bbox(self):
        # 创建一个 Tkinter 窗口
        self.root = tk.Tk()
        self.root.title("屏幕区域截取工具")
        self.root.attributes('-fullscreen', True)  # 全屏
        self.root.attributes('-topmost', True)  # 确保窗口在最顶层

        # 获取屏幕尺寸
        self.screen_width = self.root.winfo_screenwidth()
        self.screen_height = self.root.winfo_screenheight()

        # 创建一个全屏的画布
        self.canvas = tk.Canvas(self.root, width=self.screen_width, height=self.screen_height, bg='white')
        self.canvas.pack()

        # 绑定鼠标事件
        self.canvas.bind("<ButtonPress-1>", self.on_click)  # 鼠标左键按下
        self.canvas.bind("<B1-Motion>", self.on_drag)  # 拖拽过程中
        self.canvas.bind("<ButtonRelease-1>", self.on_release)  # 鼠标左键释放

        # 显示全屏截图
        screenshot_img = ImageTk.PhotoImage(ImageGrab.grab())
        self.canvas.create_image(self.screen_width//2, self.screen_height//2, image=screenshot_img)
        self.root.mainloop()
        return self.bbox
    
class TimedScreenCapture:
    def __init__(self, interval, save_folder, bbox):
        super().__init__()
        self.bbox = bbox  # 截图区域
        self.interval = interval  # 截图间隔时间（秒）
        self.save_folder = save_folder  # 图片保存路径

        if not os.path.exists(self.save_folder):
            os.makedirs(self.save_folder)

        self.count = 1  # 截图计数器

    def save_screenshot(self):
        self.screenshot = ImageGrab.grab(self.bbox)
        save_path = f"{self.save_folder}/screenshot_{self.count:05d}.jpg"
        self.screenshot.save(save_path, "JPEG", quality=95)
        print(f"保存截图：{save_path}")
        self.count += 1


if __name__ == "__main__":
    bbox = ScreenCapture().get_bbox()
    print(f"截图区域：{bbox}")

    frame_rate = 30  # 设置帧率为 30
    interval = 1 / frame_rate  # 间隔时间
    save_folder = "../data/Sample3"  # 图片保存路径
    capture = TimedScreenCapture(interval, save_folder, bbox)

    while True:
        capture.save_screenshot()
        time.sleep(interval)
