import os
import cv2
import numpy as np

from object_extraction import champ_extraction
from detect_circles import draw_circles
from skimage.metrics import structural_similarity as ssim


def crop_champs(circles, tolerance) -> list:
    detected_champs = []
    for pt in circles[0, :]:
        a, b, r = pt[0], pt[1], pt[2]

        a, b, r = int(a), int(b), int(r)

        crop_img = img[b - r - tolerance: b + r +
                       tolerance, a - r - tolerance:a + r + tolerance]
        detected_champs.append(crop_img)

    return detected_champs


def crop_icon(img):
    center_x, center_y = img.shape[1] // 2, img.shape[0] // 2
    radius = min(center_x, center_y)

    mask = np.zeros((img.shape[0], img.shape[1]), dtype=np.uint8)
    cv2.circle(mask, (center_x, center_y), radius, 255, -1)
    masked_img = cv2.bitwise_and(img, img, mask=mask)

    return masked_img


def resize_image(image, size):
    return cv2.resize(image, size, interpolation=cv2.INTER_AREA)


def ssim_similarity(img1, img2):
    size = (min(img1.shape[1], img2.shape[1]),
            min(img1.shape[0], img2.shape[0]))
    img1 = resize_image(img1, size)
    img2 = resize_image(img2, size)

    ssim_scores = []
    for i in range(3):
        score, _ = ssim(img1[:, :, i], img2[:, :, i], full=True,
                        gaussian_weights=True)
        ssim_scores.append(score)

    avg_ssim_score = np.mean(ssim_scores)
    return avg_ssim_score


def compare_champs(detected):
    results = []
    for file in os.listdir('reference_images/champions/'):
        champ = cv2.imread(f'reference_images/champions/{file}')
        champ = crop_icon(champ)

        difference = ssim_similarity(detected, champ)

        result = (file, np.mean(difference))
        results.append(result)

    results.sort(key=lambda x: x[1])
    results.reverse()

    return results[0][0]


def draw_label(img, circles, result):

    a, b, r = circles[0], circles[1], circles[2]
    a, b, r = int(a), int(b), int(r)

    cv2.putText(img, result, (a - r, b - r - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    return img


def detect_champs(img, radius):
    circles = champ_extraction(img, radius)
    detected_champs = crop_champs(circles, -3)

    detected_circles = np.uint16(np.around(circles))
    for i, pt in enumerate(detected_circles[0, :]):
        detected = crop_icon(detected_champs[i])

        result = compare_champs(detected)
        img = draw_label(img, pt, result)

    img = draw_circles(img, detected_circles)

    return img


if __name__ == '__main__':
    for file in os.listdir('test_images/'):
        if file.endswith('.png'):
            img = cv2.imread(f'test_images/{file}')
            img = detect_champs(img, 31)

            cv2.imshow("Detected Champions", img)
            cv2.waitKey(0)
