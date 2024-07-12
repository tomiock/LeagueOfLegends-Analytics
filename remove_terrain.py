import cv2
import numpy as np
from PIL import Image


def hsv_to_bounds(hsv_value, tolerance):
    H, S, V = hsv_value

    # Convert to OpenCV HSV ranges
    H = int(H / 2)
    S = int(S / 100 * 255)
    V = int(V / 100 * 255)

    # Define bounds with tolerance
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


def update_image(image: Image.Image) -> Image.Image:
    open_cv_image = np.array(image)
    hsv = cv2.cvtColor(open_cv_image, cv2.COLOR_RGB2HSV)

    ground = (72.9, 27.3, 60.4)
    shadow = (75, 28.6, 22)
    terrain = (205.7, 100, 2.7)

    lower_bound1, upper_bound1 = hsv_to_bounds(terrain, 0)
    lower_bound2, upper_bound2 = hsv_to_bounds(shadow, 20)
    lower_bound3, upper_bound3 = hsv_to_bounds(ground, 50)

    mask1 = cv2.inRange(hsv, lower_bound1, upper_bound1)
    mask2 = cv2.inRange(hsv, lower_bound2, upper_bound2)
    mask3 = cv2.inRange(hsv, lower_bound3, upper_bound3)

    mask = cv2.bitwise_or(mask1, mask2, mask3)

    lower_bound = np.array([36 - 10, 70 - 40, 153 - 40])
    upper_bound = np.array([36 + 10, 70 + 40, 153 + 40])
    mask = cv2.inRange(hsv, lower_bound, upper_bound)

    target = cv2.bitwise_and(open_cv_image, open_cv_image, mask=mask)

    return Image.fromarray(open_cv_image - target)


if __name__ == '__main__':
    image_path = 'dataset/images/b.png'
    image = Image.open(image_path)
    updated_image = update_image(image)
    updated_image.show()
    updated_image.save('test_images/IMG_11_clean.png')
