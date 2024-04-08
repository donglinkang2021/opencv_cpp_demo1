from PIL import ImageGrab, ImageTk
import tkinter as tk

# 定义全局变量，用于存储鼠标点击和释放时的坐标
start_x, start_y = 0, 0
end_x, end_y = 0, 0

def on_click(event):
    global start_x, start_y
    start_x, start_y = event.x, event.y

def on_release(event):
    global end_x, end_y
    end_x, end_y = event.x, event.y
    # 截取选中区域
    x1, y1 = min(start_x, end_x), min(start_y, end_y)
    x2, y2 = max(start_x, end_x), max(start_y, end_y)
    screenshot = ImageGrab.grab(bbox=(x1, y1, x2, y2))
    screenshot.save("selected_area.jpg")
    print("选取区域已保存为 selected_area.jpg")
    root.destroy()  # 关闭窗口

# 创建一个 Tkinter 窗口
root = tk.Tk()
root.title("屏幕区域截取工具")
root.attributes('-topmost', True)  # 确保窗口在最顶层

# 获取屏幕尺寸
screen_width = root.winfo_screenwidth()
screen_height = root.winfo_screenheight()

# 创建一个全屏的画布
canvas = tk.Canvas(root, width=screen_width, height=screen_height, bg='white')
canvas.pack()

# 绑定鼠标事件
canvas.bind("<ButtonPress-1>", on_click)  # 鼠标左键按下
canvas.bind("<ButtonRelease-1>", on_release)  # 鼠标左键释放

# 显示全屏截图
screenshot = ImageGrab.grab()

# 将截图显示在画布上
# 这里需要将 PIL.Image 转换为 Tkinter.PhotoImage
screenshot = ImageTk.PhotoImage(screenshot)
canvas.create_image(screen_width//2, screen_height//2, image=screenshot)


# 运行窗口主循环
root.mainloop()
