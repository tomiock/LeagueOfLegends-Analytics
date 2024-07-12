import cv2
import numpy as np
from PIL import Image


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

    ground = (72.9, 27.3, 60.4)
    shadow = (75, 28.6, 22)
    terrain = (205.7, 100, 2.7)
    river = (193.9, 92, 58.8)

    lower_bound1, upper_bound1 = hsv_to_bounds(terrain, 10)
    lower_bound2, upper_bound2 = hsv_to_bounds(shadow, 10)
    lower_bound3, upper_bound3 = hsv_to_bounds(ground, 40)
    lower_bound4, upper_bound4 = hsv_to_bounds(river, 5)

    mask1 = cv2.inRange(hsv, lower_bound1, upper_bound1)
    mask2 = cv2.inRange(hsv, lower_bound2, upper_bound2)
    mask3 = cv2.inRange(hsv, lower_bound3, upper_bound3)
    mask4 = cv2.inRange(hsv, lower_bound4, upper_bound4)

    mask = 255*(mask1 + mask2 + mask3 + mask4)

    target = cv2.bitwise_and(open_cv_image, open_cv_image, mask=mask)
    return open_cv_image - target


if __name__ == '__main__':
    image_path = 'dataset/images/b.png'
    img = cv2.imread(image_path)
    updated_image = update_image(img)
    updated_image = Image.fromarray(updated_image)
    updated_image.show()
