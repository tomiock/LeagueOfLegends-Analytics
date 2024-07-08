from ultralytics import YOLO

model = YOLO('train8/weights/best.pt')
model.predict('test_images/IMG_11.png', save=True)
