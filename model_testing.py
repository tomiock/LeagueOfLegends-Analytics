from ultralytics import YOLO

model = YOLO('best.pt')
model.predict('test_images/IMG_9_clean.png', save=True)
