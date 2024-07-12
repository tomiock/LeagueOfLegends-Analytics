from detect_circles import detect_circles
from remove_terrain import update_image

import os
import cv2


def champ_extraction(image):
    img_clean = update_image(image)
    circles = detect_circles(img_clean, radius=20, param1=300, param2=13)
    return circles


if __name__ == '__main__':
    dir = 'dataset/images'

    count = 0
    for filename in os.listdir(dir):
        if filename.endswith('.png'):
            img = cv2.imread(os.path.join(dir, filename))
            img = champ_extraction(img)
            cv2.imshow('image', img)
            cv2.waitKey(0)
        count += 1

        if count == 10:
            break
