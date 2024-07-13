import os
import cv2
import numpy as np

from object_extraction import champ_extraction
from detect_circles import draw_circles
from skimage.metrics import structural_similarity as ssim


def crop_champs(circles) -> list:
    detected_champs = []
    for pt in circles[0, :]:
        a, b, r = pt[0], pt[1], pt[2]

        a, b, r = int(a), int(b), int(r)

        # crop the image
        crop_img = img[b - r:b + r, a - r:a + r]
        detected_champs.append(crop_img)

    return detected_champs


def resize_image(image, size):
    return cv2.resize(image, size, interpolation=cv2.INTER_AREA)


def ssim_similarity(img1, img2):
    # Convert images to grayscale
    if len(img1.shape) == 3:
        img1 = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
    if len(img2.shape) == 3:
        img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)

    # Resize images to the same size
    size = (min(img1.shape[1], img2.shape[1]),
            min(img1.shape[0], img2.shape[0]))
    img1 = resize_image(img1, size)
    img2 = resize_image(img2, size)

    # Compute SSIM
    score, _ = ssim(img1, img2, full=True)
    return score


def compare_champs(detected):
    results = []
    for file in os.listdir('reference_images/champions/'):
        champ = cv2.imread(f'reference_images/champions/{file}')

        difference = ssim_similarity(detected, champ)

        result = (file, np.mean(difference))
        results.append(result)

    results.sort(key=lambda x: x[1])
    results.reverse()
    return results[0][0]


def draw_labels(img, circles, result):

    a, b, r = circles[0], circles[1], circles[2]
    a, b, r = int(a), int(b), int(r)

    cv2.putText(img, result, (a - r, b - r - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    return img


def detect_champs(img):
    circles = champ_extraction(img, 31)
    detected_champs = crop_champs(circles)

    img = draw_circles(img, circles)

    detected_circles = np.uint16(np.around(circles))
    for i, pt in enumerate(detected_circles[0, :]):
        result = compare_champs(detected_champs[i])
        img = draw_labels(img, pt, result)

    return img


for file in os.listdir('test_images/'):
    img = cv2.imread(f'test_images/{file}')
    img = detect_champs(img)

    cv2.imshow("Detected Champions", img)
    cv2.waitKey(0)
