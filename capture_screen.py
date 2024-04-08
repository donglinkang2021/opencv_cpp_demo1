from PIL import ImageGrab, ImageTk
import tkinter as tk

class ScreenCapture:
    def __init__(self):
        self.start_x, self.start_y = 0, 0
        self.end_x, self.end_y = 0, 0
        self.bbox = None
        self.bbox_rect = None
        self.screenshot = None

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
        # 显示全屏截图
        screenshot_img = ImageTk.PhotoImage(ImageGrab.grab())
        self.canvas.create_image(self.screen_width//2, self.screen_height//2, image=screenshot_img)
        self.root.mainloop()
        return self.bbox

if __name__ == "__main__":
    # 实例化 ScreenCapture 类
    screen_capture = ScreenCapture()
    # 调用截图方法
    bbox = screen_capture.get_bbox()
    # 保存截图
    if bbox:
        screenshot = ImageGrab.grab(bbox)
        screenshot.save("screenshot.jpg", "JPEG", quality=95)
        print("截图已保存为 screenshot.jpg")
    else:
        print("未截取到有效区域")