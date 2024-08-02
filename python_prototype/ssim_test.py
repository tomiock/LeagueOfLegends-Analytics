import cv2
from champ_detection import ssim_similarity

detected_img = cv2.imread('test_images/diana/partial_diana.png')
detected_img_erased = cv2.imread('test_images/diana/partial_diana_erased.png')

true_img = cv2.imread('test_images/diana/diana_circle.png')
true_img_erased = cv2.imread('test_images/diana/diana_circle_erased.png')

print(ssim_similarity(detected_img, true_img))
print(ssim_similarity(detected_img_erased, true_img_erased))
