import cv2
import numpy as np
from PIL import Image

GROUND = (75.7, 26.9, 61.2)
SHADOW = (78.8, 28.1, 22.4)
TERRAIN = (100, 33.3, 3.5)
RIVER = (188.9, 83.9, 75.7)
RIVER_SHADOW = (188.1, 84.3, 27.5)


def hsv_to_bounds(hsv_value, tolerance):
    H, S, V = hsv_value

    H = int(H / 2)
    S = int(S / 100 * 255)
    V = int(V / 100 * 255)

    lower_bound = (
        max(H - tolerance, 0),
        max(S - tolerance, 0),
        max(V - tolerance, 0)
    )
    upper_bound = (
        min(H + tolerance, 179),
        min(S + tolerance, 255),
        min(V + tolerance, 255)
    )

    return np.array(lower_bound), np.array(upper_bound)


def update_image(image) -> np.ndarray:
    open_cv_image = np.array(image)
    hsv = cv2.cvtColor(open_cv_image, cv2.COLOR_RGB2HSV)

    lower_bound1, upper_bound1 = hsv_to_bounds(TERRAIN, 50)
    lower_bound2, upper_bound2 = hsv_to_bounds(SHADOW, 50)
    lower_bound3, upper_bound3 = hsv_to_bounds(GROUND, 50)
    lower_bound4, upper_bound4 = hsv_to_bounds(RIVER, 50)
    lower_bound5, upper_bound5 = hsv_to_bounds(RIVER_SHADOW, 50)

    mask1 = cv2.inRange(hsv, lower_bound1, upper_bound1)
    mask2 = cv2.inRange(hsv, lower_bound2, upper_bound2)
    mask3 = cv2.inRange(hsv, lower_bound3, upper_bound3)
    mask4 = cv2.inRange(hsv, lower_bound4, upper_bound4)
    mask5 = cv2.inRange(hsv, lower_bound5, upper_bound5)

    mask = (mask1 + mask2 + mask3 + mask4 + mask5)

    target = cv2.bitwise_and(open_cv_image, open_cv_image, mask=mask)
    return open_cv_image - target


if __name__ == '__main__':
    image_path = 'test_images/IMG_11.png'
    img = cv2.imread(image_path)
    print(img.shape)
    updated_image = update_image(img)
    print(updated_image.shape)
    updated_image = Image.fromarray(updated_image)
    updated_image.show()
