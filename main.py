from ultralytics import YOLO

# Load model
model = YOLO('download/last.pt')

model.predict('test_images/IMG_4.png', save=True)
