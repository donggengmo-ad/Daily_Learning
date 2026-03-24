import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageDraw, ImageTk
import torch
from torch import nn
from torchvision import transforms


class NetWork(nn.Module):
    def __init__(self):
        super().__init__()
        self.layer1 = nn.Linear(784, 256)
        self.layer2 = nn.Linear(256, 10)
        
    def forward(self, x):
        x = x.view(-1, 28*28)
        x = self.layer1(x)
        x = torch.relu(x)
        x = self.layer2(x)
        return x


def normalize():
    transform = transforms.Compose([
        transforms.Grayscale(num_output_channels=1),
        transforms.Resize((28, 28)),
        transforms.ToTensor(),
        transforms.Normalize((0.5,), (0.5,))
    ])
    return transform


class DigitRecognizerGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Handwritten Digit Recognizer")
        self.root.geometry("600x700")
        
        self.model = NetWork()
        self.model.load_state_dict(torch.load("D:\\Coding\\Programme\\Python\\机器学习\\MLP\\mnist.pth", map_location=torch.device('cpu')))
        self.model.eval()
        self.transform = normalize()
        
        self.drawing = False
        self.last_x = None
        self.last_y = None
        
        self.canvas_image = Image.new("RGB", (280, 280), "white")
        self.draw = ImageDraw.Draw(self.canvas_image)
        
        self.setup_ui()
        
    def setup_ui(self):
        title_label = tk.Label(self.root, text="Handwritten Digit Recognition", font=("Arial", 18, "bold"))
        title_label.pack(pady=10)
        
        self.canvas = tk.Canvas(self.root, width=280, height=280, bg="white", bd=2, relief=tk.SUNKEN)
        self.canvas.pack(pady=10)
        
        self.canvas.bind("<Button-1>", self.start_draw)
        self.canvas.bind("<B1-Motion>", self.draw_line)
        self.canvas.bind("<ButtonRelease-1>", self.stop_draw)
        
        button_frame = tk.Frame(self.root)
        button_frame.pack(pady=10)
        
        upload_btn = tk.Button(button_frame, text="Upload Image", command=self.upload_image, 
                               width=15, height=2, font=("Arial", 12))
        upload_btn.pack(side=tk.LEFT, padx=5)
        
        clear_btn = tk.Button(button_frame, text="Clear Canvas", command=self.clear_canvas,
                              width=15, height=2, font=("Arial", 12))
        clear_btn.pack(side=tk.LEFT, padx=5)
        
        predict_btn = tk.Button(button_frame, text="Recognize Digit", command=self.predict_digit,
                               width=15, height=2, font=("Arial", 12), bg="#4CAF50", fg="white")
        predict_btn.pack(side=tk.LEFT, padx=5)
        
        result_label = tk.Label(self.root, text="Recognition Result:", font=("Arial", 14))
        result_label.pack(pady=10)
        
        self.result_var = tk.StringVar()
        self.result_var.set("-")
        result_display = tk.Label(self.root, textvariable=self.result_var, 
                                   font=("Arial", 48, "bold"), fg="#2196F3")
        result_display.pack(pady=10)
        
    def start_draw(self, event):
        self.drawing = True
        self.last_x = event.x
        self.last_y = event.y
        
    def draw_line(self, event):
        if self.drawing and self.last_x is not None and self.last_y is not None:
            self.canvas.create_line(self.last_x, self.last_y, event.x, event.y, 
                                    width=15, fill="black", capstyle=tk.ROUND, smooth=tk.TRUE)
            self.draw.line([(self.last_x, self.last_y), (event.x, event.y)], 
                          fill="black", width=15)
            self.last_x = event.x
            self.last_y = event.y
            
    def stop_draw(self, event):
        self.drawing = False
        self.last_x = None
        self.last_y = None
        
    def clear_canvas(self):
        self.canvas.delete("all")
        self.canvas_image = Image.new("RGB", (280, 280), "white")
        self.draw = ImageDraw.Draw(self.canvas_image)
        self.result_var.set("-")
        
    def upload_image(self):
        file_path = filedialog.askopenfilename(
            filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp")]
        )
        if file_path:
            try:
                img = Image.open(file_path)
                img = img.resize((280, 280))
                self.canvas_image = img.copy()
                self.draw = ImageDraw.Draw(self.canvas_image)
                self.canvas.delete("all")
                photo = ImageTk.PhotoImage(img)
                self.canvas.create_image(0, 0, anchor=tk.NW, image=photo)
                self.canvas.image = photo
                self.result_var.set("-")
            except Exception as e:
                messagebox.showerror("Error", f"Could not load image: {str(e)}")
                
    def predict_digit(self):
        try:
            img = self.canvas_image.convert("L")
            img = Image.eval(img, lambda x: 255 - x)
            img = self.transform(img)
            
            with torch.no_grad():
                output = self.model(img)
                probabilities = torch.softmax(output, dim=1)
                predict = output.argmax(dim=1).item()
                confidence = probabilities[0][predict].item() * 100
                
            self.result_var.set(f"{predict}")
            messagebox.showinfo("Recognition Result", f"Recognized Digit: {predict}\nConfidence: {confidence:.2f}%")
        except Exception as e:
            messagebox.showerror("Error", f"Recognition failed: {str(e)}")


if __name__ == "__main__":
    root = tk.Tk()
    app = DigitRecognizerGUI(root)
    root.mainloop()
